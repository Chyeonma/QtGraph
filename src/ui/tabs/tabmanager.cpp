#include "tabmanager.h"
#include "editor/view/codeviewer.h"
#include "ui/canvas/canvasview.h"
#include "ui/welcome/welcometab.h"
#include "services/fileservice.h"
#include "services/editorservice.h"
#include "interfaces/ifontprovider.h"
#include "core/utils/pathutils.h"
#include "interfaces/ihighlighterfactory.h"
#include "ui/settings/settingstab.h"
#include "services/settingsservice.h"
#include "app/appcontext.h"
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTabBar>
#include <algorithm>

const QString TabManager::CANVAS_MARKER = "##canvas##";
const QString TabManager::SETTINGS_MARKER = "##settings##";
const QString TabManager::WELCOME_MARKER = "##welcome##";

TabManager::TabManager(QTabWidget *tabWidget, AppContext *context, QObject *parent)
    : QObject(parent)
    , tabWidget(tabWidget)
    , appContext(context)
    , fileService(context->fileService())
    , editorService(context->editorService())
    , fontProvider(context->fontProvider())
    , highlighterFactory(context->highlighterFactory())
    , settingsService(context->settingsService())
    , canvasView(nullptr)
    , settingsTab(nullptr)
    , welcomeTab(nullptr)
{
    // Cấu hình TabWidget
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; background: #1e1e1e; }"
        "QTabBar::tab { background: #2d2d2d; color: #cccccc; padding: 6px 14px; border: none; border-right: 1px solid #1e1e1e; }"
        "QTabBar::tab:selected { background: #1e1e1e; color: #ffffff; }"
        "QTabBar::tab:hover { background: #3c3c3c; }"
    );

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &TabManager::closeTab);
    tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabWidget->tabBar(), &QWidget::customContextMenuRequested, this, &TabManager::onTabContextMenuRequested);
    connect(settingsService, &SettingsService::settingsChanged, this, [this]() {
        applyEditorFontToOpenTabs();
    });

    // Handle external file changes securely managed by EditorService
    connect(editorService, &EditorService::externalFileChanged, this, [this](const QString &filePath) {
        const int tabIndex = findTabByPath(filePath);
        if (tabIndex == -1) return;

        CodeViewer *viewer = qobject_cast<CodeViewer*>(this->tabWidget->widget(tabIndex));
        if (!viewer || !viewer->editableContent()) return;

        this->tabWidget->setCurrentIndex(tabIndex);

        QMessageBox messageBox(this->tabWidget);
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setWindowTitle("External File Change");
        messageBox.setText("File \"" + viewer->displayName() + "\" was modified outside the app.");
        messageBox.setInformativeText("Choose reload to use the external change, or keep your current editing buffer and ignore that external change.");

        QPushButton *keepEditingButton = messageBox.addButton("Keep My Changes", QMessageBox::RejectRole);
        QPushButton *reloadButton = messageBox.addButton("Reload From Disk", QMessageBox::AcceptRole);
        messageBox.exec();

        if (messageBox.clickedButton() == reloadButton) {
            reloadViewerFromDisk(viewer);
        } else if (messageBox.clickedButton() == keepEditingButton) {
            this->editorService->watchFilePath(filePath);
        }
    });
}

void TabManager::openFile(const QString &filePath, const QString &fileName)
{
    // Nếu file đã mở → focus
    int existingTab = findTabByPath(filePath);
    if (existingTab != -1) {
        tabWidget->setCurrentIndex(existingTab);
        return;
    }

    OpenFileResult result = editorService->prepareFile(filePath, fileName, true);
    
    if (result.status == OpenFileResult::ReadError) {
        QMessageBox::warning(nullptr, "Lỗi", "Không thể mở file: " + fileName);
        return;
    }

    QString contentToLoad = result.editable ? result.content : result.previewMessage;

    CodeViewer *editor = createTextViewer(contentToLoad, filePath, fileName, result.editable);

    // Gắn syntax highlighter (nếu hỗ trợ ngôn ngữ)
    if (result.editable) {
        highlighterFactory->createForFile(filePath, editor->document());
    }

    int tabIndex = tabWidget->addTab(editor, fileName);
    tabWidget->setTabToolTip(tabIndex, filePath);
    tabWidget->setCurrentIndex(tabIndex);

    connect(editor->document(), &QTextDocument::modificationChanged, this, [this, editor]() {
        updateTabTitle(editor);
    });

    if (result.editable) {
        editorService->watchFilePath(filePath);
    }
}

bool TabManager::saveCurrentTab()
{
    return saveViewer(currentViewer());
}

bool TabManager::saveAllTabs()
{
    bool allSaved = true;

    for (int i = 0; i < tabWidget->count(); ++i) {
        if (CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(i))) {
            if (viewer->editableContent() && viewer->document()->isModified()) {
                allSaved = saveViewer(viewer) && allSaved;
            }
        }
    }

    return allSaved;
}

bool TabManager::confirmCloseAllTabs()
{
    for (int i = tabWidget->count() - 1; i >= 0; --i) {
        if (!maybeSaveTab(i)) {
            return false;
        }
    }

    return true;
}

void TabManager::undoCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        if (viewer->editableContent()) {
            viewer->undo();
        }
    }
}

void TabManager::redoCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        if (viewer->editableContent()) {
            viewer->redo();
        }
    }
}

void TabManager::cutCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        if (viewer->editableContent()) {
            viewer->cut();
        }
    }
}

void TabManager::copyCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        viewer->copy();
    }
}

void TabManager::pasteCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        if (viewer->editableContent()) {
            viewer->paste();
        }
    }
}

void TabManager::selectAllInCurrentTab()
{
    if (CodeViewer *viewer = currentViewer()) {
        viewer->selectAll();
    }
}

CanvasView* TabManager::openCanvas()
{
    // Nếu đã mở → focus và trả về
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabToolTip(i) == CANVAS_MARKER) {
            tabWidget->setCurrentIndex(i);
            return canvasView;
        }
    }

    // Tạo mới
    canvasView = new CanvasView();
    int tabIndex = tabWidget->addTab(canvasView, "🎨 Canvas");
    tabWidget->setTabToolTip(tabIndex, CANVAS_MARKER);
    tabWidget->setCurrentIndex(tabIndex);

    return canvasView;
}

SettingsTab* TabManager::openSettings()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabToolTip(i) == SETTINGS_MARKER) {
            tabWidget->setCurrentIndex(i);
            return settingsTab;
        }
    }

    settingsTab = new SettingsTab(fontProvider, settingsService);
    int tabIndex = tabWidget->addTab(settingsTab, "Setting");
    tabWidget->setTabToolTip(tabIndex, SETTINGS_MARKER);
    tabWidget->setCurrentIndex(tabIndex);

    return settingsTab;
}

WelcomeTab* TabManager::openWelcome()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabToolTip(i) == WELCOME_MARKER) {
            tabWidget->setCurrentIndex(i);
            return welcomeTab;
        }
    }

    welcomeTab = new WelcomeTab();
    int tabIndex = tabWidget->addTab(welcomeTab, "Welcome");
    tabWidget->setTabToolTip(tabIndex, WELCOME_MARKER);
    tabWidget->setCurrentIndex(tabIndex);

    return welcomeTab;
}

int TabManager::findTabByPath(const QString &filePath)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabToolTip(i) == filePath) {
            return i;
        }
    }
    return -1;
}

CodeViewer* TabManager::currentViewer() const
{
    return qobject_cast<CodeViewer*>(tabWidget->currentWidget());
}

void TabManager::handlePathRenamed(const QString &oldPath, const QString &newPath)
{
    const QList<int> indexes = affectedFileTabIndexes(oldPath);

    for (int index : indexes) {
        CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(index));
        if (!viewer) {
            continue;
        }

        const QString oldViewerPath = viewer->filePath();
        const QString remappedPath = PathUtils::remapPathAfterRename(oldViewerPath, oldPath, newPath);

        editorService->renameWatchedPath(oldViewerPath, remappedPath);

        viewer->updateFileContext(remappedPath, QFileInfo(remappedPath).fileName());
        tabWidget->setTabToolTip(index, remappedPath);
        updateTabTitle(viewer);
    }
}

bool TabManager::confirmPathRemoval(const QString &path, QWidget *dialogParent) const
{
    const QList<int> indexes = affectedFileTabIndexes(path);
    QStringList modifiedTabs;

    for (int index : indexes) {
        CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(index));
        if (viewer && viewer->document()->isModified()) {
            modifiedTabs.append(viewer->displayName());
        }
    }

    if (modifiedTabs.isEmpty()) {
        return true;
    }

    QMessageBox messageBox(dialogParent ? dialogParent : tabWidget);
    messageBox.setIcon(QMessageBox::Warning);
    messageBox.setWindowTitle("Unsaved Changes");
    messageBox.setText("Deleting this path will close open tabs with unsaved changes.");

    QString detail = "Modified tabs: " + modifiedTabs.join(", ") + ".";
    if (modifiedTabs.size() > 3) {
        detail = QString("There are %1 modified tabs inside this path.").arg(modifiedTabs.size());
    }

    messageBox.setInformativeText(detail + " Continue and discard those unsaved edits?");
    QPushButton *continueButton = messageBox.addButton("Delete and Close Tabs", QMessageBox::AcceptRole);
    messageBox.addButton(QMessageBox::Cancel);
    messageBox.exec();
    return messageBox.clickedButton() == continueButton;
}

void TabManager::handlePathRemoved(const QString &path)
{
    QList<int> indexes = affectedFileTabIndexes(path);
    std::sort(indexes.begin(), indexes.end(), std::greater<int>());

    for (int index : indexes) {
        QWidget *widget = tabWidget->widget(index);
        CodeViewer *viewer = qobject_cast<CodeViewer*>(widget);

        if (viewer) {
            editorService->unwatchFilePath(viewer->filePath());
        }

        tabWidget->removeTab(index);
        delete widget;
    }
}

void TabManager::closeTab(int index)
{
    if (!maybeSaveTab(index)) {
        return;
    }

    QWidget *widget = tabWidget->widget(index);
    CodeViewer *viewer = qobject_cast<CodeViewer*>(widget);

    if (viewer && viewer->editableContent()) {
        editorService->unwatchFilePath(viewer->filePath());
    }

    if (tabWidget->tabToolTip(index) == CANVAS_MARKER) {
        canvasView = nullptr;
    } else if (tabWidget->tabToolTip(index) == SETTINGS_MARKER) {
        settingsTab = nullptr;
    } else if (tabWidget->tabToolTip(index) == WELCOME_MARKER) {
        welcomeTab = nullptr;
    }

    tabWidget->removeTab(index);
    delete widget;
}

void TabManager::applyEditorFontToOpenTabs()
{
    const AppSettings &currentSettings = settingsService->settings();
    const QFont editorFont = fontProvider->editorFont(currentSettings.editorFontFamily, currentSettings.editorFontSize);

    for (int i = 0; i < tabWidget->count(); ++i) {
        if (CodeViewer *editor = qobject_cast<CodeViewer*>(tabWidget->widget(i))) {
            editor->setFont(editorFont);
        }
    }
}

CodeViewer* TabManager::createTextViewer(const QString &content, const QString &filePath, const QString &fileName, bool supportedContent)
{
    CodeViewer *viewer = new CodeViewer();
    viewer->configureFileContext(filePath, fileName, supportedContent);
    viewer->setPlainText(content);

    const AppSettings &currentSettings = settingsService->settings();
    viewer->setFont(fontProvider->editorFont(currentSettings.editorFontFamily, currentSettings.editorFontSize));
    viewer->setStyleSheet(
        "QPlainTextEdit { background: #1e1e1e; color: #d4d4d4; "
        "border: none; padding: 8px; }"
    );
    viewer->setLineWrapMode(QPlainTextEdit::NoWrap);
    viewer->setLineNumbersVisible(settingsService->settings().defaultShowLineNumbers);
    viewer->document()->setModified(false);
    return viewer;
}

int TabManager::findTabByViewer(CodeViewer *viewer) const
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->widget(i) == viewer) {
            return i;
        }
    }

    return -1;
}

void TabManager::updateTabTitle(CodeViewer *viewer)
{
    const int tabIndex = findTabByViewer(viewer);
    if (tabIndex == -1) {
        return;
    }

    const QString title = viewer->displayName() + (viewer->document()->isModified() ? "*" : "");
    tabWidget->setTabText(tabIndex, title);
}

bool TabManager::saveViewer(CodeViewer *viewer)
{
    if (!viewer || !viewer->editableContent()) {
        return false;
    }

    if (!viewer->document()->isModified()) {
        return true;
    }

    bool externalConflict = false;
    if (!editorService->saveFile(viewer->filePath(), viewer->toPlainText(), externalConflict)) {
        QMessageBox::warning(tabWidget, "Lỗi", "Không thể lưu file:\n" + viewer->filePath());
        return false;
    }

    viewer->document()->setModified(false);
    updateTabTitle(viewer);
    editorService->watchFilePath(viewer->filePath());
    return true;
}

bool TabManager::maybeSaveTab(int index)
{
    CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(index));
    if (!viewer || !viewer->editableContent() || !viewer->document()->isModified()) {
        return true;
    }

    QMessageBox::StandardButton reply = QMessageBox::warning(
        tabWidget,
        "Unsaved Changes",
        "File \"" + viewer->displayName() + "\" has unsaved changes.\nDo you want to save before closing?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (reply == QMessageBox::Save) {
        return saveViewer(viewer);
    }

    if (reply == QMessageBox::Cancel) {
        return false;
    }

    return true;
}

bool TabManager::reloadViewerFromDisk(CodeViewer *viewer)
{
    if (!viewer) {
        return false;
    }

    OpenFileResult result = editorService->prepareFile(viewer->filePath(), viewer->displayName(), false);
    
    if (result.status != OpenFileResult::Ok) {
        QMessageBox::warning(tabWidget, "Lỗi", "Không thể tải lại file từ đĩa:\n" + viewer->filePath());
        return false;
    }

    viewer->setPlainText(result.content);
    viewer->document()->setModified(false);
    updateTabTitle(viewer);
    editorService->watchFilePath(viewer->filePath());
    return true;
}

QList<int> TabManager::affectedFileTabIndexes(const QString &path) const
{
    QList<int> indexes;

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(i));
        if (viewer && PathUtils::isSameOrChildPath(viewer->filePath(), path)) {
            indexes.append(i);
        }
    }

    return indexes;
}

void TabManager::onTabContextMenuRequested(const QPoint &pos)
{
    const int tabIndex = tabWidget->tabBar()->tabAt(pos);
    if (tabIndex < 0) {
        return;
    }

    CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(tabIndex));
    if (!viewer) {
        return;
    }

    QMenu menu(tabWidget);
    QAction *lineNumberAction = menu.addAction("Show Line Numbers");
    lineNumberAction->setCheckable(true);
    lineNumberAction->setChecked(viewer->lineNumbersVisible());

    QAction *selectedAction = menu.exec(tabWidget->tabBar()->mapToGlobal(pos));
    if (selectedAction == lineNumberAction) {
        viewer->setLineNumbersVisible(lineNumberAction->isChecked());
    }
}

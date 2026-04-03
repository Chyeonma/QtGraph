#include "tabmanager.h"
#include "editor/view/codeviewer.h"
#include "ui/canvas/canvasview.h"
#include "ui/welcome/welcometab.h"
#include "core/filemanager.h"
#include "core/fonts/fontmanager.h"
#include "editor/syntax/highlighterfactory.h"
#include "settings/settingstab.h"
#include "settings/settingsmanager.h"
#include <algorithm>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QLocale>
#include <QPushButton>
#include <QTabBar>

const QString TabManager::CANVAS_MARKER = "##canvas##";
const QString TabManager::SETTINGS_MARKER = "##settings##";
const QString TabManager::WELCOME_MARKER = "##welcome##";

namespace {
Qt::CaseSensitivity pathCaseSensitivity()
{
#ifdef Q_OS_WIN
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

QString cleanPath(const QString &path)
{
    return QDir::cleanPath(path);
}

bool isSamePath(const QString &left, const QString &right)
{
    return QString::compare(cleanPath(left), cleanPath(right), pathCaseSensitivity()) == 0;
}

bool isSameOrChildPath(const QString &candidatePath, const QString &rootPath)
{
    const QString candidate = cleanPath(candidatePath);
    const QString root = cleanPath(rootPath);

    if (isSamePath(candidate, root)) {
        return true;
    }

    return candidate.startsWith(root + QDir::separator(), pathCaseSensitivity());
}

QString remapPathAfterRename(const QString &originalPath, const QString &oldRootPath, const QString &newRootPath)
{
    const QString original = cleanPath(originalPath);
    const QString oldRoot = cleanPath(oldRootPath);
    const QString newRoot = cleanPath(newRootPath);

    if (isSamePath(original, oldRoot)) {
        return newRoot;
    }

    return cleanPath(newRoot + original.mid(oldRoot.size()));
}

QString unsupportedPreviewMessage(const QString &filePath, const QString &fileName)
{
    const QString suffix = QFileInfo(filePath).suffix().toLower();
    return
        "This file preview is not supported.\n\n"
        "File: " + fileName + "\n"
        "Path: " + filePath + "\n"
        "Extension: ." + suffix + "\n\n"
        "The tab is still opened so you can keep track of the file inside the project, "
        "but its content cannot be rendered as plain text.";
}

QString largeTextFileMessage(const QString &filePath, const QString &fileName)
{
    const QFileInfo fileInfo(filePath);
    const QString fileSize = QLocale().formattedDataSize(fileInfo.size());
    const QString maxSize = QLocale().formattedDataSize(FileManager::maxEditableTextFileBytes());

    return
        "This text file is too large to open in the built-in editor.\n\n"
        "File: " + fileName + "\n"
        "Path: " + filePath + "\n"
        "Size: " + fileSize + "\n"
        "Editor limit: " + maxSize + "\n\n"
        "The tab stays open so you know the file exists, but the app skips loading its full content "
        "to keep the interface responsive.";
}

bool shouldOpenLargeTextFile(QWidget *parent, const QString &filePath, const QString &fileName, bool &useSafePreview)
{
    const QFileInfo fileInfo(filePath);
    const QString fileSize = QLocale().formattedDataSize(fileInfo.size());
    const QString maxSize = QLocale().formattedDataSize(FileManager::maxEditableTextFileBytes());

    QMessageBox messageBox(parent);
    messageBox.setIcon(QMessageBox::Warning);
    messageBox.setWindowTitle("Large Text File");
    messageBox.setText("This file is larger than the editor's recommended limit.");
    messageBox.setInformativeText(
        "File: " + fileName + "\n"
        "Size: " + fileSize + "\n"
        "Recommended limit: " + maxSize + "\n\n"
        "You can still open it, but the app may become slower while rendering or editing the content."
    );

    QPushButton *openAnywayButton = messageBox.addButton("Open Anyway", QMessageBox::AcceptRole);
    QPushButton *safePreviewButton = messageBox.addButton("Open Safe Preview", QMessageBox::ActionRole);
    messageBox.addButton(QMessageBox::Cancel);
    messageBox.setDefaultButton(safePreviewButton);
    messageBox.exec();

    if (messageBox.clickedButton() == openAnywayButton) {
        useSafePreview = false;
        return true;
    }

    if (messageBox.clickedButton() == safePreviewButton) {
        useSafePreview = true;
        return true;
    }

    return false;
}
}

TabManager::TabManager(QTabWidget *tabWidget, SettingsManager *settingsManager, QObject *parent)
    : QObject(parent)
    , tabWidget(tabWidget)
    , settingsManager(settingsManager)
    , fileWatcher(new QFileSystemWatcher(this))
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
    connect(settingsManager, &SettingsManager::settingsChanged, this, [this]() {
        applyEditorFontToOpenTabs();
    });
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &filePath) {
        handleWatchedFileChanged(filePath);
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

    QString content;
    const bool binaryContent = FileManager::isBinaryFile(filePath);
    const bool oversizedTextContent = !binaryContent && FileManager::isTextFileTooLarge(filePath);
    bool useSafePreview = oversizedTextContent;

    if (oversizedTextContent && !shouldOpenLargeTextFile(tabWidget, filePath, fileName, useSafePreview)) {
        return;
    }

    const bool supportedContent = !binaryContent && !useSafePreview;

    if (supportedContent && !FileManager::readFileContent(filePath, content)) {
        QMessageBox::warning(nullptr, "Lỗi", "Không thể mở file: " + fileName);
        return;
    }

    if (binaryContent) {
        content = unsupportedPreviewMessage(filePath, fileName);
    } else if (useSafePreview) {
        content = largeTextFileMessage(filePath, fileName);
    }

    CodeViewer *editor = createTextViewer(content, filePath, fileName, supportedContent);

    // Gắn syntax highlighter (nếu hỗ trợ ngôn ngữ)
    if (supportedContent) {
        HighlighterFactory::createForFile(filePath, editor->document());
    }

    int tabIndex = tabWidget->addTab(editor, fileName);
    tabWidget->setTabToolTip(tabIndex, filePath);
    tabWidget->setCurrentIndex(tabIndex);

    connect(editor->document(), &QTextDocument::modificationChanged, this, [this, editor]() {
        updateTabTitle(editor);
    });

    if (supportedContent) {
        watchFilePath(filePath);
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

    settingsTab = new SettingsTab(settingsManager);
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
        const QString remappedPath = remapPathAfterRename(oldViewerPath, oldPath, newPath);

        unwatchFilePath(oldViewerPath);
        internalChangePaths.remove(oldViewerPath);

        viewer->updateFileContext(remappedPath, QFileInfo(remappedPath).fileName());
        tabWidget->setTabToolTip(index, remappedPath);
        updateTabTitle(viewer);

        if (viewer->editableContent()) {
            watchFilePath(remappedPath);
        }
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
            unwatchFilePath(viewer->filePath());
            internalChangePaths.remove(viewer->filePath());
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
        unwatchFilePath(viewer->filePath());
        internalChangePaths.remove(viewer->filePath());
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
    const AppSettings &currentSettings = settingsManager->settings();
    const QFont editorFont = FontManager::editorFont(currentSettings.editorFontFamily, currentSettings.editorFontSize);

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

    const AppSettings &currentSettings = settingsManager->settings();
    viewer->setFont(FontManager::editorFont(currentSettings.editorFontFamily, currentSettings.editorFontSize));
    viewer->setStyleSheet(
        "QPlainTextEdit { background: #1e1e1e; color: #d4d4d4; "
        "border: none; padding: 8px; }"
    );
    viewer->setLineWrapMode(QPlainTextEdit::NoWrap);
    viewer->setLineNumbersVisible(settingsManager->settings().defaultShowLineNumbers);
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

    internalChangePaths.insert(viewer->filePath());
    if (!FileManager::writeFileContent(viewer->filePath(), viewer->toPlainText())) {
        internalChangePaths.remove(viewer->filePath());
        QMessageBox::warning(tabWidget, "Lỗi", "Không thể lưu file:\n" + viewer->filePath());
        return false;
    }

    viewer->document()->setModified(false);
    updateTabTitle(viewer);
    watchFilePath(viewer->filePath());
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

void TabManager::watchFilePath(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return;
    }

    if (!fileWatcher->files().contains(filePath) && QFile::exists(filePath)) {
        fileWatcher->addPath(filePath);
    }
}

void TabManager::unwatchFilePath(const QString &filePath)
{
    if (fileWatcher->files().contains(filePath)) {
        fileWatcher->removePath(filePath);
    }
}

void TabManager::handleWatchedFileChanged(const QString &filePath)
{
    watchFilePath(filePath);

    if (internalChangePaths.contains(filePath)) {
        internalChangePaths.remove(filePath);
        return;
    }

    const int tabIndex = findTabByPath(filePath);
    if (tabIndex == -1) {
        return;
    }

    CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(tabIndex));
    if (!viewer || !viewer->editableContent()) {
        return;
    }

    tabWidget->setCurrentIndex(tabIndex);

    QMessageBox messageBox(tabWidget);
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
        watchFilePath(filePath);
    }
}

bool TabManager::reloadViewerFromDisk(CodeViewer *viewer)
{
    if (!viewer) {
        return false;
    }

    QString content;
    if (!FileManager::readFileContent(viewer->filePath(), content)) {
        QMessageBox::warning(tabWidget, "Lỗi", "Không thể tải lại file từ đĩa:\n" + viewer->filePath());
        return false;
    }

    viewer->setPlainText(content);
    viewer->document()->setModified(false);
    updateTabTitle(viewer);
    watchFilePath(viewer->filePath());
    return true;
}

QList<int> TabManager::affectedFileTabIndexes(const QString &path) const
{
    QList<int> indexes;

    for (int i = 0; i < tabWidget->count(); ++i) {
        CodeViewer *viewer = qobject_cast<CodeViewer*>(tabWidget->widget(i));
        if (viewer && isSameOrChildPath(viewer->filePath(), path)) {
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

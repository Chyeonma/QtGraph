#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaces/ifontprovider.h"
#include "interfaces/ihighlighterfactory.h"
#include "app/appcontext.h"
#include "services/projectservice.h"
#include "services/fileservice.h"
#include "services/editorservice.h"
#include "core/utils/pathutils.h"
#include "ui/tabs/tabmanager.h"
#include "ui/tree/projecttreeview.h"
#include "ui/canvas/canvasview.h"
#include "ui/welcome/welcometab.h"
#include "services/settingsservice.h"
#include <QApplication>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

namespace {
QString actionVerb(bool moveOperation)
{
    return moveOperation ? "move" : "copy";
}
}

MainWindow::MainWindow(AppContext *context, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , appContext(context)
    , projectService(context->projectService())
    , fileService(context->fileService())
    , editorService(context->editorService())
    , fontProvider(context->fontProvider())
    , highlighterFactory(context->highlighterFactory())
    , settingsService(context->settingsService())
{
    ui->setupUi(this);

    // ==========================================
    // 1. KHỞI TẠO CÁC MODULE
    // ==========================================

    tabManager = new TabManager(ui->tabWidget, appContext, this);

    fileModel = new QFileSystemModel(this);
    ui->treeView->setModel(fileModel);

    // ==========================================
    // 2. CẤU HÌNH GIAO DIỆN LÚC MỚI MỞ
    // ==========================================

    ui->treeView->hide();
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->splitter->setSizes({0, 1000});

    // ==========================================
    // 3. KẾT NỐI TÍN HIỆU (SIGNAL → SLOT)
    // ==========================================

    // Menu actions
    connect(ui->actionOpen_Folder, &QAction::triggered, this, &MainWindow::onOpenFolderTriggered);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::onSaveTriggered);
    connect(ui->actionSave_All, &QAction::triggered, this, &MainWindow::onSaveAllTriggered);
    connect(ui->actionCanvas_View, &QAction::triggered, this, &MainWindow::onCanvasViewTriggered);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::onSettingsTriggered);
    connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::onUndoTriggered);
    connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::onRedoTriggered);
    connect(ui->actionCut, &QAction::triggered, this, &MainWindow::onCutTriggered);
    connect(ui->actionCopy, &QAction::triggered, this, &MainWindow::onCopyTriggered);
    connect(ui->actionPaste, &QAction::triggered, this, &MainWindow::onPasteTriggered);
    connect(ui->actionSelect_All, &QAction::triggered, this, &MainWindow::onSelectAllTriggered);
    connect(settingsService, &SettingsService::settingsChanged, this, &MainWindow::applyUiSettings);

    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave_All->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcuts({QKeySequence(Qt::CTRL | Qt::Key_Y), QKeySequence::Redo});
    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);
    ui->actionSelect_All->setShortcut(QKeySequence::SelectAll);

    // TreeView
    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::onTreeViewDoubleClicked);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
    connect(ui->treeView, &ProjectTreeView::pathsDropped, this, &MainWindow::onTreePathsDropped);
    
    // Services Connects
    connect(projectService, &ProjectService::projectOpened, this, [this](const QString &path) {
        QModelIndex index = fileModel->setRootPath(path);
        ui->treeView->setRootIndex(index);
        ui->treeView->setProjectRootPath(path);
        ui->treeView->show();
        ui->splitter->setSizes({250, 800});
    });
    
    connect(fileService, &FileService::clipboardChanged, this, [this]() {
        if (this->fileService->clipboard().mode == FileClipboard::Cut) {
            ui->treeView->setCutPath(this->fileService->clipboard().path);
        } else {
            ui->treeView->clearCutPath();
        }
    });

    applyUiSettings();

    WelcomeTab *welcome = tabManager->openWelcome();
    connect(welcome, &WelcomeTab::openFolderRequested, this, &MainWindow::onOpenFolderTriggered, Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (tabManager->confirmCloseAllTabs()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::onOpenFolderTriggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Chọn thư mục dự án", QDir::homePath());

    if (!folderPath.isEmpty()) {
        projectService->openProject(folderPath);
    }
}

void MainWindow::onSaveTriggered()
{
    if (tabManager->saveCurrentTab()) {
        ui->statusbar->showMessage("Saved current file.", 3000);
    } else {
        ui->statusbar->showMessage("Nothing was saved.", 3000);
    }
}

void MainWindow::onSaveAllTriggered()
{
    if (tabManager->saveAllTabs()) {
        ui->statusbar->showMessage("Saved all modified files.", 3000);
    } else {
        ui->statusbar->showMessage("Some files could not be saved.", 3000);
    }
}

void MainWindow::onCanvasViewTriggered()
{
    QString currentFolder = projectService->currentRootPath();
    if (currentFolder.isEmpty()) {
        QMessageBox::information(this, "Thông báo", "Vui lòng mở một dự án (Open Folder) trước khi bật Canvas.");
        return;
    }

    CanvasView *canvas = tabManager->openCanvas();
    canvas->drawNodes(currentFolder);
}

void MainWindow::onSettingsTriggered()
{
    tabManager->openSettings();
}

void MainWindow::onUndoTriggered()
{
    tabManager->undoCurrentTab();
}

void MainWindow::onRedoTriggered()
{
    tabManager->redoCurrentTab();
}

void MainWindow::onCutTriggered()
{
    if (ui->treeView->hasFocus() && ui->treeView->currentIndex().isValid()) {
        handleCutPath(ui->treeView->currentIndex());
        return;
    }

    tabManager->cutCurrentTab();
}

void MainWindow::onCopyTriggered()
{
    if (ui->treeView->hasFocus() && ui->treeView->currentIndex().isValid()) {
        handleCopyPath(ui->treeView->currentIndex());
        return;
    }

    tabManager->copyCurrentTab();
}

void MainWindow::onPasteTriggered()
{
    if (ui->treeView->hasFocus()) {
        handlePastePath(ui->treeView->currentIndex());
        return;
    }

    tabManager->pasteCurrentTab();
}

void MainWindow::onSelectAllTriggered()
{
    tabManager->selectAllInCurrentTab();
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    if (fileModel->isDir(index)) return;

    QString filePath = fileModel->filePath(index);
    QString fileName = fileModel->fileName(index);
    tabManager->openFile(filePath, fileName);
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    QMenu contextMenu(this);
    const bool canPaste = fileService->clipboard().hasContent();

    if (!index.isValid()) {
        QAction *actionNewFile = contextMenu.addAction("New File (Root)...");
        QAction *actionNewFolder = contextMenu.addAction("New Folder (Root)...");
        QAction *actionPaste = contextMenu.addAction("Paste");
        actionPaste->setEnabled(canPaste);

        QAction *selectedAction = contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));

        if (selectedAction == actionNewFile) handleNewFile(QModelIndex());
        else if (selectedAction == actionNewFolder) handleNewFolder(QModelIndex());
        else if (selectedAction == actionPaste) handlePastePath(QModelIndex());
    }
    else {
        QAction *actionCopy = contextMenu.addAction("Copy");
        QAction *actionCut = contextMenu.addAction("Cut");
        QAction *actionPaste = contextMenu.addAction("Paste");
        actionPaste->setEnabled(canPaste);
        contextMenu.addSeparator();
        QAction *actionNewFile = contextMenu.addAction("New File...");
        QAction *actionNewFolder = contextMenu.addAction("New Folder...");
        contextMenu.addSeparator();
        QAction *actionRename = contextMenu.addAction("Rename (F2)");
        QAction *actionDelete = contextMenu.addAction("Delete (Del)");

        QAction *selectedAction = contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));

        if (selectedAction == actionCopy) handleCopyPath(index);
        else if (selectedAction == actionCut) handleCutPath(index);
        else if (selectedAction == actionPaste) handlePastePath(index);
        else if (selectedAction == actionNewFile) handleNewFile(index);
        else if (selectedAction == actionNewFolder) handleNewFolder(index);
        else if (selectedAction == actionRename) handleRename(index);
        else if (selectedAction == actionDelete) handleDelete(index);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.isValid()) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_F2) {
        handleRename(currentIndex);
    }
    else if (event->key() == Qt::Key_Delete) {
        handleDelete(currentIndex);
    }
    else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::applyUiSettings()
{
    const AppSettings &current = settingsService->settings();
    qApp->setFont(fontProvider->uiFont(current.uiFontFamily, current.uiFontSize));
}

void MainWindow::onTreePathsDropped(const QStringList &sourcePaths, const QString &targetDirPath, Qt::DropAction dropAction)
{
    const bool moveOperation = dropAction == Qt::MoveAction;
    int successCount = 0;
    QStringList failedNames;

    for (const QString &sourcePath : sourcePaths) {
        QString resultPath;
        if (fileService->moveOrCopyPath(sourcePath, targetDirPath, moveOperation, &resultPath)) {
            ++successCount;
            if (moveOperation) {
                tabManager->handlePathRenamed(sourcePath, resultPath);
            }
        } else {
            failedNames.append(QFileInfo(sourcePath).fileName());
        }
    }

    if (successCount > 0) {
        ui->statusbar->showMessage(
            QString("%1 item(s) %2 successfully.").arg(successCount).arg(moveOperation ? "moved" : "copied"),
            3000
        );
    }

    if (!failedNames.isEmpty()) {
        QMessageBox::warning(
            this,
            "Operation Failed",
            "Could not " + actionVerb(moveOperation) + " these item(s): " + failedNames.join(", ")
        );
    }
}

void MainWindow::handleNewFile(const QModelIndex &index) {
    QString dirPath = index.isValid() ?
                      (fileModel->isDir(index) ? fileModel->filePath(index) : QFileInfo(fileModel->filePath(index)).absolutePath()) :
                      projectService->currentRootPath();
    if (dirPath.isEmpty()) return;

    bool ok;
    QString fileName = QInputDialog::getText(this, "New File", "Enter file name:", QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        if (!fileService->createNewFile(dirPath, fileName)) {
            QMessageBox::warning(this, "Lỗi", "Không thể tạo file. Hãy kiểm tra quyền truy cập!");
        }
    }
}

void MainWindow::handleNewFolder(const QModelIndex &index) {
    QString dirPath = index.isValid() ?
                      (fileModel->isDir(index) ? fileModel->filePath(index) : QFileInfo(fileModel->filePath(index)).absolutePath()) :
                      projectService->currentRootPath();
    if (dirPath.isEmpty()) return;

    bool ok;
    QString folderName = QInputDialog::getText(this, "New Folder", "Enter folder name:", QLineEdit::Normal, "", &ok);
    if (ok && !folderName.isEmpty()) {
        fileService->createNewFolder(dirPath, folderName);
    }
}

void MainWindow::handleRename(const QModelIndex &index) {
    QString path = fileModel->filePath(index);
    QString currentName = QFileInfo(path).fileName();

    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "Enter new name:", QLineEdit::Normal, currentName, &ok);
    if (ok && !newName.isEmpty() && newName != currentName) {
        const QString newPath = QDir(QFileInfo(path).absolutePath()).filePath(newName);
        if (!fileService->renamePath(path, newName)) {
            QMessageBox::warning(this, "Lỗi", "Không thể đổi tên. File có thể đang được sử dụng.");
            return;
        }

        tabManager->handlePathRenamed(path, newPath);
    }
}

void MainWindow::handleDelete(const QModelIndex &index) {
    QString path = fileModel->filePath(index);
    auto reply = QMessageBox::question(this, "Delete",
        "Bạn có chắc chắn muốn xóa '" + QFileInfo(path).fileName() + "'?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (!tabManager->confirmPathRemoval(path, this)) {
            return;
        }

        if (!fileService->deletePath(path)) {
            QMessageBox::warning(this, "Lỗi", "Không thể xóa mục đã chọn.");
            return;
        }

        tabManager->handlePathRemoved(path);
    }
}

void MainWindow::handleCopyPath(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QString path = fileModel->filePath(index);
    fileService->setClipboard(path, FileClipboard::Copy);
    ui->statusbar->showMessage("Copied: " + QFileInfo(path).fileName(), 3000);
}

void MainWindow::handleCutPath(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QString path = fileModel->filePath(index);
    fileService->setClipboard(path, FileClipboard::Cut);
    ui->statusbar->showMessage("Cut: " + QFileInfo(path).fileName(), 3000);
}

void MainWindow::handlePastePath(const QModelIndex &index)
{
    if (!fileService->clipboard().hasContent()) {
        ui->statusbar->showMessage("No copied or cut file or folder.", 3000);
        return;
    }

    if (!QFileInfo(fileService->clipboard().path).exists()) {
        fileService->clearClipboard();
        ui->statusbar->showMessage("The stored file or folder no longer exists.", 3000);
        return;
    }

    const bool moveOperation = fileService->clipboard().mode == FileClipboard::Cut;
    QString resultPath;
    QString sourcePath = fileService->clipboard().path;
    
    if (!fileService->pasteClipboard(targetDirectoryForIndex(index), &resultPath)) {
        QMessageBox::warning(this, "Paste Failed", "Could not paste the selected file or folder here.");
        return;
    }

    if (moveOperation) {
        tabManager->handlePathRenamed(sourcePath, resultPath);
        ui->statusbar->showMessage("Moved: " + QFileInfo(resultPath).fileName(), 3000);
        return;
    }

    ui->statusbar->showMessage("Pasted: " + QFileInfo(resultPath).fileName(), 3000);
}

QString MainWindow::targetDirectoryForIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return projectService->currentRootPath();
    }

    if (fileModel->isDir(index)) {
        return fileModel->filePath(index);
    }

    return QFileInfo(fileModel->filePath(index)).absolutePath();
}

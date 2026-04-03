#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/fonts/fontmanager.h"
#include "ui/tabs/tabmanager.h"
#include "ui/tree/projecttreeview.h"
#include "ui/canvas/canvasview.h"
#include "ui/welcome/welcometab.h"
#include "core/filemanager.h"
#include "settings/settingsmanager.h"
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
    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
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
}

MainWindow::MainWindow(SettingsManager *settingsManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settingsManager(settingsManager)
{
    ui->setupUi(this);

    // ==========================================
    // 1. KHỞI TẠO CÁC MODULE
    // ==========================================

    tabManager = new TabManager(ui->tabWidget, settingsManager, this);

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
    connect(settingsManager, &SettingsManager::settingsChanged, this, &MainWindow::applyUiSettings);

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

// ==========================================
// SLOTS: MENU & TOOLBAR
// ==========================================

void MainWindow::onOpenFolderTriggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Chọn thư mục dự án", QDir::homePath());

    if (!folderPath.isEmpty()) {
        QModelIndex index = fileModel->setRootPath(folderPath);
        ui->treeView->setRootIndex(index);
        ui->treeView->setProjectRootPath(folderPath);
        ui->treeView->show();
        ui->splitter->setSizes({250, 800});
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
    QString currentFolder = fileModel->rootPath();
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

// ==========================================
// SLOTS: CONTEXT MENU & PHÍM TẮT
// ==========================================

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    QMenu contextMenu(this);
    const bool canPaste = hasTreeClipboardPath();

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
    const AppSettings &current = settingsManager->settings();
    qApp->setFont(FontManager::uiFont(current.uiFontFamily, current.uiFontSize));
}

void MainWindow::onTreePathsDropped(const QStringList &sourcePaths, const QString &targetDirPath, Qt::DropAction dropAction)
{
    const bool moveOperation = dropAction == Qt::MoveAction;
    int successCount = 0;
    QStringList failedNames;

    for (const QString &sourcePath : sourcePaths) {
        QString resultPath;
        if (performTreeOperation(sourcePath, targetDirPath, moveOperation, &resultPath)) {
            ++successCount;
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

// ==========================================
// HELPER: GỌI FILEMANAGER + HIỂN THỊ KẾT QUẢ
// ==========================================

void MainWindow::handleNewFile(const QModelIndex &index) {
    QString dirPath = index.isValid() ?
                      (fileModel->isDir(index) ? fileModel->filePath(index) : QFileInfo(fileModel->filePath(index)).absolutePath()) :
                      fileModel->rootPath();
    if (dirPath.isEmpty()) return;

    bool ok;
    QString fileName = QInputDialog::getText(this, "New File", "Enter file name:", QLineEdit::Normal, "", &ok);
    if (ok && !fileName.isEmpty()) {
        if (!FileManager::createFile(dirPath, fileName)) {
            QMessageBox::warning(this, "Lỗi", "Không thể tạo file. Hãy kiểm tra quyền truy cập!");
        }
    }
}

void MainWindow::handleNewFolder(const QModelIndex &index) {
    QString dirPath = index.isValid() ?
                      (fileModel->isDir(index) ? fileModel->filePath(index) : QFileInfo(fileModel->filePath(index)).absolutePath()) :
                      fileModel->rootPath();
    if (dirPath.isEmpty()) return;

    bool ok;
    QString folderName = QInputDialog::getText(this, "New Folder", "Enter folder name:", QLineEdit::Normal, "", &ok);
    if (ok && !folderName.isEmpty()) {
        FileManager::createFolder(dirPath, folderName);
    }
}

void MainWindow::handleRename(const QModelIndex &index) {
    QString path = fileModel->filePath(index);
    QString currentName = QFileInfo(path).fileName();

    bool ok;
    QString newName = QInputDialog::getText(this, "Rename", "Enter new name:", QLineEdit::Normal, currentName, &ok);
    if (ok && !newName.isEmpty() && newName != currentName) {
        const QString newPath = QDir(QFileInfo(path).absolutePath()).filePath(newName);
        if (!FileManager::rename(path, newName)) {
            QMessageBox::warning(this, "Lỗi", "Không thể đổi tên. File có thể đang được sử dụng.");
            return;
        }

        syncTreeClipboardAfterRename(path, newPath);
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

        if (!FileManager::remove(path)) {
            QMessageBox::warning(this, "Lỗi", "Không thể xóa mục đã chọn.");
            return;
        }

        clearTreeClipboardIfAffected(path);
        tabManager->handlePathRemoved(path);
    }
}

void MainWindow::handleCopyPath(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    treeClipboardPath = fileModel->filePath(index);
    treeClipboardMode = TreeClipboardMode::Copy;
    ui->treeView->clearCutPath();
    ui->statusbar->showMessage("Copied: " + QFileInfo(treeClipboardPath).fileName(), 3000);
}

void MainWindow::handleCutPath(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    treeClipboardPath = fileModel->filePath(index);
    treeClipboardMode = TreeClipboardMode::Cut;
    ui->treeView->setCutPath(treeClipboardPath);
    ui->statusbar->showMessage("Cut: " + QFileInfo(treeClipboardPath).fileName(), 3000);
}

void MainWindow::handlePastePath(const QModelIndex &index)
{
    if (!hasTreeClipboardPath()) {
        ui->statusbar->showMessage("No copied or cut file or folder.", 3000);
        return;
    }

    if (!QFileInfo(treeClipboardPath).exists()) {
        clearTreeClipboard();
        ui->statusbar->showMessage("The stored file or folder no longer exists.", 3000);
        return;
    }

    const bool moveOperation = treeClipboardMode == TreeClipboardMode::Cut;
    QString resultPath;
    if (!performTreeOperation(treeClipboardPath, targetDirectoryForIndex(index), moveOperation, &resultPath)) {
        QMessageBox::warning(this, "Paste Failed", "Could not paste the selected file or folder here.");
        return;
    }

    if (moveOperation) {
        clearTreeClipboard();
        ui->statusbar->showMessage("Moved: " + QFileInfo(resultPath).fileName(), 3000);
        return;
    }

    ui->statusbar->showMessage("Pasted: " + QFileInfo(resultPath).fileName(), 3000);
}

QString MainWindow::targetDirectoryForIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return fileModel->rootPath();
    }

    if (fileModel->isDir(index)) {
        return fileModel->filePath(index);
    }

    return QFileInfo(fileModel->filePath(index)).absolutePath();
}

bool MainWindow::performTreeOperation(const QString &sourcePath, const QString &targetDirPath, bool moveOperation, QString *outResultPath)
{
    if (sourcePath.isEmpty() || targetDirPath.isEmpty()) {
        return false;
    }

    const QFileInfo sourceInfo(sourcePath);
    if (!sourceInfo.exists()) {
        return false;
    }

    if (moveOperation && QFileInfo(sourcePath).absolutePath() == QDir(targetDirPath).absolutePath()) {
        return true;
    }

    QString resultPath;
    const bool ok = moveOperation
        ? FileManager::movePath(sourcePath, targetDirPath, &resultPath)
        : FileManager::copyPath(sourcePath, targetDirPath, &resultPath);

    if (!ok) {
        return false;
    }

    if (moveOperation) {
        tabManager->handlePathRenamed(sourcePath, resultPath);
    }

    if (outResultPath) {
        *outResultPath = resultPath;
    }
    return true;
}

bool MainWindow::hasTreeClipboardPath() const
{
    return treeClipboardMode != TreeClipboardMode::None && !treeClipboardPath.isEmpty();
}

void MainWindow::clearTreeClipboard()
{
    treeClipboardPath.clear();
    treeClipboardMode = TreeClipboardMode::None;
    ui->treeView->clearCutPath();
}

void MainWindow::syncTreeClipboardAfterRename(const QString &oldPath, const QString &newPath)
{
    if (!hasTreeClipboardPath() || !isSameOrChildPath(treeClipboardPath, oldPath)) {
        return;
    }

    treeClipboardPath = remapPathAfterRename(treeClipboardPath, oldPath, newPath);
    if (treeClipboardMode == TreeClipboardMode::Cut) {
        ui->treeView->setCutPath(treeClipboardPath);
    }
}

void MainWindow::clearTreeClipboardIfAffected(const QString &path)
{
    if (hasTreeClipboardPath() && isSameOrChildPath(treeClipboardPath, path)) {
        clearTreeClipboard();
    }
}

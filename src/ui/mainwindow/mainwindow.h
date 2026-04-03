#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class TabManager;
class SettingsManager;
class ProjectTreeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SettingsManager *settingsManager, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onOpenFolderTriggered();
    void onSaveTriggered();
    void onSaveAllTriggered();
    void onCanvasViewTriggered();
    void onSettingsTriggered();
    void onUndoTriggered();
    void onRedoTriggered();
    void onCutTriggered();
    void onCopyTriggered();
    void onPasteTriggered();
    void onSelectAllTriggered();
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);
    void onTreePathsDropped(const QStringList &sourcePaths, const QString &targetDirPath, Qt::DropAction dropAction);
    void applyUiSettings();

private:
    enum class TreeClipboardMode {
        None,
        Copy,
        Cut
    };

    Ui::MainWindow *ui;
    QFileSystemModel *fileModel;
    TabManager *tabManager;
    SettingsManager *settingsManager;
    QString treeClipboardPath;
    TreeClipboardMode treeClipboardMode = TreeClipboardMode::None;

    // Các hàm UI cho context menu (dùng FileManager bên trong)
    void handleNewFile(const QModelIndex &index);
    void handleNewFolder(const QModelIndex &index);
    void handleRename(const QModelIndex &index);
    void handleDelete(const QModelIndex &index);
    void handleCopyPath(const QModelIndex &index);
    void handleCutPath(const QModelIndex &index);
    void handlePastePath(const QModelIndex &index);
    QString targetDirectoryForIndex(const QModelIndex &index) const;
    bool performTreeOperation(const QString &sourcePath, const QString &targetDirPath, bool moveOperation, QString *outResultPath = nullptr);
    bool hasTreeClipboardPath() const;
    void clearTreeClipboard();
    void syncTreeClipboardAfterRename(const QString &oldPath, const QString &newPath);
    void clearTreeClipboardIfAffected(const QString &path);
};

#endif // MAINWINDOW_H

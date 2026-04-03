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

class AppContext;
class TabManager;
class ProjectTreeView;
class ProjectService;
class FileService;
class EditorService;
class IFontProvider;
class IHighlighterFactory;
class SettingsService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AppContext *context, QWidget *parent = nullptr);
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
    Ui::MainWindow *ui;
    QFileSystemModel *fileModel;
    AppContext *appContext;
    ProjectService *projectService;
    FileService *fileService;
    EditorService *editorService;
    IFontProvider *fontProvider;
    IHighlighterFactory *highlighterFactory;
    SettingsService *settingsService;
    TabManager *tabManager;

    void handleNewFile(const QModelIndex &index);
    void handleNewFolder(const QModelIndex &index);
    void handleRename(const QModelIndex &index);
    void handleDelete(const QModelIndex &index);
    void handleCopyPath(const QModelIndex &index);
    void handleCutPath(const QModelIndex &index);
    void handlePastePath(const QModelIndex &index);
    QString targetDirectoryForIndex(const QModelIndex &index) const;
};

#endif // MAINWINDOW_H

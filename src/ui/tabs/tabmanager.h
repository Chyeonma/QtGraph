#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QList>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QSet>

class CanvasView;
class WelcomeTab;
class SettingsManager;
class SettingsTab;
class CodeViewer;
class QFileSystemWatcher;
class QWidget;

class TabManager : public QObject
{
    Q_OBJECT

public:
    explicit TabManager(QTabWidget *tabWidget, SettingsManager *settingsManager, QObject *parent = nullptr);

    // Mở file trong tab mới (hoặc focus tab đã mở)
    void openFile(const QString &filePath, const QString &fileName);

    bool saveCurrentTab();
    bool saveAllTabs();
    bool confirmCloseAllTabs();
    void undoCurrentTab();
    void redoCurrentTab();
    void cutCurrentTab();
    void copyCurrentTab();
    void pasteCurrentTab();
    void selectAllInCurrentTab();

    // Mở hoặc focus tab Canvas, trả về CanvasView đang dùng
    CanvasView* openCanvas();

    // Mở hoặc focus tab Setting
    SettingsTab* openSettings();

    // Hiển thị tab Welcome
    WelcomeTab* openWelcome();

    // Tìm tab theo đường dẫn file (trả về index, -1 nếu không tìm thấy)
    int findTabByPath(const QString &filePath);
    CodeViewer* currentViewer() const;
    void handlePathRenamed(const QString &oldPath, const QString &newPath);
    bool confirmPathRemoval(const QString &path, QWidget *dialogParent) const;
    void handlePathRemoved(const QString &path);

public slots:
    // Đóng tab theo index
    void closeTab(int index);
    void onTabContextMenuRequested(const QPoint &pos);

private:
    QTabWidget *tabWidget;
    SettingsManager *settingsManager;
    QFileSystemWatcher *fileWatcher;

    // Con trỏ đến tab Canvas (nullptr nếu chưa mở)
    CanvasView *canvasView;
    SettingsTab *settingsTab;
    WelcomeTab *welcomeTab;
    QSet<QString> internalChangePaths;

    static const QString CANVAS_MARKER;
    static const QString SETTINGS_MARKER;
    static const QString WELCOME_MARKER;

    void applyEditorFontToOpenTabs();
    CodeViewer* createTextViewer(const QString &content, const QString &filePath, const QString &fileName, bool supportedContent);
    int findTabByViewer(CodeViewer *viewer) const;
    void updateTabTitle(CodeViewer *viewer);
    bool saveViewer(CodeViewer *viewer);
    bool maybeSaveTab(int index);
    void watchFilePath(const QString &filePath);
    void unwatchFilePath(const QString &filePath);
    void handleWatchedFileChanged(const QString &filePath);
    bool reloadViewerFromDisk(CodeViewer *viewer);
    QList<int> affectedFileTabIndexes(const QString &path) const;
};

#endif // TABMANAGER_H

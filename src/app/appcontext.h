#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include <QObject>

class IFileIO;
class IConfigStore;
class IFontProvider;
class IHighlighterFactory;
class ProjectService;
class FileService;
class EditorService;
class SettingsService;
class PluginManager;

class AppContext : public QObject
{
    Q_OBJECT
public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext() override;

    // Core interfaces
    IFileIO* fileIO() const;
    IConfigStore* configStore() const;
    IFontProvider* fontProvider() const;
    IHighlighterFactory* highlighterFactory() const;

    // Services
    ProjectService* projectService() const;
    FileService* fileService() const;
    EditorService* editorService() const;
    SettingsService* settingsService() const;
    PluginManager* pluginManager() const;

private:
    IFileIO *m_fileIO;
    IConfigStore *m_configStore;
    IFontProvider *m_fontProvider;
    IHighlighterFactory *m_highlighterFactory;

    ProjectService *m_projectService;
    FileService *m_fileService;
    EditorService *m_editorService;
    SettingsService *m_settingsService;
    PluginManager *m_pluginManager;
};

#endif // APPCONTEXT_H

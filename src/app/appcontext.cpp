#include "appcontext.h"
#include "core/utils/fileio.h"
#include "core/fonts/fontprovider.h"
#include "editor/syntax/highlighterregistry.h"
#include "core/config/jsonconfigstore.h"
#include "services/projectservice.h"
#include "services/fileservice.h"
#include "services/editorservice.h"
#include "services/settingsservice.h"
#include "services/pluginmanager.h"
#include <QCoreApplication>

AppContext::AppContext(QObject *parent)
    : QObject(parent)
{
    // Initialize Core Interfaces
    m_fileIO = new FileIO();
    m_fontProvider = new FontProvider();
    
    m_fontProvider->loadApplicationFonts();

    m_highlighterFactory = new HighlighterRegistry();
    m_configStore = new JsonConfigStore();

    // Initialize Services
    m_settingsService = new SettingsService(m_configStore, this);
    m_projectService = new ProjectService(this);
    m_fileService = new FileService(m_fileIO, this);
    m_editorService = new EditorService(m_fileIO, this);
    
    m_pluginManager = new PluginManager(this, this);
    m_pluginManager->discoverPlugins(QCoreApplication::applicationDirPath() + "/plugins");
    m_pluginManager->loadAllPlugins();
}

AppContext::~AppContext()
{
    delete m_fileIO;
    delete m_configStore;
    delete m_fontProvider;
    delete m_highlighterFactory;
}

IFileIO* AppContext::fileIO() const { return m_fileIO; }
IConfigStore* AppContext::configStore() const { return m_configStore; }
IFontProvider* AppContext::fontProvider() const { return m_fontProvider; }
IHighlighterFactory* AppContext::highlighterFactory() const { return m_highlighterFactory; }
ProjectService* AppContext::projectService() const { return m_projectService; }
FileService* AppContext::fileService() const { return m_fileService; }
EditorService* AppContext::editorService() const { return m_editorService; }
SettingsService* AppContext::settingsService() const { return m_settingsService; }
PluginManager* AppContext::pluginManager() const { return m_pluginManager; }

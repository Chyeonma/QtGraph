#include "pluginmanager.h"
#include "interfaces/iplugin.h"
#include "app/appcontext.h"
#include <QDebug>

PluginManager::PluginManager(AppContext *context, QObject *parent)
    : QObject(parent)
    , m_context(context)
{
}

PluginManager::~PluginManager()
{
    unloadAllPlugins();
}

void PluginManager::discoverPlugins(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        return;
    }

    dir.setNameFilters(QStringList() << "*.so" << "*.dll" << "*.dylib");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (const QFileInfo &fileInfo : list) {
        if (!m_discoveredPaths.contains(fileInfo.absoluteFilePath())) {
            m_discoveredPaths.append(fileInfo.absoluteFilePath());
        }
    }
}

void PluginManager::loadAllPlugins()
{
    for (const QString &path : m_discoveredPaths) {
        QSharedPointer<QPluginLoader> loader(new QPluginLoader(path));
        
        QObject *pluginObject = loader->instance();
        if (!pluginObject) {
            emit pluginError(path, loader->errorString());
            continue;
        }

        IPlugin *plugin = qobject_cast<IPlugin*>(pluginObject);
        if (!plugin) {
            emit pluginError(path, "Not a valid IPlugin instance");
            continue;
        }

        if (plugin->initialize(m_context)) {
            PluginInfo info;
            info.name = plugin->name();
            info.version = plugin->version();
            info.description = plugin->description();
            info.author = plugin->author();
            info.filePath = path;

            PluginContainer container;
            container.loader = loader;
            container.instance = plugin;
            container.info = info;

            m_plugins.append(container);
            emit pluginLoaded(info);
        } else {
            emit pluginError(path, "Plugin initialization failed");
            loader->unload();
        }
    }
}

void PluginManager::unloadAllPlugins()
{
    for (PluginContainer &container : m_plugins) {
        if (container.instance) {
            container.instance->shutdown();
            emit pluginUnloaded(container.info.name);
        }
        if (container.loader->isLoaded()) {
            container.loader->unload();
        }
    }
    m_plugins.clear();
}

QVector<PluginInfo> PluginManager::loadedPlugins() const
{
    QVector<PluginInfo> result;
    for (const PluginContainer &container : m_plugins) {
        result.append(container.info);
    }
    return result;
}

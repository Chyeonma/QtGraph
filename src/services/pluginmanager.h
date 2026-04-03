#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "models/plugininfo.h"
#include <QObject>
#include <QVector>
#include <QPluginLoader>
#include <QDir>
#include <QSharedPointer>

class AppContext;
class IPlugin;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(AppContext *context, QObject *parent = nullptr);
    ~PluginManager() override;

    void discoverPlugins(const QString &dirPath);
    void loadAllPlugins();
    void unloadAllPlugins();

    QVector<PluginInfo> loadedPlugins() const;

signals:
    void pluginLoaded(const PluginInfo &info);
    void pluginUnloaded(const QString &name);
    void pluginError(const QString &path, const QString &errorString);

private:
    AppContext *m_context;
    
    struct PluginContainer {
        QSharedPointer<QPluginLoader> loader;
        IPlugin *instance;
        PluginInfo info;
    };
    
    QVector<PluginContainer> m_plugins;
    QStringList m_discoveredPaths;
};

#endif // PLUGINMANAGER_H

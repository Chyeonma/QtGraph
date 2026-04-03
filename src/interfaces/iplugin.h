#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <QString>
#include <QtPlugin>

class AppContext;

class IPlugin
{
public:
    virtual ~IPlugin() = default;

    // Metadata
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    virtual QString author() const = 0;

    // Lifecycle — gọi bởi PluginManager
    virtual bool initialize(AppContext *context) = 0;
    virtual void shutdown() = 0;
};

#define QTGRAPH_PLUGIN_IID "com.qtgraph.plugin/1.0"
Q_DECLARE_INTERFACE(IPlugin, QTGRAPH_PLUGIN_IID)

#endif // IPLUGIN_H

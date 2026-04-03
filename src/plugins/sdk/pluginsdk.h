#ifndef PLUGINSDK_H
#define PLUGINSDK_H

// Convenience header for Plugin Developers

#include "interfaces/iplugin.h"
#include "interfaces/ihighlighterfactory.h"
#include "interfaces/ifontprovider.h"
#include "interfaces/ifileio.h"
#include "app/appcontext.h"
#include "models/plugininfo.h"

// Macro definition helper for exporting the plugin in Qt6
#define QTGRAPH_EXPORT_PLUGIN(ClassName) \
    Q_PLUGIN_METADATA(IID QTGRAPH_PLUGIN_IID)

#endif // PLUGINSDK_H

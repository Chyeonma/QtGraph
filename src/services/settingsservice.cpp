#include "settingsservice.h"
#include "interfaces/iconfigstore.h"

SettingsService::SettingsService(IConfigStore *store, QObject *parent)
    : QObject(parent), store(store)
{
    if (store) {
        currentSettings = store->load();
    }
}

const AppSettings &SettingsService::settings() const
{
    return currentSettings;
}

QString SettingsService::settingsFilePath() const
{
    if (store) {
        return store->settingsFilePath();
    }
    return QString();
}

bool SettingsService::applySettings(const AppSettings &newSettings)
{
    if (store && !store->save(newSettings)) {
        return false;
    }

    currentSettings = newSettings;
    emit settingsChanged();
    return true;
}

bool SettingsService::resetToDefaults()
{
    if (store) {
        return applySettings(store->loadDefaults());
    }
    return false;
}

#ifndef SETTINGSSERVICE_H
#define SETTINGSSERVICE_H

#include "models/appsettings.h"
#include <QObject>

class IConfigStore;

class SettingsService : public QObject
{
    Q_OBJECT

public:
    explicit SettingsService(IConfigStore *store, QObject *parent = nullptr);

    const AppSettings &settings() const;
    QString settingsFilePath() const;

public slots:
    bool applySettings(const AppSettings &newSettings);
    bool resetToDefaults();

signals:
    void settingsChanged();

private:
    IConfigStore *store;
    AppSettings currentSettings;
};

#endif // SETTINGSSERVICE_H

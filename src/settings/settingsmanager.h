#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "appsettings.h"
#include <QObject>
#include <QString>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    const AppSettings &settings() const;
    QString settingsRootPath() const;
    QString settingsFilePath() const;
    QString defaultsFilePath() const;

public slots:
    bool applySettings(const AppSettings &newSettings);
    bool resetToDefaults();

signals:
    void settingsChanged();

private:
    void load();
    AppSettings readSettingsFile(const QString &filePath, const AppSettings &fallback) const;
    bool writeSettingsFile(const QString &filePath, const AppSettings &settings) const;

    AppSettings defaultSettings;
    AppSettings currentSettings;
};

#endif // SETTINGSMANAGER_H

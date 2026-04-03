#include "jsonconfigstore.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace {
AppSettings settingsFromJsonObject(const QJsonObject &object, const AppSettings &fallback)
{
    AppSettings settings = fallback;
    settings.uiFontFamily = object.value("uiFontFamily").toString(fallback.uiFontFamily);
    settings.uiFontSize = object.value("uiFontSize").toInt(fallback.uiFontSize);
    settings.editorFontFamily = object.value("editorFontFamily").toString(fallback.editorFontFamily);
    settings.editorFontSize = object.value("editorFontSize").toInt(fallback.editorFontSize);
    settings.defaultShowLineNumbers = object.value("defaultShowLineNumbers").toBool(fallback.defaultShowLineNumbers);
    return settings;
}

QJsonObject settingsToJsonObject(const AppSettings &settings)
{
    return {
        {"uiFontFamily", settings.uiFontFamily},
        {"uiFontSize", settings.uiFontSize},
        {"editorFontFamily", settings.editorFontFamily},
        {"editorFontSize", settings.editorFontSize},
        {"defaultShowLineNumbers", settings.defaultShowLineNumbers}
    };
}
}

AppSettings JsonConfigStore::load()
{
    AppSettings defaults = loadDefaults();
    AppSettings loaded = readSettingsFile(settingsFilePath(), defaults);

    QDir rootDir(settingsRootPath());
    rootDir.mkpath("data");

    QFile settingsFile(settingsFilePath());
    if (!settingsFile.exists()) {
        writeSettingsFile(settingsFilePath(), loaded);
    }

    return loaded;
}

bool JsonConfigStore::save(const AppSettings &settings)
{
    return writeSettingsFile(settingsFilePath(), settings);
}

AppSettings JsonConfigStore::loadDefaults()
{
    return readSettingsFile(defaultsFilePath(), AppSettings());
}

QString JsonConfigStore::settingsRootPath() const
{
    const QString appConfigRoot = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!appConfigRoot.isEmpty()) {
        return QDir(appConfigRoot).filePath("settings");
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath("settings");
}

QString JsonConfigStore::settingsFilePath() const
{
    return QDir(settingsRootPath()).filePath("data/app-settings.json");
}

QString JsonConfigStore::defaultsFilePath() const
{
    return QStringLiteral(":/settings/default-settings.json");
}

AppSettings JsonConfigStore::readSettingsFile(const QString &filePath, const AppSettings &fallback) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return fallback;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!document.isObject()) {
        return fallback;
    }

    return settingsFromJsonObject(document.object(), fallback);
}

bool JsonConfigStore::writeSettingsFile(const QString &filePath, const AppSettings &settings) const
{
    QFileInfo info(filePath);
    QDir().mkpath(info.absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    const QJsonDocument document(settingsToJsonObject(settings));
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

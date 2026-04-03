#ifndef JSONCONFIGSTORE_H
#define JSONCONFIGSTORE_H

#include "interfaces/iconfigstore.h"
#include "models/appsettings.h"

class JsonConfigStore : public IConfigStore
{
public:
    JsonConfigStore() = default;

    AppSettings load() override;
    bool save(const AppSettings &settings) override;
    AppSettings loadDefaults() override;
    QString settingsFilePath() const override;

private:
    QString settingsRootPath() const;
    QString defaultsFilePath() const;
    AppSettings readSettingsFile(const QString &filePath, const AppSettings &fallback) const;
    bool writeSettingsFile(const QString &filePath, const AppSettings &settings) const;
};

#endif // JSONCONFIGSTORE_H

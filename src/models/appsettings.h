#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>

struct AppSettings
{
    QString uiFontFamily = "Ubuntu Sans";
    int uiFontSize = 10;
    QString editorFontFamily = "JetBrains Mono";
    int editorFontSize = 11;
    bool defaultShowLineNumbers = false;

    bool operator==(const AppSettings &other) const
    {
        return uiFontFamily == other.uiFontFamily &&
               uiFontSize == other.uiFontSize &&
               editorFontFamily == other.editorFontFamily &&
               editorFontSize == other.editorFontSize &&
               defaultShowLineNumbers == other.defaultShowLineNumbers;
    }

    bool operator!=(const AppSettings &other) const
    {
        return !(*this == other);
    }
};

#endif // APPSETTINGS_H

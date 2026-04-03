#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QFont>
#include <QStringList>

class FontManager
{
public:
    static void loadApplicationFonts();
    static QStringList availableFontFamilies();
    static QFont editorFont(const QString &family = "JetBrains Mono", int pointSize = 11);
    static QFont uiFont(const QString &family = "Ubuntu Sans", int pointSize = 10);
};

#endif // FONTMANAGER_H

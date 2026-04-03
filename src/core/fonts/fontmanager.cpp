#include "fontmanager.h"
#include <QFontDatabase>
#include <QDebug>
#include <algorithm>

namespace {
QStringList &loadedFamilies()
{
    static QStringList families;
    return families;
}

void loadFontOrWarn(const QString &resourcePath)
{
    const int fontId = QFontDatabase::addApplicationFont(resourcePath);
    if (fontId == -1) {
        qWarning() << "Could not load font from resource:" << resourcePath;
        return;
    }

    const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    for (const QString &family : families) {
        if (!loadedFamilies().contains(family)) {
            loadedFamilies().append(family);
        }
    }
}
}

void FontManager::loadApplicationFonts()
{
    static bool loaded = false;
    if (loaded) {
        return;
    }

    loadFontOrWarn(":/fonts/source-sans-3/SourceSans3-Regular.ttf");
    loadFontOrWarn(":/fonts/source-sans-3/SourceSans3-Bold.ttf");
    loadFontOrWarn(":/fonts/source-code-pro/SourceCodePro-Regular.ttf");
    loadFontOrWarn(":/fonts/jetbrains-mono/JetBrainsMono-Regular.ttf");
    loadFontOrWarn(":/fonts/ubuntu-sans/UbuntuSans-Regular.otf");
    loadFontOrWarn(":/fonts/ubuntu-sans/UbuntuSans-Bold.otf");
    loadFontOrWarn(":/fonts/ibm-plex-sans/IBMPlexSans-Regular.ttf");
    loadFontOrWarn(":/fonts/ibm-plex-sans/IBMPlexSans-Bold.ttf");

    std::sort(loadedFamilies().begin(), loadedFamilies().end(), [](const QString &left, const QString &right) {
        return left.toLower() < right.toLower();
    });

    loaded = true;
}

QStringList FontManager::availableFontFamilies()
{
    loadApplicationFonts();
    return loadedFamilies();
}

QFont FontManager::editorFont(const QString &family, int pointSize)
{
    QFont font(family, pointSize);
    return font;
}

QFont FontManager::uiFont(const QString &family, int pointSize)
{
    return QFont(family, pointSize);
}

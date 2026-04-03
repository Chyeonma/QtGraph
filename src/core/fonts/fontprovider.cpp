#include "fontprovider.h"
#include <QFontDatabase>
#include <QDebug>
#include <algorithm>

namespace {
void loadFontOrWarn(const QString &resourcePath, QStringList &familiesOut)
{
    const int fontId = QFontDatabase::addApplicationFont(resourcePath);
    if (fontId == -1) {
        qWarning() << "Could not load font from resource:" << resourcePath;
        return;
    }

    const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    for (const QString &family : families) {
        if (!familiesOut.contains(family)) {
            familiesOut.append(family);
        }
    }
}
}

void FontProvider::loadApplicationFonts()
{
    if (m_fontsLoaded) {
        return;
    }

    loadFontOrWarn(":/fonts/source-sans-3/SourceSans3-Regular.ttf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/source-sans-3/SourceSans3-Bold.ttf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/source-code-pro/SourceCodePro-Regular.ttf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/jetbrains-mono/JetBrainsMono-Regular.ttf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/ubuntu-sans/UbuntuSans-Regular.otf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/ubuntu-sans/UbuntuSans-Bold.otf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/ibm-plex-sans/IBMPlexSans-Regular.ttf", m_loadedFamilies);
    loadFontOrWarn(":/fonts/ibm-plex-sans/IBMPlexSans-Bold.ttf", m_loadedFamilies);

    std::sort(m_loadedFamilies.begin(), m_loadedFamilies.end(), [](const QString &left, const QString &right) {
        return left.toLower() < right.toLower();
    });

    m_fontsLoaded = true;
}

QStringList FontProvider::availableFontFamilies() const
{
    if (!m_fontsLoaded) {
        const_cast<FontProvider*>(this)->loadApplicationFonts();
    }
    return m_loadedFamilies;
}

QFont FontProvider::editorFont(const QString &family, int pointSize) const
{
    return QFont(family, pointSize);
}

QFont FontProvider::uiFont(const QString &family, int pointSize) const
{
    return QFont(family, pointSize);
}

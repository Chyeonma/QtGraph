#ifndef FONTPROVIDER_H
#define FONTPROVIDER_H

#include "interfaces/ifontprovider.h"

class FontProvider : public IFontProvider
{
public:
    FontProvider() = default;

    void loadApplicationFonts() override;
    QStringList availableFontFamilies() const override;
    QFont uiFont(const QString &family, int pointSize) const override;
    QFont editorFont(const QString &family, int pointSize) const override;

private:
    QStringList m_loadedFamilies;
    bool m_fontsLoaded = false;
};

#endif // FONTPROVIDER_H

#ifndef IFONTPROVIDER_H
#define IFONTPROVIDER_H

#include <QFont>
#include <QString>
#include <QStringList>

class IFontProvider
{
public:
    virtual ~IFontProvider() = default;

    // Load fonts từ resources vào Qt font database
    virtual void loadApplicationFonts() = 0;

    // Danh sách font families đã load
    virtual QStringList availableFontFamilies() const = 0;

    // Tạo font cho UI (menu, tree, statusbar...)
    virtual QFont uiFont(const QString &family, int pointSize) const = 0;

    // Tạo font cho code editor
    virtual QFont editorFont(const QString &family, int pointSize) const = 0;
};

#endif // IFONTPROVIDER_H

#ifndef ICONFIGSTORE_H
#define ICONFIGSTORE_H

#include <QString>

struct AppSettings;

class IConfigStore
{
public:
    virtual ~IConfigStore() = default;

    // Đọc settings từ storage, trả về fallback nếu không có
    virtual AppSettings load() = 0;

    // Ghi settings xuống storage
    virtual bool save(const AppSettings &settings) = 0;

    // Đọc settings mặc định (embedded trong app)
    virtual AppSettings loadDefaults() = 0;

    // Đường dẫn file settings hiện tại (để hiển thị trên UI)
    virtual QString settingsFilePath() const = 0;
};

#endif // ICONFIGSTORE_H

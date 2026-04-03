#ifndef IFILEIO_H
#define IFILEIO_H

#include <QString>
#include <QStringList>
#include <QtGlobal>

class IFileIO
{
public:
    virtual ~IFileIO() = default;

    // Tạo file mới, trả về true nếu thành công
    virtual bool createFile(const QString &dirPath, const QString &fileName) = 0;

    // Tạo thư mục mới, trả về true nếu thành công
    virtual bool createFolder(const QString &dirPath, const QString &folderName) = 0;

    // Đổi tên file/thư mục, trả về true nếu thành công
    virtual bool rename(const QString &oldPath, const QString &newName) = 0;

    // Xóa file hoặc thư mục (đệ quy), trả về true nếu thành công
    virtual bool remove(const QString &path) = 0;

    // Đọc nội dung file text, trả về true nếu thành công
    virtual bool readFileContent(const QString &filePath, QString &outContent) = 0;

    // Ghi nội dung file text, trả về true nếu thành công
    virtual bool writeFileContent(const QString &filePath, const QString &content) = 0;

    // Sao chép file hoặc thư mục vào thư mục đích
    virtual bool copyPath(const QString &sourcePath, const QString &targetDirPath,
                          QString *outCopiedPath = nullptr) = 0;

    // Di chuyển file hoặc thư mục vào thư mục đích
    virtual bool movePath(const QString &sourcePath, const QString &targetDirPath,
                          QString *outMovedPath = nullptr) = 0;

    // Kiểm tra file có phải binary không
    virtual bool isBinaryFile(const QString &filePath) = 0;

    // Kiểm tra file text có vượt quá ngưỡng mở trong editor không
    virtual bool isTextFileTooLarge(const QString &filePath) = 0;

    // Ngưỡng kích thước tối đa để mở file text trong editor
    virtual qint64 maxEditableTextFileBytes() const = 0;

    // Lấy danh sách extensions binary
    virtual QStringList binaryExtensions() const = 0;
};

#endif // IFILEIO_H

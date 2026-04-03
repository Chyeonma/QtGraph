#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QtGlobal>

class FileManager
{
public:
    // Tạo file mới, trả về true nếu thành công
    static bool createFile(const QString &dirPath, const QString &fileName);

    // Tạo thư mục mới, trả về true nếu thành công
    static bool createFolder(const QString &dirPath, const QString &folderName);

    // Đổi tên file/thư mục, trả về true nếu thành công
    static bool rename(const QString &oldPath, const QString &newName);

    // Xóa file hoặc thư mục (đệ quy), trả về true nếu thành công
    static bool remove(const QString &path);

    // Đọc nội dung file text, trả về true nếu thành công
    static bool readFileContent(const QString &filePath, QString &outContent);

    // Ghi nội dung file text, trả về true nếu thành công
    static bool writeFileContent(const QString &filePath, const QString &content);

    // Sao chép file hoặc thư mục vào thư mục đích
    static bool copyPath(const QString &sourcePath, const QString &targetDirPath, QString *outCopiedPath = nullptr);

    // Di chuyển file hoặc thư mục vào thư mục đích
    static bool movePath(const QString &sourcePath, const QString &targetDirPath, QString *outMovedPath = nullptr);

    // Kiểm tra file có phải binary không (dựa trên extension)
    static bool isBinaryFile(const QString &filePath);

    // Kiểm tra file text có vượt quá ngưỡng mở trong editor không
    static bool isTextFileTooLarge(const QString &filePath);

    // Ngưỡng kích thước tối đa để mở file text trong editor
    static qint64 maxEditableTextFileBytes();

    // Lấy danh sách extensions binary
    static QStringList binaryExtensions();
};

#endif // FILEMANAGER_H

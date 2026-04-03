#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "models/fileclipboard.h"
#include <QObject>

class IFileIO;

class FileService : public QObject
{
    Q_OBJECT
public:
    explicit FileService(IFileIO *fileIO, QObject *parent = nullptr);

    const FileClipboard& clipboard() const;
    void setClipboard(const QString &path, FileClipboard::Mode mode);
    void clearClipboard();
    void syncClipboardAfterRename(const QString &oldPath, const QString &newPath);
    void clearClipboardIfAffected(const QString &path);

    bool createNewFile(const QString &dirPath, const QString &fileName);
    bool createNewFolder(const QString &dirPath, const QString &folderName);
    bool renamePath(const QString &oldPath, const QString &newPath);
    bool deletePath(const QString &path);
    bool pasteClipboard(const QString &targetDir, QString *outResultPath = nullptr);
    bool moveOrCopyPath(const QString &sourcePath, const QString &targetDirPath, bool moveOperation, QString *outResultPath = nullptr);

signals:
    void clipboardChanged();

private:
    IFileIO *m_fileIO;
    FileClipboard m_clipboard;
};

#endif // FILESERVICE_H

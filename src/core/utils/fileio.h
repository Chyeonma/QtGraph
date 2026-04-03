#ifndef FILEIO_H
#define FILEIO_H

#include "interfaces/ifileio.h"

class FileIO : public IFileIO
{
public:
    FileIO() = default;

    bool createFile(const QString &dirPath, const QString &fileName) override;
    bool createFolder(const QString &dirPath, const QString &folderName) override;
    bool rename(const QString &oldPath, const QString &newName) override;
    bool remove(const QString &path) override;

    bool readFileContent(const QString &filePath, QString &outContent) override;
    bool writeFileContent(const QString &filePath, const QString &content) override;

    bool copyPath(const QString &sourcePath, const QString &targetDirPath,
                  QString *outCopiedPath = nullptr) override;
    bool movePath(const QString &sourcePath, const QString &targetDirPath,
                  QString *outMovedPath = nullptr) override;

    bool isBinaryFile(const QString &filePath) override;
    bool isTextFileTooLarge(const QString &filePath) override;
    qint64 maxEditableTextFileBytes() const override;
    QStringList binaryExtensions() const override;
};

#endif // FILEIO_H

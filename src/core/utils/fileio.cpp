#include "fileio.h"
#include "core/utils/pathutils.h"
#include <QDir>
#include <QFile>

namespace {
constexpr qint64 kBinarySniffBytes = 8192;
constexpr qint64 kMaxEditableTextFileBytes = 1024 * 1024;

bool hasBinarySignature(const QByteArray &sample)
{
    if (sample.isEmpty()) {
        return false;
    }

    int suspiciousControlBytes = 0;

    for (unsigned char byte : sample) {
        if (byte == '\0') {
            return true;
        }

        const bool isAllowedControl =
            byte == '\n' || byte == '\r' || byte == '\t' || byte == '\f' || byte == '\b';
        if (byte < 0x20 && !isAllowedControl) {
            ++suspiciousControlBytes;
        }
    }

    return suspiciousControlBytes * 20 > sample.size();
}

QString copyCandidateName(const QFileInfo &sourceInfo, int attempt)
{
    const QString copySuffix = attempt == 1 ? " copy" : QString(" copy %1").arg(attempt);

    if (sourceInfo.isDir() || sourceInfo.completeSuffix().isEmpty()) {
        return sourceInfo.fileName() + copySuffix;
    }

    return sourceInfo.completeBaseName() + copySuffix + "." + sourceInfo.completeSuffix();
}

QString uniqueDestinationPath(const QFileInfo &sourceInfo, const QString &targetDirPath, bool preserveNameFirst)
{
    QDir targetDir(targetDirPath);
    QString candidatePath = targetDir.filePath(sourceInfo.fileName());

    if (preserveNameFirst && !QFileInfo::exists(candidatePath)) {
        return candidatePath;
    }

    for (int attempt = 1; ; ++attempt) {
        candidatePath = targetDir.filePath(copyCandidateName(sourceInfo, attempt));
        if (!QFileInfo::exists(candidatePath)) {
            return candidatePath;
        }
    }
}

bool copyDirectoryRecursively(const QString &sourceDirPath, const QString &targetDirPath)
{
    QDir sourceDir(sourceDirPath);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir targetDir(targetDirPath);
    if (!targetDir.exists() && !QDir().mkpath(targetDirPath)) {
        return false;
    }

    const QFileInfoList entries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo &entry : entries) {
        const QString sourceEntryPath = entry.absoluteFilePath();
        const QString targetEntryPath = targetDir.filePath(entry.fileName());

        if (entry.isDir()) {
            if (!copyDirectoryRecursively(sourceEntryPath, targetEntryPath)) {
                return false;
            }
        } else {
            if (!QFile::copy(sourceEntryPath, targetEntryPath)) {
                return false;
            }
        }
    }

    return true;
}
}

bool FileIO::createFile(const QString &dirPath, const QString &fileName)
{
    QFile file(QDir(dirPath).filePath(fileName));
    if (file.open(QIODevice::WriteOnly)) {
        file.close();
        return true;
    }
    return false;
}

bool FileIO::createFolder(const QString &dirPath, const QString &folderName)
{
    return QDir(dirPath).mkdir(folderName);
}

bool FileIO::rename(const QString &oldPath, const QString &newName)
{
    QString newPath = QDir(QFileInfo(oldPath).absolutePath()).filePath(newName);
    return QFile::rename(oldPath, newPath);
}

bool FileIO::remove(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir()) {
        return QDir(path).removeRecursively();
    } else {
        return QFile::remove(path);
    }
}

bool FileIO::readFileContent(const QString &filePath, QString &outContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    outContent = file.readAll();
    file.close();
    return true;
}

bool FileIO::writeFileContent(const QString &filePath, const QString &content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    file.write(content.toUtf8());
    file.close();
    return true;
}

bool FileIO::copyPath(const QString &sourcePath, const QString &targetDirPath, QString *outCopiedPath)
{
    const QFileInfo sourceInfo(sourcePath);
    const QFileInfo targetDirInfo(targetDirPath);

    if (!sourceInfo.exists() || !targetDirInfo.exists() || !targetDirInfo.isDir()) {
        return false;
    }

    if (sourceInfo.isDir() && PathUtils::isSameOrChildPath(targetDirPath, sourcePath)) {
        return false;
    }

    const QString destinationPath = uniqueDestinationPath(sourceInfo, targetDirPath, false);
    bool copied = false;

    if (sourceInfo.isDir()) {
        copied = copyDirectoryRecursively(sourceInfo.absoluteFilePath(), destinationPath);
    } else {
        copied = QFile::copy(sourceInfo.absoluteFilePath(), destinationPath);
    }

    if (!copied) {
        return false;
    }

    if (outCopiedPath) {
        *outCopiedPath = destinationPath;
    }
    return true;
}

bool FileIO::movePath(const QString &sourcePath, const QString &targetDirPath, QString *outMovedPath)
{
    const QFileInfo sourceInfo(sourcePath);
    const QFileInfo targetDirInfo(targetDirPath);

    if (!sourceInfo.exists() || !targetDirInfo.exists() || !targetDirInfo.isDir()) {
        return false;
    }

    if (sourceInfo.isDir() && PathUtils::isSameOrChildPath(targetDirPath, sourcePath)) {
        return false;
    }

    const QString sourceParentPath = sourceInfo.absolutePath();
    if (PathUtils::isSamePath(sourceParentPath, targetDirPath)) {
        return false;
    }

    QString destinationPath = uniqueDestinationPath(sourceInfo, targetDirPath, true);
    bool moved = false;

    if (sourceInfo.isDir()) {
        moved = QDir().rename(sourceInfo.absoluteFilePath(), destinationPath);
    } else {
        moved = QFile::rename(sourceInfo.absoluteFilePath(), destinationPath);
    }

    if (!moved) {
        if (!copyPath(sourceInfo.absoluteFilePath(), targetDirPath, &destinationPath)) {
            return false;
        }

        if (!remove(sourceInfo.absoluteFilePath())) {
            remove(destinationPath);
            return false;
        }
    }

    if (outMovedPath) {
        *outMovedPath = destinationPath;
    }
    return true;
}

bool FileIO::isBinaryFile(const QString &filePath)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    if (binaryExtensions().contains(suffix)) {
        return true;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QByteArray sample = file.read(kBinarySniffBytes);
    file.close();
    return hasBinarySignature(sample);
}

bool FileIO::isTextFileTooLarge(const QString &filePath)
{
    return QFileInfo(filePath).size() > maxEditableTextFileBytes();
}

qint64 FileIO::maxEditableTextFileBytes() const
{
    return kMaxEditableTextFileBytes;
}

QStringList FileIO::binaryExtensions() const
{
    return {
        // Ảnh
        "png", "jpg", "jpeg", "gif", "bmp", "ico", "svg", "webp", "tiff", "tif",
        // Video
        "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm",
        // Âm thanh
        "mp3", "wav", "flac", "aac", "ogg", "wma",
        // Nén
        "zip", "rar", "7z", "tar", "gz", "bz2", "xz",
        // Thực thi / Binary
        "exe", "dll", "so", "bin", "o", "a", "out", "class", "pyc",
        // Tài liệu đặc biệt
        "pdf", "doc", "docx", "xls", "xlsx", "ppt", "pptx", "odt",
        // Font
        "ttf", "otf", "woff", "woff2",
        // Khác
        "iso", "img", "db", "sqlite"
    };
}

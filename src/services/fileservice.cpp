#include "fileservice.h"
#include "interfaces/ifileio.h"
#include "core/utils/pathutils.h"
#include <QFileInfo>

FileService::FileService(IFileIO *fileIO, QObject *parent)
    : QObject(parent), m_fileIO(fileIO)
{
}

const FileClipboard& FileService::clipboard() const
{
    return m_clipboard;
}

void FileService::setClipboard(const QString &path, FileClipboard::Mode mode)
{
    m_clipboard.path = path;
    m_clipboard.mode = mode;
    emit clipboardChanged();
}

void FileService::clearClipboard()
{
    if (m_clipboard.hasContent()) {
        m_clipboard.clear();
        emit clipboardChanged();
    }
}

void FileService::syncClipboardAfterRename(const QString &oldPath, const QString &newPath)
{
    if (!m_clipboard.hasContent()) return;

    QString newClipboardPath = PathUtils::remapPathAfterRename(m_clipboard.path, oldPath, newPath);
    if (newClipboardPath != m_clipboard.path) {
        m_clipboard.path = newClipboardPath;
        emit clipboardChanged();
    }
}

void FileService::clearClipboardIfAffected(const QString &path)
{
    if (!m_clipboard.hasContent()) return;

    if (PathUtils::isSameOrChildPath(m_clipboard.path, path)) {
        clearClipboard();
    }
}

bool FileService::createNewFile(const QString &dirPath, const QString &fileName)
{
    if (!m_fileIO) return false;
    return m_fileIO->createFile(dirPath, fileName);
}

bool FileService::createNewFolder(const QString &dirPath, const QString &folderName)
{
    if (!m_fileIO) return false;
    return m_fileIO->createFolder(dirPath, folderName);
}

bool FileService::renamePath(const QString &oldPath, const QString &newPath)
{
    if (!m_fileIO) return false;
    bool ok = m_fileIO->rename(oldPath, newPath);
    if (ok) {
        syncClipboardAfterRename(oldPath, newPath);
    }
    return ok;
}

bool FileService::deletePath(const QString &path)
{
    if (!m_fileIO) return false;
    bool ok = m_fileIO->remove(path);
    if (ok) {
        clearClipboardIfAffected(path);
    }
    return ok;
}

bool FileService::pasteClipboard(const QString &targetDir, QString *outResultPath)
{
    if (!m_clipboard.hasContent() || !m_fileIO) return false;

    bool moveOperation = (m_clipboard.mode == FileClipboard::Cut);
    bool ok = moveOrCopyPath(m_clipboard.path, targetDir, moveOperation, outResultPath);

    if (ok && moveOperation) {
        clearClipboard();
    }
    return ok;
}

bool FileService::moveOrCopyPath(const QString &sourcePath, const QString &targetDirPath, bool moveOperation, QString *outResultPath)
{
    if (!m_fileIO) return false;

    if (PathUtils::isSameOrChildPath(targetDirPath, sourcePath)) {
        return false;
    }

    if (moveOperation) {
        return m_fileIO->movePath(sourcePath, targetDirPath, outResultPath);
    } else {
        return m_fileIO->copyPath(sourcePath, targetDirPath, outResultPath);
    }
}

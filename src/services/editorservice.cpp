#include "editorservice.h"
#include "interfaces/ifileio.h"
#include "core/utils/pathutils.h"
#include <QFileSystemWatcher>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

EditorService::EditorService(IFileIO *fileIO, QObject *parent)
    : QObject(parent), m_fileIO(fileIO)
{
    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &EditorService::handleWatchedFileChanged);
}

OpenFileResult EditorService::prepareFile(const QString &filePath, const QString &fileName, bool askForLargeFiles)
{
    OpenFileResult result;
    result.status = OpenFileResult::Ok;
    result.editable = true;

    if (!m_fileIO) {
        result.status = OpenFileResult::ReadError;
        return result;
    }

    const bool binaryContent = m_fileIO->isBinaryFile(filePath);
    const bool oversizedTextContent = !binaryContent && m_fileIO->isTextFileTooLarge(filePath);
    bool useSafePreview = oversizedTextContent;

    if (oversizedTextContent && askForLargeFiles) {
        QString msg = QString("File văn bản '%1' có kích thước lớn (> %2 MB).\n\n"
                              "Kích hoạt Syntax Highlighting có thể làm đơ ứng dụng. Bạn có muốn mở ở chế độ Safe Preview (không Highlighting, không edit được) không?")
                          .arg(fileName)
                          .arg(m_fileIO->maxEditableTextFileBytes() / (1024.0 * 1024.0), 0, 'f', 1);

        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr, "File lớn", msg,
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
            QMessageBox::Yes);

        if (reply == QMessageBox::Cancel) {
            result.status = OpenFileResult::ReadError;
            return result;
        }

        useSafePreview = (reply == QMessageBox::Yes);
    }

    result.editable = !binaryContent && !useSafePreview;

    if (result.editable && !m_fileIO->readFileContent(filePath, result.content)) {
        result.status = OpenFileResult::ReadError;
        return result;
    }

    if (binaryContent) {
        result.status = OpenFileResult::Binary;
        result.previewMessage = unsupportedPreviewMessage(filePath, fileName);
    } else if (useSafePreview) {
        result.status = OpenFileResult::TooLarge;
        result.previewMessage = largeTextFileMessage(filePath, fileName);
    }

    return result;
}

bool EditorService::saveFile(const QString &filePath, const QString &content, bool &outIsModifiedExternal)
{
    outIsModifiedExternal = false;
    if (!m_fileIO) return false;

    // Track state to avoid feedback loop from FileWatcher
    if (m_fileWatcher->files().contains(filePath)) {
        m_internalChangePaths.insert(filePath);
    }

    bool success = m_fileIO->writeFileContent(filePath, content);
    if (!success) {
        m_internalChangePaths.remove(filePath);
    }

    return success;
}

void EditorService::watchFilePath(const QString &filePath)
{
    if (QFile::exists(filePath)) {
        m_fileWatcher->addPath(filePath);
    }
}

void EditorService::unwatchFilePath(const QString &filePath)
{
    m_fileWatcher->removePath(filePath);
    m_internalChangePaths.remove(filePath);
}

void EditorService::renameWatchedPath(const QString &oldPath, const QString &newPath)
{
    if (m_fileWatcher->files().contains(oldPath)) {
        unwatchFilePath(oldPath);
        watchFilePath(newPath);
    }
}

void EditorService::handleWatchedFileChanged(const QString &filePath)
{
    // Nếu file thay đổi do nội dung mình vừa save → bỏ qua và xóa cờ
    if (m_internalChangePaths.contains(filePath)) {
        m_internalChangePaths.remove(filePath);
        return;
    }

    // Nếu không, đây là file thay đổi do ứng dụng bên ngoài
    emit externalFileChanged(filePath);
}

QString EditorService::unsupportedPreviewMessage(const QString &filePath, const QString &fileName) const
{
    QFileInfo info(filePath);
    qint64 sizeKb = info.size() / 1024;
    return QString("\n\n\n\n\n"
                   "                          File: %1\n"
                   "                          Kích thước: %2 KB\n\n"
                   "            Đây là file binary không được hỗ trợ hiển thị nội dung trực tiếp.\n"
                   "            Vui lòng mở file này bằng phần mềm chuyên dụng khác trên hệ thống.")
        .arg(fileName)
        .arg(sizeKb);
}

QString EditorService::largeTextFileMessage(const QString &filePath, const QString &fileName) const
{
    QFileInfo info(filePath);
    double sizeMb = info.size() / (1024.0 * 1024.0);
    return QString("\n\n\n\n\n"
                   "                          File: %1\n"
                   "                          Kích thước: %2 MB\n\n"
                   "            File văn bản quá lớn để có thể edit an toàn.\n"
                   "            File này đang được mở trong chế độ Đọc Tạm (Safe Preview).\n"
                   "            Bạn không thể chỉnh sửa đoạn text này.")
        .arg(fileName)
        .arg(sizeMb, 0, 'f', 2);
}

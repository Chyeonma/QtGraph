#ifndef EDITORSERVICE_H
#define EDITORSERVICE_H

#include "models/openfileresult.h"
#include <QObject>
#include <QSet>
#include <QString>

class IFileIO;
class QFileSystemWatcher;

class EditorService : public QObject
{
    Q_OBJECT
public:
    explicit EditorService(IFileIO *fileIO, QObject *parent = nullptr);

    OpenFileResult prepareFile(const QString &filePath, const QString &fileName, bool askForLargeFiles = true);
    bool saveFile(const QString &filePath, const QString &content, bool &outIsModifiedExternal);

    void watchFilePath(const QString &filePath);
    void unwatchFilePath(const QString &filePath);
    void renameWatchedPath(const QString &oldPath, const QString &newPath);

signals:
    void externalFileChanged(const QString &filePath);

private slots:
    void handleWatchedFileChanged(const QString &filePath);

private:
    IFileIO *m_fileIO;
    QFileSystemWatcher *m_fileWatcher;
    QSet<QString> m_internalChangePaths;

    QString unsupportedPreviewMessage(const QString &filePath, const QString &fileName) const;
    QString largeTextFileMessage(const QString &filePath, const QString &fileName) const;
};

#endif // EDITORSERVICE_H

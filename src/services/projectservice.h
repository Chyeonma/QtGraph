#ifndef PROJECTSERVICE_H
#define PROJECTSERVICE_H

#include <QObject>
#include <QString>

class ProjectService : public QObject
{
    Q_OBJECT
public:
    explicit ProjectService(QObject *parent = nullptr);

    QString currentRootPath() const;
    bool isProjectOpen() const;

public slots:
    void openProject(const QString &path);
    void closeProject();

signals:
    void projectOpened(const QString &path);
    void projectClosed();

private:
    QString m_rootPath;
};

#endif // PROJECTSERVICE_H

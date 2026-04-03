#include "projectservice.h"

ProjectService::ProjectService(QObject *parent)
    : QObject(parent)
{
}

QString ProjectService::currentRootPath() const
{
    return m_rootPath;
}

bool ProjectService::isProjectOpen() const
{
    return !m_rootPath.isEmpty();
}

void ProjectService::openProject(const QString &path)
{
    if (m_rootPath == path) return;
    
    m_rootPath = path;
    emit projectOpened(path);
}

void ProjectService::closeProject()
{
    if (m_rootPath.isEmpty()) return;

    m_rootPath.clear();
    emit projectClosed();
}

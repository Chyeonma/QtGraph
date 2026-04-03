#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <QString>
#include <QDir>
#include <QtGlobal>

namespace PathUtils {

Qt::CaseSensitivity caseSensitivity();

QString cleanPath(const QString &path);

bool isSamePath(const QString &left, const QString &right);

bool isSameOrChildPath(const QString &candidatePath, const QString &rootPath);

QString remapPathAfterRename(const QString &originalPath,
                             const QString &oldRootPath,
                             const QString &newRootPath);

} // namespace PathUtils

#endif // PATHUTILS_H

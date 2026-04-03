#include "pathutils.h"
#include <QFileInfo>

namespace PathUtils {

Qt::CaseSensitivity caseSensitivity()
{
#ifdef Q_OS_WIN
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

QString cleanPath(const QString &path)
{
    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

bool isSamePath(const QString &left, const QString &right)
{
    return QString::compare(cleanPath(left), cleanPath(right), caseSensitivity()) == 0;
}

bool isSameOrChildPath(const QString &candidatePath, const QString &rootPath)
{
    const QString candidate = cleanPath(candidatePath);
    const QString root = cleanPath(rootPath);

    if (isSamePath(candidate, root)) {
        return true;
    }

    return candidate.startsWith(root + QDir::separator(), caseSensitivity());
}

QString remapPathAfterRename(const QString &originalPath,
                             const QString &oldRootPath,
                             const QString &newRootPath)
{
    const QString original = cleanPath(originalPath);
    const QString oldRoot = cleanPath(oldRootPath);
    const QString newRoot = cleanPath(newRootPath);

    if (isSamePath(original, oldRoot)) {
        return newRoot;
    }

    return cleanPath(newRoot + original.mid(oldRoot.size()));
}

} // namespace PathUtils

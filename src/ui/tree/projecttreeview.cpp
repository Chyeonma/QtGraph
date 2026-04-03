#include "projecttreeview.h"
#include "core/utils/pathutils.h"
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDir>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QUrl>

namespace {
class ProjectTreeItemDelegate : public QStyledItemDelegate
{
public:
    explicit ProjectTreeItemDelegate(ProjectTreeView *treeView)
        : QStyledItemDelegate(treeView)
        , treeView(treeView)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QStyleOptionViewItem adjustedOption(option);
        initStyleOption(&adjustedOption, index);

        const QFileSystemModel *fileSystemModel = qobject_cast<const QFileSystemModel*>(index.model());
        const QString itemPath = fileSystemModel ? fileSystemModel->filePath(index) : QString();

        if (treeView->isPathAffectedByCut(itemPath)) {
            adjustedOption.font.setItalic(true);
            adjustedOption.palette.setColor(QPalette::Text, QColor("#8a8a8a"));
            adjustedOption.palette.setColor(QPalette::HighlightedText, QColor("#d0d0d0"));

            painter->save();
            painter->setOpacity(0.55);
            QStyledItemDelegate::paint(painter, adjustedOption, index);
            painter->restore();
            return;
        }

        QStyledItemDelegate::paint(painter, adjustedOption, index);
    }

private:
    ProjectTreeView *treeView;
};
}

ProjectTreeView::ProjectTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropOverwriteMode(false);
    setItemDelegate(new ProjectTreeItemDelegate(this));
}

void ProjectTreeView::setProjectRootPath(const QString &path)
{
    projectRootPath = path;
}

void ProjectTreeView::setCutPath(const QString &path)
{
    const QString normalized = PathUtils::cleanPath(path);
    if (cutPath == normalized) {
        return;
    }

    cutPath = normalized;
    viewport()->update();
}

void ProjectTreeView::clearCutPath()
{
    if (cutPath.isEmpty()) {
        return;
    }

    cutPath.clear();
    viewport()->update();
}

void ProjectTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (canAcceptDrop(event->mimeData(), event->position().toPoint())) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void ProjectTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    if (canAcceptDrop(event->mimeData(), event->position().toPoint())) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void ProjectTreeView::dropEvent(QDropEvent *event)
{
    const QString targetDirPath = dropTargetDirectory(event->position().toPoint());
    const QStringList sourcePaths = localPathsFromMimeData(event->mimeData());

    if (targetDirPath.isEmpty() || sourcePaths.isEmpty()) {
        event->ignore();
        return;
    }

    emit pathsDropped(sourcePaths, targetDirPath, event->proposedAction());
    event->acceptProposedAction();
}

QString ProjectTreeView::dropTargetDirectory(const QPoint &position) const
{
    if (projectRootPath.isEmpty()) {
        return QString();
    }

    const QFileSystemModel *fileSystemModel = qobject_cast<const QFileSystemModel*>(model());
    if (!fileSystemModel) {
        return QString();
    }

    const QModelIndex dropIndex = indexAt(position);
    if (!dropIndex.isValid()) {
        return projectRootPath;
    }

    const QString dropPath = fileSystemModel->filePath(dropIndex);
    if (fileSystemModel->isDir(dropIndex)) {
        return dropPath;
    }

    return QFileInfo(dropPath).absolutePath();
}

QStringList ProjectTreeView::localPathsFromMimeData(const QMimeData *mimeData) const
{
    QStringList paths;
    if (!mimeData || !mimeData->hasUrls()) {
        return paths;
    }

    for (const QUrl &url : mimeData->urls()) {
        if (url.isLocalFile()) {
            paths.append(url.toLocalFile());
        }
    }

    return paths;
}

bool ProjectTreeView::canAcceptDrop(const QMimeData *mimeData, const QPoint &position) const
{
    return !dropTargetDirectory(position).isEmpty() && !localPathsFromMimeData(mimeData).isEmpty();
}

bool ProjectTreeView::isPathAffectedByCut(const QString &path) const
{
    return !cutPath.isEmpty() && !path.isEmpty() && PathUtils::isSameOrChildPath(path, cutPath);
}

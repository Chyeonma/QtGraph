#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QTreeView>

class QMimeData;

class ProjectTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit ProjectTreeView(QWidget *parent = nullptr);

    void setProjectRootPath(const QString &path);
    void setCutPath(const QString &path);
    void clearCutPath();
    bool isPathAffectedByCut(const QString &path) const;

signals:
    void pathsDropped(const QStringList &sourcePaths, const QString &targetDirPath, Qt::DropAction dropAction);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString projectRootPath;
    QString cutPath;

    QString dropTargetDirectory(const QPoint &position) const;
    QStringList localPathsFromMimeData(const QMimeData *mimeData) const;
    bool canAcceptDrop(const QMimeData *mimeData, const QPoint &position) const;
};

#endif // PROJECTTREEVIEW_H

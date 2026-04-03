#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class CanvasView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasView(QWidget *parent = nullptr);

    // Quét thư mục và vẽ các Node lên canvas
    void drawNodes(const QString &folderPath);

private:
    QGraphicsScene *scene;
};

#endif // CANVASVIEW_H

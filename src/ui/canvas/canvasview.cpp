#include "canvasview.h"
#include <QDir>
#include <QPainter>
#include <QFontMetrics>
#include <QPen>
#include <QBrush>

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    // Cấu hình hiển thị
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QColor("#1e1e1e"));
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void CanvasView::drawNodes(const QString &folderPath)
{
    scene->clear();

    QDir dir(folderPath);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    int x = 50;
    int y = 50;

    for (const QFileInfo &fileInfo : list) {
        // Tạo khối Node (120x40)
        QGraphicsRectItem *node = new QGraphicsRectItem(0, 0, 120, 40);

        // Vàng cho thư mục, Xanh cho file
        if (fileInfo.isDir()) {
            node->setBrush(QBrush(QColor("#d4b856")));
        } else {
            node->setBrush(QBrush(QColor("#1793d1")));
        }
        node->setPen(QPen(Qt::NoPen));

        // Cho phép kéo thả và chọn
        node->setFlag(QGraphicsItem::ItemIsMovable);
        node->setFlag(QGraphicsItem::ItemIsSelectable);

        // Tên file trên Node
        QGraphicsTextItem *text = new QGraphicsTextItem(fileInfo.fileName(), node);
        text->setDefaultTextColor(Qt::white);
        text->setPos(5, 10);

        // Rút gọn tên nếu quá dài
        QFontMetrics metrics(text->font());
        QString elidedText = metrics.elidedText(fileInfo.fileName(), Qt::ElideRight, 110);
        text->setPlainText(elidedText);

        // Xếp hàng ngang, xuống dòng khi vượt 600px
        node->setPos(x, y);
        x += 150;
        if (x > 600) {
            x = 50;
            y += 70;
        }

        scene->addItem(node);
    }
}

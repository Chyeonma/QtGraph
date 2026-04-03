#include "codeviewer.h"
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QTextBlock>
#include <QWidget>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeViewer *viewer)
        : QWidget(viewer)
        , viewer(viewer)
    {
    }

    QSize sizeHint() const override
    {
        return QSize(viewer->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        viewer->lineNumberAreaPaintEvent(event);
    }

private:
    CodeViewer *viewer;
};

CodeViewer::CodeViewer(QWidget *parent)
    : QPlainTextEdit(parent)
    , lineNumberArea(new LineNumberArea(this))
    , showLineNumbers(false)
    , isEditableContent(false)
{
    connect(this, &QPlainTextEdit::blockCountChanged, this, [this]() {
        updateLineNumberAreaWidth();
    });
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeViewer::updateLineNumberArea);

    lineNumberArea->hide();
    updateLineNumberAreaWidth();
}

void CodeViewer::configureFileContext(const QString &filePath, const QString &displayName, bool editableContent)
{
    currentFilePath = filePath;
    currentDisplayName = displayName;
    isEditableContent = editableContent;
    setReadOnly(!editableContent);
}

void CodeViewer::updateFileContext(const QString &filePath, const QString &displayName)
{
    currentFilePath = filePath;
    currentDisplayName = displayName;
}

QString CodeViewer::filePath() const
{
    return currentFilePath;
}

QString CodeViewer::displayName() const
{
    return currentDisplayName;
}

bool CodeViewer::editableContent() const
{
    return isEditableContent;
}

void CodeViewer::setLineNumbersVisible(bool visible)
{
    if (showLineNumbers == visible) {
        return;
    }

    showLineNumbers = visible;
    lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth();
    viewport()->update();
}

bool CodeViewer::lineNumbersVisible() const
{
    return showLineNumbers;
}

int CodeViewer::lineNumberAreaWidth() const
{
    if (!showLineNumbers) {
        return 0;
    }

    int digits = 1;
    int maxLineCount = qMax(1, blockCount());
    while (maxLineCount >= 10) {
        maxLineCount /= 10;
        ++digits;
    }

    return 12 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeViewer::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!showLineNumbers) {
        return;
    }

    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor("#252526"));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString lineNumber = QString::number(blockNumber + 1);
            painter.setPen(QColor("#858585"));
            painter.drawText(0, top, lineNumberArea->width() - 6, fontMetrics().height(),
                             Qt::AlignRight, lineNumber);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeViewer::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect contentRect = contentsRect();
    lineNumberArea->setGeometry(QRect(contentRect.left(), contentRect.top(),
                                      lineNumberAreaWidth(), contentRect.height()));
}

void CodeViewer::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeViewer::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth();
    }
}

#ifndef CODEVIEWER_H
#define CODEVIEWER_H

#include <QPlainTextEdit>
#include <QString>

class QWidget;
class QPaintEvent;
class QResizeEvent;

class CodeViewer : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeViewer(QWidget *parent = nullptr);

    void configureFileContext(const QString &filePath, const QString &displayName, bool editableContent);
    void updateFileContext(const QString &filePath, const QString &displayName);
    QString filePath() const;
    QString displayName() const;
    bool editableContent() const;

    void setLineNumbersVisible(bool visible);
    bool lineNumbersVisible() const;
    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    bool showLineNumbers;
    QString currentFilePath;
    QString currentDisplayName;
    bool isEditableContent;
};

#endif // CODEVIEWER_H

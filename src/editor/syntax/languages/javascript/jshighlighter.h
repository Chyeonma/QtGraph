#ifndef JSHIGHLIGHTER_H
#define JSHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class JsHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JsHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat templateStringFormat;
};

#endif // JSHIGHLIGHTER_H

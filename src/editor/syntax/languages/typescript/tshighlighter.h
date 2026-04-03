#ifndef TSHIGHLIGHTER_H
#define TSHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class TsHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit TsHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat templateStringFormat;
};

#endif // TSHIGHLIGHTER_H

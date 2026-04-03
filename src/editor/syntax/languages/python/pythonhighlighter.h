#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class PythonHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit PythonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat tripleStringFormat;
};

#endif // PYTHONHIGHLIGHTER_H

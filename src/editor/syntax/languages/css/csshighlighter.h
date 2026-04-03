#ifndef CSSHIGHLIGHTER_H
#define CSSHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class CssHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CssHighlighter(QTextDocument *parent = nullptr);
};

#endif // CSSHIGHLIGHTER_H

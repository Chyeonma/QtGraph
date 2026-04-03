#ifndef HTMLHIGHLIGHTER_H
#define HTMLHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class HtmlHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit HtmlHighlighter(QTextDocument *parent = nullptr);
};

#endif // HTMLHIGHLIGHTER_H

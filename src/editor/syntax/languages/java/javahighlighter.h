#ifndef JAVAHIGHLIGHTER_H
#define JAVAHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class JavaHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JavaHighlighter(QTextDocument *parent = nullptr);
};

#endif // JAVAHIGHLIGHTER_H

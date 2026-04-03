#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class JsonHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JsonHighlighter(QTextDocument *parent = nullptr);
};

#endif // JSONHIGHLIGHTER_H

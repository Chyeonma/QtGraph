#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include "editor/syntax/base/syntaxhighlighter.h"

class CppHighlighter : public SyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CppHighlighter(QTextDocument *parent = nullptr);
};

#endif // CPPHIGHLIGHTER_H

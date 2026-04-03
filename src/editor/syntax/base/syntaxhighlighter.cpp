#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // 1. Áp dụng tất cả single-line rules
    for (const HighlightRule &rule : qAsConst(rules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // 2. Xử lý multi-line comments (/* ... */)
    if (commentStartPattern.pattern().isEmpty()) return;

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(commentStartPattern);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEndPattern.match(text, startIndex + 2);
        int endIndex = endMatch.capturedStart();
        int commentLength;

        if (endIndex == -1 || !endMatch.hasMatch()) {
            // Comment chưa kết thúc → đánh dấu block state = 1
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);

        QRegularExpressionMatch nextStart = commentStartPattern.match(text, startIndex + commentLength);
        startIndex = nextStart.hasMatch() ? nextStart.capturedStart() : -1;
    }
}

#include "htmlhighlighter.h"
#include <QColor>

HtmlHighlighter::HtmlHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat doctypeFormat;
    doctypeFormat.setForeground(QColor("#c586c0"));
    doctypeFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("<!DOCTYPE[^>]*>", QRegularExpression::CaseInsensitiveOption);
    rule.format = doctypeFormat;
    rules.append(rule);

    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor("#569cd6"));
    rule.pattern = QRegularExpression("</?\\s*[a-zA-Z][a-zA-Z0-9:-]*");
    rule.format = tagFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("/?>");
    rule.format = tagFormat;
    rules.append(rule);

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor("#9cdcfe"));
    rule.pattern = QRegularExpression("\\b[a-zA-Z_:][a-zA-Z0-9_:\\-.]*\\b(?=\\s*=)");
    rule.format = attributeFormat;
    rules.append(rule);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = stringFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = stringFormat;
    rules.append(rule);

    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("&[a-zA-Z0-9#]+;");
    rule.format = entityFormat;
    rules.append(rule);

    multiLineCommentFormat.setForeground(QColor("#6a9955"));
    commentStartPattern = QRegularExpression("<!--");
    commentEndPattern = QRegularExpression("-->");
}

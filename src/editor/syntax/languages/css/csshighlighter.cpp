#include "csshighlighter.h"
#include <QColor>
#include <QStringList>

CssHighlighter::CssHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat atRuleFormat;
    atRuleFormat.setForeground(QColor("#c586c0"));
    atRuleFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("@[a-zA-Z-]+");
    rule.format = atRuleFormat;
    rules.append(rule);

    QTextCharFormat selectorFormat;
    selectorFormat.setForeground(QColor("#d7ba7d"));
    rule.pattern = QRegularExpression("\\.[a-zA-Z_][a-zA-Z0-9_-]*");
    rule.format = selectorFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("#[a-zA-Z_][a-zA-Z0-9_-]*");
    rule.format = selectorFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression(":[a-zA-Z-]+");
    rule.format = selectorFormat;
    rules.append(rule);

    QTextCharFormat propertyFormat;
    propertyFormat.setForeground(QColor("#9cdcfe"));
    rule.pattern = QRegularExpression("\\b[a-zA-Z-]+(?=\\s*:)");
    rule.format = propertyFormat;
    rules.append(rule);

    QTextCharFormat valueKeywordFormat;
    valueKeywordFormat.setForeground(QColor("#4ec9b0"));
    QStringList valueKeywords = {
        "absolute", "auto", "block", "bold", "center", "fixed", "flex",
        "grid", "inherit", "inline", "none", "relative", "solid", "sticky"
    };
    for (const QString &valueKeyword : valueKeywords) {
        rule.pattern = QRegularExpression("\\b" + valueKeyword + "\\b");
        rule.format = valueKeywordFormat;
        rules.append(rule);
    }

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));
    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?(px|em|rem|vh|vw|%|s|ms|deg)?\\b");
    rule.format = numberFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("#[0-9a-fA-F]{3,8}\\b");
    rule.format = numberFormat;
    rules.append(rule);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
    rule.format = stringFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'");
    rule.format = stringFormat;
    rules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("\\b[a-zA-Z-]+(?=\\s*\\()");
    rule.format = functionFormat;
    rules.append(rule);

    multiLineCommentFormat.setForeground(QColor("#6a9955"));
    commentStartPattern = QRegularExpression("/\\*");
    commentEndPattern = QRegularExpression("\\*/");
}

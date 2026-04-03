#include "jsonhighlighter.h"
#include <QColor>

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat keyFormat;
    keyFormat.setForeground(QColor("#9cdcfe"));
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"(?=\\s*:)");
    rule.format = keyFormat;
    rules.append(rule);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
    rule.format = stringFormat;
    rules.append(rule);

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));
    rule.pattern = QRegularExpression("\\b-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?\\b");
    rule.format = numberFormat;
    rules.append(rule);

    QTextCharFormat literalFormat;
    literalFormat.setForeground(QColor("#569cd6"));
    rule.pattern = QRegularExpression("\\b(true|false|null)\\b");
    rule.format = literalFormat;
    rules.append(rule);

    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor("#d4d4d4"));
    rule.pattern = QRegularExpression("[\\{\\}\\[\\],:]");
    rule.format = punctuationFormat;
    rules.append(rule);
}

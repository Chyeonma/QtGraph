#include "javahighlighter.h"
#include <QColor>
#include <QStringList>

JavaHighlighter::JavaHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#c586c0"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "abstract", "assert", "break", "case", "catch", "class", "const",
        "continue", "default", "do", "else", "enum", "extends", "final",
        "finally", "for", "if", "implements", "import", "instanceof",
        "interface", "native", "new", "package", "private", "protected",
        "public", "return", "static", "strictfp", "super", "switch",
        "synchronized", "this", "throw", "throws", "transient", "try",
        "volatile", "while", "record", "sealed", "permits", "non-sealed",
        "var"
    };

    for (const QString &keyword : keywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#4ec9b0"));

    QStringList types = {
        "boolean", "byte", "char", "double", "float", "int", "long",
        "short", "void", "String", "Object", "Integer", "Long",
        "Double", "Float", "Boolean", "Character", "List", "Map", "Set"
    };

    for (const QString &type : types) {
        rule.pattern = QRegularExpression("\\b" + type + "\\b");
        rule.format = typeFormat;
        rules.append(rule);
    }

    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor("#569cd6"));
    QStringList constants = {"true", "false", "null"};
    for (const QString &constant : constants) {
        rule.pattern = QRegularExpression("\\b" + constant + "\\b");
        rule.format = constantFormat;
        rules.append(rule);
    }

    QTextCharFormat annotationFormat;
    annotationFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("@[a-zA-Z_][a-zA-Z0-9_]*");
    rule.format = annotationFormat;
    rules.append(rule);

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));
    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?[fFdDlL]?\\b");
    rule.format = numberFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+[lL]?\\b");
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
    rule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\s*\\()");
    rule.format = functionFormat;
    rules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor("#6a9955"));
    rule.pattern = QRegularExpression("//[^\\n]*");
    rule.format = singleLineCommentFormat;
    rules.append(rule);

    multiLineCommentFormat.setForeground(QColor("#6a9955"));
    commentStartPattern = QRegularExpression("/\\*");
    commentEndPattern = QRegularExpression("\\*/");
}

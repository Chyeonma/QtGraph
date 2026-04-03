#include "pythonhighlighter.h"
#include <QColor>
#include <QStringList>

namespace {
constexpr int kTripleDoubleState = 2;
constexpr int kTripleSingleState = 3;

int findNextTripleQuoteStart(const QString &text, int from, QString &delimiter)
{
    int doubleStart = text.indexOf(QStringLiteral("\"\"\""), from);
    int singleStart = text.indexOf(QStringLiteral("'''"), from);

    if (doubleStart == -1 && singleStart == -1) {
        return -1;
    }

    if (doubleStart != -1 && (singleStart == -1 || doubleStart < singleStart)) {
        delimiter = QStringLiteral("\"\"\"");
        return doubleStart;
    }

    delimiter = QStringLiteral("'''");
    return singleStart;
}
}

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#c586c0"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "False", "None", "True", "and", "as", "assert", "async", "await",
        "break", "case", "class", "continue", "def", "del", "elif", "else",
        "except", "finally", "for", "from", "global", "if", "import", "in",
        "is", "lambda", "match", "nonlocal", "not", "or", "pass", "raise",
        "return", "try", "while", "with", "yield"
    };

    for (const QString &keyword : keywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#4ec9b0"));

    QStringList types = {
        "int", "float", "bool", "str", "list", "dict", "set", "tuple",
        "bytes", "bytearray", "complex", "object", "range", "type"
    };

    for (const QString &type : types) {
        rule.pattern = QRegularExpression("\\b" + type + "\\b");
        rule.format = typeFormat;
        rules.append(rule);
    }

    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("@[a-zA-Z_][a-zA-Z0-9_\\.]*");
    rule.format = decoratorFormat;
    rules.append(rule);

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));
    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?\\b");
    rule.format = numberFormat;
    rules.append(rule);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    tripleStringFormat = stringFormat;
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

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor("#6a9955"));
    rule.pattern = QRegularExpression("#[^\\n]*");
    rule.format = commentFormat;
    rules.append(rule);
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    SyntaxHighlighter::highlightBlock(text);
    setCurrentBlockState(0);

    QString delimiter;
    int startIndex = -1;

    if (previousBlockState() == kTripleDoubleState) {
        delimiter = QStringLiteral("\"\"\"");
        startIndex = 0;
    } else if (previousBlockState() == kTripleSingleState) {
        delimiter = QStringLiteral("'''");
        startIndex = 0;
    } else {
        startIndex = findNextTripleQuoteStart(text, 0, delimiter);
    }

    while (startIndex >= 0) {
        int endIndex = text.indexOf(delimiter, startIndex + 3);
        if (endIndex == -1) {
            setFormat(startIndex, text.length() - startIndex, tripleStringFormat);
            setCurrentBlockState(delimiter == QStringLiteral("\"\"\"") ? kTripleDoubleState : kTripleSingleState);
            return;
        }

        int tokenLength = endIndex - startIndex + 3;
        setFormat(startIndex, tokenLength, tripleStringFormat);
        startIndex = findNextTripleQuoteStart(text, startIndex + tokenLength, delimiter);
    }
}

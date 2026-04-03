#include "tshighlighter.h"
#include <QColor>
#include <QStringList>

namespace {
constexpr int kTemplateStringState = 2;

bool isEscapedBacktick(const QString &text, int index)
{
    int backslashCount = 0;
    int cursor = index - 1;

    while (cursor >= 0 && text.at(cursor) == '\\') {
        ++backslashCount;
        --cursor;
    }

    return (backslashCount % 2) == 1;
}

int findUnescapedBacktick(const QString &text, int from)
{
    for (int i = from; i < text.length(); ++i) {
        if (text.at(i) == '`' && !isEscapedBacktick(text, i)) {
            return i;
        }
    }
    return -1;
}
}

TsHighlighter::TsHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#c586c0"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "abstract", "any", "as", "asserts", "async", "await", "break",
        "case", "catch", "class", "const", "constructor", "continue",
        "debugger", "declare", "default", "delete", "do", "else", "enum",
        "export", "extends", "finally", "for", "from", "function", "get",
        "if", "implements", "import", "in", "infer", "instanceof",
        "interface", "is", "keyof", "let", "module", "namespace", "new",
        "of", "override", "private", "protected", "public", "readonly",
        "return", "set", "static", "super", "switch", "this", "throw",
        "try", "type", "typeof", "var", "void", "while", "with", "yield"
    };

    for (const QString &keyword : keywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#4ec9b0"));

    QStringList types = {
        "Array", "BigInt", "Boolean", "Date", "Error", "Function", "Map",
        "Number", "Object", "Promise", "Record", "RegExp", "Set", "String",
        "Symbol", "never", "null", "number", "object", "string", "symbol",
        "tuple", "undefined", "unknown", "boolean"
    };

    for (const QString &type : types) {
        rule.pattern = QRegularExpression("\\b" + type + "\\b");
        rule.format = typeFormat;
        rules.append(rule);
    }

    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("@[a-zA-Z_$][a-zA-Z0-9_$]*");
    rule.format = decoratorFormat;
    rules.append(rule);

    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor("#569cd6"));
    QStringList constants = {"true", "false", "null", "undefined", "NaN", "Infinity"};
    for (const QString &constant : constants) {
        rule.pattern = QRegularExpression("\\b" + constant + "\\b");
        rule.format = constantFormat;
        rules.append(rule);
    }

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));
    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?\\b");
    rule.format = numberFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+\\b");
    rule.format = numberFormat;
    rules.append(rule);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));
    templateStringFormat = stringFormat;
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
    rule.format = stringFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'");
    rule.format = stringFormat;
    rules.append(rule);

    rule.pattern = QRegularExpression("`([^`\\\\]|\\\\.)*`");
    rule.format = stringFormat;
    rules.append(rule);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor("#dcdcaa"));
    rule.pattern = QRegularExpression("\\b[a-zA-Z_$][a-zA-Z0-9_$]*(?=\\s*\\()");
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

void TsHighlighter::highlightBlock(const QString &text)
{
    SyntaxHighlighter::highlightBlock(text);

    // Tránh va chạm với trạng thái comment nhiều dòng của base class (state = 1)
    if (previousBlockState() == 1) {
        return;
    }

    int baseState = currentBlockState();
    int searchStart = 0;

    if (previousBlockState() == kTemplateStringState) {
        int templateEnd = findUnescapedBacktick(text, 0);
        if (templateEnd == -1) {
            setFormat(0, text.length(), templateStringFormat);
            setCurrentBlockState(kTemplateStringState);
            return;
        }

        setFormat(0, templateEnd + 1, templateStringFormat);
        searchStart = templateEnd + 1;
    }

    while (searchStart < text.length()) {
        int templateStart = findUnescapedBacktick(text, searchStart);
        if (templateStart == -1) {
            break;
        }

        int templateEnd = findUnescapedBacktick(text, templateStart + 1);
        if (templateEnd == -1) {
            setFormat(templateStart, text.length() - templateStart, templateStringFormat);
            setCurrentBlockState(kTemplateStringState);
            return;
        }

        setFormat(templateStart, templateEnd - templateStart + 1, templateStringFormat);
        searchStart = templateEnd + 1;
    }

    setCurrentBlockState(baseState);
}

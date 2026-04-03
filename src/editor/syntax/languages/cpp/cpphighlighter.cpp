#include "cpphighlighter.h"

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightRule rule;

    // ==========================================
    // 1. KEYWORDS (tím - #c586c0)
    // ==========================================
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor("#c586c0"));
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto",
        "bitand", "bitor", "break",
        "case", "catch", "class", "compl", "concept", "const",
        "consteval", "constexpr", "constinit", "const_cast", "continue",
        "co_await", "co_return", "co_yield",
        "decltype", "default", "delete", "do", "dynamic_cast",
        "else", "enum", "explicit", "export", "extern",
        "for", "friend",
        "goto",
        "if", "inline",
        "mutable",
        "namespace", "new", "noexcept", "not", "not_eq",
        "operator", "or", "or_eq",
        "private", "protected", "public",
        "register", "reinterpret_cast", "requires", "return",
        "sizeof", "static", "static_assert", "static_cast",
        "struct", "switch",
        "template", "this", "throw", "try", "typedef", "typeid", "typename",
        "union", "using",
        "virtual", "volatile",
        "while",
        "xor", "xor_eq",
        "override", "final",
        "emit", "signals", "slots", "Q_OBJECT"
    };

    for (const QString &keyword : keywords) {
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // ==========================================
    // 2. TYPES (xanh lá - #4ec9b0)
    // ==========================================
    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor("#4ec9b0"));

    QStringList types = {
        "bool", "char", "char8_t", "char16_t", "char32_t",
        "double", "float", "int", "long", "short",
        "signed", "unsigned", "void", "wchar_t",
        "size_t", "int8_t", "int16_t", "int32_t", "int64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "nullptr_t", "string",
        // Qt common types
        "QString", "QStringList", "QList", "QVector", "QMap", "QHash",
        "QSet", "QPair", "QVariant", "QObject", "QWidget",
        "QFile", "QDir", "QFileInfo",
        "QColor", "QBrush", "QPen", "QFont", "QRect", "QPoint", "QSize"
    };

    for (const QString &type : types) {
        rule.pattern = QRegularExpression("\\b" + type + "\\b");
        rule.format = typeFormat;
        rules.append(rule);
    }

    // ==========================================
    // 3. PREPROCESSOR (cam - #ce9178)
    // ==========================================
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor("#c586c0"));

    rule.pattern = QRegularExpression("^\\s*#\\s*\\w+");
    rule.format = preprocessorFormat;
    rules.append(rule);

    // ==========================================
    // 4. NUMBERS (xanh nhạt - #b5cea8)
    // ==========================================
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8"));

    rule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?[fFlLuU]*\\b");
    rule.format = numberFormat;
    rules.append(rule);

    // Hex
    rule.pattern = QRegularExpression("\\b0[xX][0-9a-fA-F]+[uUlL]*\\b");
    rules.append(rule);

    // ==========================================
    // 5. STRINGS (cam nhạt - #ce9178)
    // ==========================================
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178"));

    // Double-quoted strings
    rule.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\"");
    rule.format = stringFormat;
    rules.append(rule);

    // Single-quoted chars
    rule.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'");
    rules.append(rule);

    // ==========================================
    // 6. INCLUDE PATH (cam nhạt - #ce9178) 
    // ==========================================
    rule.pattern = QRegularExpression("<[a-zA-Z0-9_/\\.]+>");
    rule.format = stringFormat;
    rules.append(rule);

    // ==========================================
    // 7. FUNCTIONS (vàng nhạt - #dcdcaa)
    // ==========================================
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor("#dcdcaa"));

    rule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\s*\\()");
    rule.format = functionFormat;
    rules.append(rule);

    // ==========================================
    // 8. SINGLE-LINE COMMENTS (xám - #6a9955)
    // ==========================================
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor("#6a9955"));

    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    rules.append(rule);

    // ==========================================
    // 9. MULTI-LINE COMMENTS (xám - #6a9955)
    // ==========================================
    multiLineCommentFormat.setForeground(QColor("#6a9955"));
    commentStartPattern = QRegularExpression("/\\*");
    commentEndPattern = QRegularExpression("\\*/");

    // ==========================================
    // 10. CONSTANTS (xanh dương - #569cd6)
    // ==========================================
    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor("#569cd6"));

    QStringList constants = {"true", "false", "nullptr", "NULL", "TRUE", "FALSE"};
    for (const QString &c : constants) {
        rule.pattern = QRegularExpression("\\b" + c + "\\b");
        rule.format = constantFormat;
        rules.append(rule);
    }
}

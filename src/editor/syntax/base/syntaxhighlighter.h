#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

// Một quy tắc highlight: pattern regex + format màu
struct HighlightRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

// Base class — xử lý loop qua các rules, subclass chỉ cần khai báo rules
class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    // Qt gọi hàm này cho mỗi dòng text
    void highlightBlock(const QString &text) override;

    // Subclass thêm rules vào đây
    QVector<HighlightRule> rules;

    // Format cho multi-line comment (/* ... */) — cần xử lý đặc biệt
    QTextCharFormat multiLineCommentFormat;
    QRegularExpression commentStartPattern;
    QRegularExpression commentEndPattern;
};

#endif // SYNTAXHIGHLIGHTER_H

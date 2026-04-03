#ifndef HIGHLIGHTERFACTORY_H
#define HIGHLIGHTERFACTORY_H

#include "editor/syntax/base/syntaxhighlighter.h"
#include <QString>

class HighlighterFactory
{
public:
    // Tạo highlighter phù hợp dựa trên đuôi file
    // Trả về nullptr nếu chưa hỗ trợ ngôn ngữ đó
    static SyntaxHighlighter* createForFile(const QString &filePath, QTextDocument *document);

    // Kiểm tra file có được hỗ trợ highlight không
    static bool isSupported(const QString &filePath);
};

#endif // HIGHLIGHTERFACTORY_H

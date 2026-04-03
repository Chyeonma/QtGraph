#ifndef IHIGHLIGHTERFACTORY_H
#define IHIGHLIGHTERFACTORY_H

#include <QString>
#include <functional>

class QSyntaxHighlighter;
class QTextDocument;

class IHighlighterFactory
{
public:
    virtual ~IHighlighterFactory() = default;

    // Tạo highlighter phù hợp dựa trên đuôi file
    // Trả về nullptr nếu chưa hỗ trợ ngôn ngữ đó
    virtual QSyntaxHighlighter* createForFile(const QString &filePath,
                                              QTextDocument *document) = 0;

    // Kiểm tra file có được hỗ trợ highlight không
    virtual bool isSupported(const QString &filePath) const = 0;

    // Extension point cho plugins — đăng ký highlighter mới theo file extension
    using HighlighterCreator = std::function<QSyntaxHighlighter*(QTextDocument*)>;
    virtual void registerExtension(const QString &extension,
                                   HighlighterCreator creator) = 0;
};

#endif // IHIGHLIGHTERFACTORY_H

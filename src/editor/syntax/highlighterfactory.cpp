#include "highlighterfactory.h"
#include "editor/syntax/languages/cpp/cpphighlighter.h"
#include "editor/syntax/languages/python/pythonhighlighter.h"
#include "editor/syntax/languages/java/javahighlighter.h"
#include "editor/syntax/languages/javascript/jshighlighter.h"
#include "editor/syntax/languages/typescript/tshighlighter.h"
#include "editor/syntax/languages/html/htmlhighlighter.h"
#include "editor/syntax/languages/css/csshighlighter.h"
#include "editor/syntax/languages/json/jsonhighlighter.h"
#include <QFileInfo>

SyntaxHighlighter* HighlighterFactory::createForFile(const QString &filePath, QTextDocument *document)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();

    // C/C++
    if (suffix == "cpp" || suffix == "cxx" || suffix == "cc" ||
        suffix == "c" || suffix == "h" || suffix == "hpp" ||
        suffix == "hxx" || suffix == "hh" || suffix == "inl") {
        return new CppHighlighter(document);
    }

    // Python
    if (suffix == "py" || suffix == "pyw") {
        return new PythonHighlighter(document);
    }

    // Java
    if (suffix == "java") {
        return new JavaHighlighter(document);
    }

    // JavaScript
    if (suffix == "js" || suffix == "mjs" || suffix == "cjs" || suffix == "jsx") {
        return new JsHighlighter(document);
    }

    // TypeScript
    if (suffix == "ts" || suffix == "tsx" || suffix == "mts" || suffix == "cts") {
        return new TsHighlighter(document);
    }

    // HTML
    if (suffix == "html" || suffix == "htm" || suffix == "xhtml") {
        return new HtmlHighlighter(document);
    }

    // CSS
    if (suffix == "css" || suffix == "scss" || suffix == "less") {
        return new CssHighlighter(document);
    }

    // JSON
    if (suffix == "json" || suffix == "jsonc" || suffix == "json5") {
        return new JsonHighlighter(document);
    }

    return nullptr; // Không hỗ trợ → hiển thị text thường
}

bool HighlighterFactory::isSupported(const QString &filePath)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    QStringList supported = {
        "cpp", "cxx", "cc", "c", "h", "hpp", "hxx", "hh", "inl",
        "py", "pyw",
        "java",
        "js", "mjs", "cjs", "jsx",
        "ts", "tsx", "mts", "cts",
        "html", "htm", "xhtml",
        "css", "scss", "less",
        "json", "jsonc", "json5"
    };
    return supported.contains(suffix);
}

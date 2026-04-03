#include "highlighterregistry.h"
#include "editor/syntax/languages/cpp/cpphighlighter.h"
#include "editor/syntax/languages/python/pythonhighlighter.h"
#include "editor/syntax/languages/java/javahighlighter.h"
#include "editor/syntax/languages/javascript/jshighlighter.h"
#include "editor/syntax/languages/typescript/tshighlighter.h"
#include "editor/syntax/languages/html/htmlhighlighter.h"
#include "editor/syntax/languages/css/csshighlighter.h"
#include "editor/syntax/languages/json/jsonhighlighter.h"
#include <QFileInfo>

HighlighterRegistry::HighlighterRegistry()
{
    registerDefaultHighlighters();
}

QSyntaxHighlighter* HighlighterRegistry::createForFile(const QString &filePath, QTextDocument *document)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    auto it = m_extensions.find(suffix);
    if (it != m_extensions.end()) {
        return it.value()(document);
    }
    return nullptr;
}

bool HighlighterRegistry::isSupported(const QString &filePath) const
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    return m_extensions.contains(suffix);
}

void HighlighterRegistry::registerExtension(const QString &extension, HighlighterCreator creator)
{
    m_extensions.insert(extension.toLower(), creator);
}

void HighlighterRegistry::registerDefaultHighlighters()
{
    auto createCpp = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new CppHighlighter(doc); };
    for (const QString &ext : {"cpp", "cxx", "cc", "c", "h", "hpp", "hxx", "hh", "inl"}) {
        registerExtension(ext, createCpp);
    }

    auto createPython = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new PythonHighlighter(doc); };
    for (const QString &ext : {"py", "pyw"}) {
        registerExtension(ext, createPython);
    }

    auto createJava = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new JavaHighlighter(doc); };
    registerExtension("java", createJava);

    auto createJs = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new JsHighlighter(doc); };
    for (const QString &ext : {"js", "mjs", "cjs", "jsx"}) {
        registerExtension(ext, createJs);
    }

    auto createTs = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new TsHighlighter(doc); };
    for (const QString &ext : {"ts", "tsx", "mts", "cts"}) {
        registerExtension(ext, createTs);
    }

    auto createHtml = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new HtmlHighlighter(doc); };
    for (const QString &ext : {"html", "htm", "xhtml"}) {
        registerExtension(ext, createHtml);
    }

    auto createCss = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new CssHighlighter(doc); };
    for (const QString &ext : {"css", "scss", "less"}) {
        registerExtension(ext, createCss);
    }

    auto createJson = [](QTextDocument *doc) -> QSyntaxHighlighter* { return new JsonHighlighter(doc); };
    for (const QString &ext : {"json", "jsonc", "json5"}) {
        registerExtension(ext, createJson);
    }
}

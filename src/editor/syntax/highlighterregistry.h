#ifndef HIGHLIGHTERREGISTRY_H
#define HIGHLIGHTERREGISTRY_H

#include "interfaces/ihighlighterfactory.h"
#include <QMap>

class HighlighterRegistry : public IHighlighterFactory
{
public:
    HighlighterRegistry();

    QSyntaxHighlighter* createForFile(const QString &filePath, QTextDocument *document) override;
    bool isSupported(const QString &filePath) const override;
    void registerExtension(const QString &extension, HighlighterCreator creator) override;

private:
    void registerDefaultHighlighters();

    QMap<QString, HighlighterCreator> m_extensions;
};

#endif // HIGHLIGHTERREGISTRY_H

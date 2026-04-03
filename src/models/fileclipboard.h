#ifndef FILECLIPBOARD_H
#define FILECLIPBOARD_H

#include <QString>

struct FileClipboard
{
    enum Mode {
        None,
        Copy,
        Cut
    };

    QString path;
    Mode mode = None;

    bool hasContent() const
    {
        return mode != None && !path.isEmpty();
    }

    void clear()
    {
        path.clear();
        mode = None;
    }
};

#endif // FILECLIPBOARD_H

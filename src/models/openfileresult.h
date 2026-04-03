#ifndef OPENFILERESULT_H
#define OPENFILERESULT_H

#include <QString>

struct OpenFileResult
{
    enum Status {
        Ok,
        Binary,
        TooLarge,
        ReadError
    };

    Status status = Ok;
    QString content;
    QString previewMessage;
    bool editable = false;
};

#endif // OPENFILERESULT_H

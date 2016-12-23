#ifndef FINSYNTAXERROR_H
#define FINSYNTAXERROR_H


#include <QString>

#include "finErrorCode.h"

class finSyntaxError
{
protected:
    unsigned int _row;
    unsigned int _column;
    QString _errString;

public:
    finSyntaxError();

    unsigned int getRow() const;
    unsigned int getColumn() const;
    QString getErrorString() const;

    finErrorCode setRow(unsigned int row);
    finErrorCode setColumn(unsigned int col);
    finErrorCode setErrorString(const QString &errstr);
};

#endif // FINSYNTAXERROR_H

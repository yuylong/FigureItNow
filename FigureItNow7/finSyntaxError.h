#ifndef FINSYNTAXERROR_H
#define FINSYNTAXERROR_H


#include <QString>

#include "finErrorCode.h"
#include "finLexNode.h"

class finSyntaxError
{
protected:
    unsigned int _row;
    unsigned int _column;
    QString _errString;

public:
    finSyntaxError();
    finSyntaxError(const finSyntaxError &src);

    finErrorCode copySyntaxError(const finSyntaxError *src);

    unsigned int getRow() const;
    unsigned int getColumn() const;
    QString getErrorString() const;

    finErrorCode setRow(unsigned int row);
    finErrorCode setColumn(unsigned int col);
    finErrorCode setErrorString(const QString &errstr);

    static finErrorCode appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                             const QString &errinfo);
};

#endif // FINSYNTAXERROR_H

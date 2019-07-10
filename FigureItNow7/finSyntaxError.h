/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXERROR_H
#define FINSYNTAXERROR_H

#include <QString>
#include <QTextStream>

#include "finErrorCode.h"
#include "finLexNode.h"


class finSyntaxError
{
public:
    enum Level {
        LV_DUMMY,
        LV_DEBUG,
        LV_INFO,
        LV_WARNING,
        LV_ERROR,
    };

    enum Stage {
        ST_DUMMY,
        ST_COMPILE,
        ST_EXECUTE,
    };

protected:
    Level _level;
    Stage _stage;
    unsigned int _row;
    unsigned int _column;
    QString _errString;

public:
    finSyntaxError();
    finSyntaxError(const finSyntaxError &src);

    finErrorCode copySyntaxError(const finSyntaxError *src);

    Level getLevel() const;
    Stage getStage() const;
    unsigned int getRow() const;
    unsigned int getColumn() const;
    QString getErrorString() const;

    finErrorCode setLevel(Level level);
    finErrorCode setStage(Stage stage);
    finErrorCode setRow(unsigned int row);
    finErrorCode setColumn(unsigned int col);
    finErrorCode setErrorString(const QString &errstr);

    finErrorCode dumpErrorInfo(QTextStream *ts) const;
    QString makeErrorInfoString() const;

    static const finSyntaxError &dummySyntaxError();

    static QString getLevelName(Level level);
    static QString getStageName(Stage stage);

    static finErrorCode appendExecutionError(const finLexNode *lexnode, QList<finSyntaxError> *errlist,
                                             const QString &errinfo);
};

#endif // FINSYNTAXERROR_H

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#ifndef FINSYNTAXERRORLIST_H
#define FINSYNTAXERRORLIST_H

#include <QList>
#include <QString>
#include <QTextStream>

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finSyntaxError.h"


class finSyntaxErrorList
{
protected:
    QList<finSyntaxError> _list;
    QTextStream *_ts;
    finSyntaxError::Level _fromLevel;

public:
    finSyntaxErrorList();

    int getEntryCountFromLevel(finSyntaxError::Level level) const;
    int getEntryCountFromLevel() const;
    int getEntryCountAtLevel(finSyntaxError::Level level) const;
    int getEntryCount() const;
    const finSyntaxError &getEntryAt(int index) const;

    finErrorCode appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
                             finLexNode *lexnode, QString info);
    finErrorCode removeEntryAt(int index);
    void clearAllErrorList();

    finErrorCode dumpList(QTextStream *ts, finSyntaxError::Level fromlevel) const;
    finErrorCode dumpList(QTextStream *ts) const;
    finErrorCode dumpList(finSyntaxError::Level fromlevel) const;
    finErrorCode dumpList() const;
};

#endif // FINSYNTAXERRORLIST_H

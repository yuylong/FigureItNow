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
#include "finSyntaxErrorDump.h"
#include "finLexNode.h"
#include "finSyntaxError.h"


class finSyntaxErrorList
{
protected:
    QList<finSyntaxError> _list;

    bool _rtDump;
    finSyntaxErrorDump *_dumper;
    finSyntaxError::Level _startingLevel;

public:
    finSyntaxErrorList();

    int getEntryCountFromLevel(finSyntaxError::Level level) const;
    int getEntryCountFromLevel() const;
    int getEntryCountAtLevel(finSyntaxError::Level level) const;
    int getEntryCount() const;
    const finSyntaxError &getEntryAt(int index) const;

    bool isRealtimeDump() const;
    finSyntaxErrorDump *getDumper() const;
    finSyntaxError::Level getDumpStartingLevel() const;

    finErrorCode appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
                             finLexNode *lexnode, QString info);
    finErrorCode removeEntryAt(int index);
    void clearAllErrorList();

    finErrorCode setIsRealtimeDump(bool rtdump);
    finErrorCode setDumper(finSyntaxErrorDump *dumper);
    finErrorCode setDumpStartingLevel(finSyntaxError::Level level);

    finErrorCode dumpList(finSyntaxErrorDump *dumper, finSyntaxError::Level startinglevel) const;
    finErrorCode dumpList(finSyntaxError::Level startinglevel) const;
    finErrorCode dumpList() const;
};

#endif // FINSYNTAXERRORLIST_H

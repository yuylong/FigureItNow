/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2019 Yulong Yu. All rights reserved.
 */

#include "finSyntaxErrorList.h"

finSyntaxErrorList::finSyntaxErrorList()
{
    this->_list.clear();
    this->_ts = nullptr;
    this->_fromLevel = finSyntaxError::LV_WARNING;
}

int finSyntaxErrorList::getEntryCountFromLevel(finSyntaxError::Level level) const
{
    int retcnt = 0;
    foreach (const finSyntaxError &entry, this->_list){
        if ( entry.getLevel() >= level )
            retcnt++;
    }

    return retcnt;
}

int finSyntaxErrorList::getEntryCountFromLevel() const
{
    return getEntryCountFromLevel(this->_fromLevel);
}

int finSyntaxErrorList::getEntryCountAtLevel(finSyntaxError::Level level) const
{
    int retcnt = 0;
    foreach (const finSyntaxError &entry, this->_list){
        if ( entry.getLevel() == level )
            retcnt++;
    }

    return retcnt;
}

int finSyntaxErrorList::getEntryCount() const
{
    return this->_list.count();
}

const finSyntaxError &finSyntaxErrorList::getEntryAt(int index) const
{
    if ( index < 0 || index >= this->_list.count() )
        return finSyntaxError::dummySyntaxError();

    return this->_list.at(index);
}

bool finSyntaxErrorList::isRealtimeDump() const
{
    return this->_rtDump;
}

QTextStream *finSyntaxErrorList::getDumpTextStream() const
{
    return this->_ts;
}

finSyntaxError::Level finSyntaxErrorList::getDumpFromLevel() const
{
    return this->_fromLevel;
}

finErrorCode finSyntaxErrorList::appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
                                             finLexNode *lexnode, QString info)
{
    finSyntaxError entry;

    entry.setLevel(level);
    entry.setStage(stage);
    if ( lexnode != nullptr ) {
        entry.setRow(lexnode->getRow());
        entry.setColumn(lexnode->getColumn());
    }else{
        entry.setRow(0);
        entry.setColumn(0);
    }
    entry.setErrorString(info);

    this->_list.append(entry);

    if ( this->_rtDump && this->_ts != nullptr && level >= this->_fromLevel )
        entry.dumpErrorInfo(this->_ts);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxErrorList::removeEntryAt(int index)
{
    if ( index < 0 || index >= this->_list.count() )
        return finErrorKits::EC_INVALID_PARAM;

    this->_list.removeAt(index);
    return finErrorKits::EC_SUCCESS;
}

void finSyntaxErrorList::clearAllErrorList()
{
    this->_list.clear();
}

finErrorCode finSyntaxErrorList::setIsRealtimeDump(bool rtdump)
{
    this->_rtDump = rtdump;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxErrorList::setDumpTextStream(QTextStream *ts)
{
    this->_ts = ts;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxErrorList::setDumpFromLevel(finSyntaxError::Level level)
{
    this->_fromLevel = level;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxErrorList::dumpList(QTextStream *ts, finSyntaxError::Level fromlevel) const
{
    finErrorCode errcode = finErrorKits::EC_SUCCESS;
    if ( ts == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    foreach ( const finSyntaxError &entry, this->_list ) {
        if ( entry.getLevel() >= fromlevel ) {
            errcode = entry.dumpErrorInfo(ts);
            if ( finErrorKits::isErrorResult(errcode) )
                continue;
        }
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finSyntaxErrorList::dumpList(QTextStream *ts) const
{
    return this->dumpList(ts, this->_fromLevel);
}

finErrorCode finSyntaxErrorList::dumpList(finSyntaxError::Level fromlevel) const
{
    return this->dumpList(this->_ts, fromlevel);
}

finErrorCode finSyntaxErrorList::dumpList() const
{
    return this->dumpList(this->_ts, this->_fromLevel);
}

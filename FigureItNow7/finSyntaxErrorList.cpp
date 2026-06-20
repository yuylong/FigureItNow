/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finSyntaxErrorList.h"

finSyntaxErrorList::finSyntaxErrorList()
{
    this->_list.clear();
    this->_dumper = nullptr;
    this->_startingLevel = finSyntaxError::LV_WARNING;
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
    return getEntryCountFromLevel(this->_startingLevel);
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

finSyntaxErrorDump *finSyntaxErrorList::getDumper() const
{
    return this->_dumper;
}

finSyntaxError::Level finSyntaxErrorList::getDumpStartingLevel() const
{
    return this->_startingLevel;
}

void finSyntaxErrorList::appendEntry(finSyntaxError::Level level, finSyntaxError::Stage stage,
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

    if ( this->_rtDump && this->_dumper != nullptr && level >= this->_startingLevel )
        entry.dumpErrorInfo(this->_dumper);
}

void finSyntaxErrorList::removeEntryAt(int index)
{
    if ( index < 0 || index >= this->_list.count() )
        finThrow(finErrorKits::EC_INVALID_PARAM,
                 QString("Index %1 out of range (size %2).")
                     .arg(index).arg(this->_list.count()));

    this->_list.removeAt(index);
}

void finSyntaxErrorList::clearAllErrorList()
{
    this->_list.clear();
}

void finSyntaxErrorList::setIsRealtimeDump(bool rtdump)
{
    this->_rtDump = rtdump;
}

void finSyntaxErrorList::setDumper(finSyntaxErrorDump *dumper)
{
    this->_dumper = dumper;
}

void finSyntaxErrorList::setDumpStartingLevel(finSyntaxError::Level level)
{
    this->_startingLevel = level;
}

void finSyntaxErrorList::dumpList(finSyntaxErrorDump *dumper, finSyntaxError::Level startinglevel) const
{
    if ( dumper == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Cannot dump syntax error list to a NULL dumper.");

    foreach ( const finSyntaxError &entry, this->_list ) {
        if ( entry.getLevel() >= startinglevel ) {
            entry.dumpErrorInfo(dumper);
        }
    }
}

void finSyntaxErrorList::dumpList(finSyntaxError::Level startinglevel) const
{
    this->dumpList(this->_dumper, startinglevel);
}

void finSyntaxErrorList::dumpList() const
{
    this->dumpList(this->_dumper, this->_startingLevel);
}

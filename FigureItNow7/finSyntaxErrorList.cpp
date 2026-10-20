/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finSyntaxErrorList.cpp
 *  \brief Implementations of the pipeline diagnostic list.
 *
 *  Provides the counting and lookup accessors declared in finSyntaxErrorList.h, the append and
 *  removal paths including the realtime dump hook, and the dump overloads that filter entries by
 *  severity before forwarding them to a finSyntaxErrorDump sink.
 */

#include "finSyntaxErrorList.h"

// A fresh list collects nothing, owns no sink, and starts at the warning level so that debug and
// info entries stay out of user-facing output unless the caller lowers the threshold.
finSyntaxErrorList::finSyntaxErrorList()
{
    this->_list.clear();
    this->_dumper = nullptr;
    this->_startingLevel = finSyntaxError::LV_WARNING;
}

// Threshold count: levels are ordered by severity, so "at least as severe as" is a plain >=
// comparison over the whole list.
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

// Deliberately lenient lookup: the front ends index while repainting, so an out-of-range request
// yields the shared dummy entry instead of throwing. Use getEntryCount() to check first.
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

// Producer-side append: take the source position from the offending token, falling back to (0, 0)
// when the caller has no token, so an entry never keeps coordinates from a previous diagnostic.
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

// Forwarding append used when diagnostics were already built elsewhere (e.g. imported from the
// syntax tree), so every field survives the copy.
void finSyntaxErrorList::appendEntry(const finSyntaxError &entry)
{
    this->_list.append(entry);

    if ( this->_rtDump && this->_dumper != nullptr && entry.getLevel() >= this->_startingLevel )
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

// Clearing keeps the sink, the realtime flag, and the starting level, so the same list can collect
// the next run's diagnostics without reconfiguration.
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

// Both convenience overloads funnel through this one, so the severity filter and the formatting
// stay in a single place. A missing sink is a caller bug: without one there is no output at all.
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

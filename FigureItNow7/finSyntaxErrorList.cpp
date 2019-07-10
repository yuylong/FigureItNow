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

finErrorCode finSyntaxErrorList::dumpList(QTextStream *ts, finSyntaxError::Level dumplevel) const
{

}

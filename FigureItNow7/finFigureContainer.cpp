/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2017-2026 Yulong Yu. All rights reserved.
 */

#include "finFigureContainer.h"

finFigureContainer::finFigureContainer()
{
    finFigureConfig::cloneFromDefaultFigureConfig(&this->_curFigCfg);
}

finFigureContainer::~finFigureContainer()
{
    this->clearFigureObjects();
}

const finFigureConfig *finFigureContainer::getFigureConfig() const
{
    return &this->_curFigCfg;
}

finFigureConfig *finFigureContainer::getFigureConfig()
{
    return &this->_curFigCfg;
}

const finGraphConfig *finFigureContainer::getGraphConfig() const
{
    return &this->_graphCfg;
}

finGraphConfig *finFigureContainer::getGraphConfig()
{
    return &this->_graphCfg;
}

void finFigureContainer::setFigureConfigForObject(finFigureObject *figobj) const
{
    this->_curFigCfg.cloneFigureConfig(figobj->getFigureConfig());
}

int finFigureContainer::getFigureObjectCount() const
{
    return this->_figList.count();
}

finFigureObject *finFigureContainer::getFigureObjectAt(int idx)
{
    return this->_figList.at(idx);
}

void finFigureContainer::appendFigureObject(finFigureObject *figobj)
{
    this->setFigureConfigForObject(figobj);
    this->_figList.append(figobj);
}

void finFigureContainer::clearFigureObjects()
{
    while ( !this->_figList.empty() ) {
        finFigureObject *figobj = this->_figList.first();
        this->_figList.removeFirst();
        delete figobj;
    }
}

void finFigureContainer::dump() const
{
    // FIXME: Use QT C++ stream function instead.
    printf ("Figure Container (Cnt: %ld)\n", (long)this->_figList.count());

    for ( int i = 0; i < this->_figList.count(); i++ )
        this->_figList.at(i)->dump();
}

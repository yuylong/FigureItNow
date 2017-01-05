/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161230    0  Yulong Yu    Create this file.
 * 20170102    1  Yulong Yu    Add graph config into figure container.
 */

#include "finFigureContainer.h"

finFigureContainer::finFigureContainer()
{
    finFigureConfig::cloneFromDefaultFigureConfig(&this->_curFigCfg);
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

finErrorCode finFigureContainer::setFigureConfigForObject(finFigureObject *figobj) const
{
    return this->_curFigCfg.cloneFigureConfig(figobj->getFigureConfig());
}

int finFigureContainer::getFigureObjectCount() const
{
    return this->_figList.count();
}

finFigureObject *finFigureContainer::getFigureObjectAt(int idx)
{
    return this->_figList.at(idx);
}

finErrorCode finFigureContainer::appendFigureObject(finFigureObject *figobj)
{
    finErrorCode errcode;

    errcode = this->setFigureConfigForObject(figobj);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->_figList.append(figobj);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureContainer::dump() const
{
    printf ("Figure Container (Cnt: %d)\n", this->_figList.count());

    for ( int i = 0; i < this->_figList.count(); i++ )
        this->_figList.at(i)->dump();
}

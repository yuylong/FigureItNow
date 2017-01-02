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

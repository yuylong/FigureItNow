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

finErrorCode finFigureContainer::setFigureConfigForObject(finFigureObject *figobj) const
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

finErrorCode finFigureContainer::appendFigureObject(finFigureObject *figobj)
{
    this->_figList.append(figobj);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

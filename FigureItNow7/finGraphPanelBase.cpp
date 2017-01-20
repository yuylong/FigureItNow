#include "finGraphPanelBase.h"

finGraphPanelBase::finGraphPanelBase()
{
    /* Do Nothing. */
}

const finGraphConfig *finGraphPanelBase::getGraphConfig() const
{
    return &this->_config;
}

finGraphConfig *finGraphPanelBase::getGraphConfig()
{
    return &this->_config;
}

finErrorCode finGraphPanelBase::setGraphConfig(const finGraphConfig *cfg)
{
    return this->_config.copyGraphConfig(cfg);
}

int finGraphPanelBase::getFigurePathCount() const
{
    return this->_pathList.count();
}

finFigurePath finGraphPanelBase::getFigurePathAt(int idx) const
{
    static const finFigurePath deffigpath = finFigurePath();
    if ( idx < 0 || idx >= this->_pathList.count() )
        return deffigpath;

    return this->_pathList.at(idx);
}

finErrorCode finGraphPanelBase::clearFigurePathList()
{
    this->_pathList.clear();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelBase::setFigureContainer(finFigureContainer *figcontainer)
{
    if ( figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    this->_pathList.clear();
    this->setGraphConfig(figcontainer->getGraphConfig());

    finErrorCode errcode;
    int failed = 0, success = 0;

    for ( int i = 0; i < figcontainer->getFigureObjectCount(); i++ ) {
        finFigureObject *figobj = figcontainer->getFigureObjectAt(i);

        errcode = this->appendFigureObject(figobj);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            failed++;
            continue;
        }
        success++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    else if ( failed == 0 )
        return finErrorCodeKits::FIN_EC_SUCCESS;
    else
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finGraphPanelBase::appendFigureObject(finFigureObject *obj)
{
    if ( obj == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return obj->getPixelFigurePath(&this->_pathList, &this->_config);
}

finErrorCode finGraphPanelBase::appendFigurePath(const finFigurePath &path)
{
    this->_pathList.append(path);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelBase::draw()
{
    this->applyGraphConfig();

    int success = 0, failed = 0;
    for ( int i = 0; i < this->_pathList.count(); i++ ) {
        finErrorCode errcode = this->drawFigurePath(this->_pathList.at(i));
        if ( finErrorCodeKits::isErrorResult(errcode) )
            failed++;
        else
            success++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorCodeKits::FIN_EC_READ_ERROR;
    else if ( failed == 0 )
        return finErrorCodeKits::FIN_EC_SUCCESS;
    else
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finGraphPanelBase::applyGraphConfig() const
{
    return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finGraphPanelBase::drawFigurePath(const finFigurePath &path) const
{
    if (path.getPath().isEmpty())
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

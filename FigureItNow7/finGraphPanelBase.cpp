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

finErrorCode finGraphPanelBase::applyGraphConfig()
{
    return finErrorCodeKits::FIN_EC_NORMAL_WARN;
}

finErrorCode finGraphPanelBase::drawContainer(finFigureContainer *figcontainer)
{
    if ( figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finErrorCode errcode;
    int failed = 0, success = 0;

    this->applyGraphConfig();

    for ( int i = 0; i < figcontainer->getFigureObjectCount(); i++ ) {
        finFigureObject *figobj = figcontainer->getFigureObjectAt(i);

        errcode = this->drawObject(figobj);
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

finErrorCode finGraphPanelBase::drawObject(finFigureObject *obj)
{
    if ( obj == NULL )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    QList<finFigurePath> pathlist;
    finErrorCode errcode = obj->getPixelFigurePath(&pathlist, &this->_config);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    for ( int i = 0; i < pathlist.count(); i++ ) {
        const finFigurePath &path = pathlist.at(i);
        errcode = this->drawFigurePath(path);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelBase::drawFigurePath(const finFigurePath &path)
{
    if (path.getPath().isEmpty())
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

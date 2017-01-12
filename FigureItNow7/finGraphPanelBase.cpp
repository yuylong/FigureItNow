#include "finGraphPanelBase.h"

finGraphPanelBase::finGraphPanelBase()
{

}

const finGraphConfig *finGraphPanelBase::getGraphConfig() const
{
    return &this->_config;
}

finGraphConfig *finGraphPanelBase::getGraphConfig()
{
    return &this->_config;
}

finErrorCode finGraphPanelBase::drawContainer(finFigureContainer *figcontainer)
{
    finErrorCode errcode;
    int failed = 0, success = 0;

    for ( int i = 0; i < figcontainer->getFigureObjectCount(); i++ ) {
        finFigureObject *figobj = figcontainer->getFigureObjectAt(i);

        errcode = this->drawOneObject(figobj);
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

finErrorCode finGraphPanelBase::drawOneObject(finFigureObject *obj)
{
    if ( obj == NULL )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    switch ( obj->getFigureType() ) {
      case finFigureObject::FIN_FO_TYPE_LINE:
        return this->drawLine((finFigureObjectLine *)obj);
        break;

      default:
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
        break;
    }
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

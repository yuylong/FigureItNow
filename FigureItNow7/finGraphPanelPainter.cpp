#include "finGraphPanelPainter.h"

finGraphPanelPainter::finGraphPanelPainter()
{
    this->_painter = NULL;
}

QPainter *finGraphPanelPainter::getPainter() const
{
    return this->_painter;
}

finErrorCode finGraphPanelPainter::setPainter(QPainter *painter)
{
    this->_painter = painter;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::drawContainer(finFigureContainer *figcontainer)
{
    if ( this->_painter == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finGraphPanelBase::drawContainer(figcontainer);
}

finErrorCode finGraphPanelPainter::drawObject(finFigureObject *obj)
{
    if ( this->_painter == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;
    if ( obj == NULL )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    switch ( obj->getFigureType() ) {
      case finFigureObject::FIN_FO_TYPE_LINE:
        return this->drawObjLine((finFigureObjectLine *)obj);
        break;

      default:
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
        break;
    }
    return finErrorCodeKits::FIN_EC_UNKNOWN_ERROR;
}

finErrorCode finGraphPanelPainter::drawObjLine(finFigureObjectLine *line)
{
    this->_painter->drawLine(line->getPoint1(), line->getPoint2());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

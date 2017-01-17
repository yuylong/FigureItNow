#include "finGraphPanelPainter.h"

#include <QSizeF>
#include <QBrush>
#include <QColor>

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
      case finFigureObject::FIN_FO_TYPE_DOT:
      case finFigureObject::FIN_FO_TYPE_LINE:
      case finFigureObject::FIN_FO_TYPE_POLYLINE:
      case finFigureObject::FIN_FO_TYPE_RECT:
      case finFigureObject::FIN_FO_TYPE_POLYGON:
      case finFigureObject::FIN_FO_TYPE_ELLIPSE:
      case finFigureObject::FIN_FO_TYPE_TEXT:
        return this->drawObjPath(obj);
        break;

      case finFigureObject::FIN_FO_TYPE_LINE3D:
        return this->drawObjLine3D((finFigureObjectLine3D *)obj);
        break;

      default:
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
        break;
    }
    return finErrorCodeKits::FIN_EC_UNKNOWN_ERROR;
}

finErrorCode finGraphPanelPainter::applyGraphConfig()
{
    this->_painter->setBackground(this->_config.getBackgroundBrush());
    this->_painter->eraseRect(this->_config.getWholePanelPixelRect());
    this->_painter->setRenderHints(this->_config.getRenderHints());

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::applyFigureConfig(finFigureConfig *cfg)
{
    this->_painter->setPen(cfg->getBorderPen());
    this->_painter->setBrush(cfg->getFillBrush());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::applyTextFigureConfig(finFigureConfig *cfg)
{
    this->_painter->setPen(cfg->getTextPen());
    this->_painter->setBrush(cfg->getTextBrush());
    this->_painter->setFont(cfg->getFont());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::drawObjPath(finFigureObject *obj)
{
    if ( obj->hasFigurePath() ) {
        this->applyFigureConfig(obj->getFigureConfig());
        QPainterPath path = obj->getPixelPath(&this->_config);
        this->_painter->drawPath(path);
    }
    if ( obj->hasTextPath() ) {
        this->applyTextFigureConfig(obj->getFigureConfig());
        QPainterPath path = obj->getPixelTextPath(&this->_config);
        this->_painter->drawPath(path);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::drawObjLine3D(finFigureObjectLine3D *line3d)
{
    this->applyFigureConfig(line3d->getFigureConfig());

    QPointF pt1 = this->_config.transformPixelPoint3D(line3d->getPoint1());
    QPointF pt2 = this->_config.transformPixelPoint3D(line3d->getPoint2());
    this->_painter->drawLine(pt1, pt2);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

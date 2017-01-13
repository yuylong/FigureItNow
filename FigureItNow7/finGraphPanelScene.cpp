#include "finGraphPanelScene.h"

finGraphPanelScene::finGraphPanelScene()
{
    this->_scene = NULL;
}

QGraphicsScene *finGraphPanelScene::getScene() const
{
    return this->_scene;
}

finErrorCode finGraphPanelScene::setScene(QGraphicsScene *scene)
{
    this->_scene = scene;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelScene::applyGraphConfig()
{
    if ( this->_scene == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_scene->setBackgroundBrush(this->_config.getBackgroundBrush());
    this->_scene->setSceneRect(this->_config.getWholePanelPixelRect());
}

finErrorCode finGraphPanelScene::drawContainer(finFigureContainer *figcontainer)
{
    if ( this->_scene == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finGraphPanelBase::drawContainer(figcontainer);
}

finErrorCode finGraphPanelScene::drawObject(finFigureObject *obj)
{
    if ( this->_scene == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;
    if ( obj == NULL )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    switch ( obj->getFigureType() ) {
      case finFigureObject::FIN_FO_TYPE_LINE:
        return this->drawObjLine((finFigureObjectLine *)obj);
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

finErrorCode finGraphPanelScene::drawObjLine(finFigureObjectLine *line)
{
    QPointF pt1 = this->_config.transformPixelPoint(line->getPoint1());
    QPointF pt2 = this->_config.transformPixelPoint(line->getPoint2());

    this->_scene->addLine(pt1.x(), pt1.y(), pt2.x(), pt2.y(), line->getFigureConfig()->getBorderPen());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelScene::drawObjLine3D(finFigureObjectLine3D *line3d)
{
    QPointF pt1 = this->_config.transformPixelPoint3D(line3d->getPoint1());
    QPointF pt2 = this->_config.transformPixelPoint3D(line3d->getPoint2());

    this->_scene->addLine(pt1.x(), pt1.y(), pt2.x(), pt2.y(), line3d->getFigureConfig()->getBorderPen());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}
/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finGraphPanelScene.h"

#include <QGraphicsView>
#include <QGraphicsItem>


finGraphPanelScene::finGraphPanelScene()
{
    this->_scene = nullptr;
}

QGraphicsScene *finGraphPanelScene::getScene() const
{
    return this->_scene;
}

finErrorCode finGraphPanelScene::setScene(QGraphicsScene *scene)
{
    this->_scene = scene;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finGraphPanelScene::draw()
{
    if ( this->_scene == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    return finGraphPanelBase::draw();
}

void finGraphPanelScene::clearScene() const
{
    if ( this->_scene == nullptr )
        return;

    QList<QGraphicsItem *> items = this->_scene->items();
    while ( !items.empty() ) {
        QGraphicsItem *item = items.first();
        items.removeFirst();
        this->_scene->removeItem(item);
        delete item;
    }
}

finErrorCode finGraphPanelScene::applyGraphConfig() const
{
    if ( this->_scene == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    this->clearScene();
    this->_scene->setBackgroundBrush(this->_config.getBackgroundBrush());
    this->_scene->setSceneRect(this->_config.getWholePanelPixelRect());

    QList<QGraphicsView *> views = this->_scene->views();
    for ( int i = 0; i < views.count(); i++ ) {
        QGraphicsView *view = views.at(i);
        view->setRenderHints(this->_config.getRenderHints());
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finGraphPanelScene::drawFigurePath(const finFigurePath &path) const
{
    if ( this->_scene == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    this->_scene->addPath(path.getPath(), path.getPen(), path.getBrush());

    if ( !path.getImage().isNull() ) {
        QGraphicsPixmapItem *item = this->_scene->addPixmap(path.getPixmap());
        item->setPos(path.getImagePosition());
    }
    return finErrorKits::EC_SUCCESS;
}

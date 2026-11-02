/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelScene.cpp
 *  \brief Implementation of the QGraphicsScene-based graph panel.
 *
 *  See finGraphPanelScene.h for the class documentation. This file stores the borrowed scene
 *  pointer, reports a missing scene as a state error, replaces the scene content on every draw, and
 *  turns the panel configuration and the queued paths into scene items and view settings.
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

    // Work on a copy of the item list because removeItem() mutates the scene; each removed item is
    // then deleted explicitly so that repeated draws do not accumulate stale items.
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

    // Render hints belong to the view rather than to the scene, so they are pushed to every view
    // currently displaying this scene.
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

    // A vector-only path carries a null pixmap, in which case no raster item is added.
    if ( !path.getImage().isNull() ) {
        QGraphicsPixmapItem *item = this->_scene->addPixmap(path.getPixmap());
        item->setPos(path.getImagePosition());
    }
    return finErrorKits::EC_SUCCESS;
}

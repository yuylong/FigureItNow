/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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

finErrorCode finGraphPanelPainter::draw()
{
    if ( this->_painter == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finGraphPanelBase::draw();
}

finErrorCode finGraphPanelPainter::applyGraphConfig() const
{
    this->_painter->setBackground(this->_config.getBackgroundBrush());
    this->_painter->eraseRect(this->_config.getWholePanelPixelRect());
    this->_painter->setRenderHints(this->_config.getRenderHints());

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::drawFigurePath(const finFigurePath &path) const
{
    this->_painter->setPen(path.getPen());
    this->_painter->setBrush(path.getBrush());
    this->_painter->drawPath(path.getPath());
    this->_painter->drawPixmap(path.getImagePosition(), path.getPixmap());
    return finErrorCodeKits::FIN_EC_SUCCESS;
}


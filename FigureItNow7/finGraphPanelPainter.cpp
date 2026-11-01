/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelPainter.cpp
 *  \brief Implementation of the QPainter-based graph panel.
 *
 *  See finGraphPanelPainter.h for the class documentation. This file stores the borrowed painter
 *  pointer, reports a missing painter as a state error, and turns the panel configuration and the
 *  queued paths into plain QPainter fill and draw operations.
 */

#include "finGraphPanelPainter.h"

#include <QSizeF>
#include <QBrush>
#include <QColor>


finGraphPanelPainter::finGraphPanelPainter()
{
    this->_painter = nullptr;
}

QPainter *finGraphPanelPainter::getPainter() const
{
    return this->_painter;
}

finErrorCode finGraphPanelPainter::setPainter(QPainter *painter)
{
    this->_painter = painter;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::draw()
{
    if ( this->_painter == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    return finGraphPanelBase::draw();
}

finErrorCode finGraphPanelPainter::applyGraphConfig() const
{
    // eraseRect() fills the panel rectangle with the brush installed by setBackground().
    this->_painter->setBackground(this->_config.getBackgroundBrush());
    this->_painter->eraseRect(this->_config.getWholePanelPixelRect());
    this->_painter->setRenderHints(this->_config.getRenderHints());

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finGraphPanelPainter::drawFigurePath(const finFigurePath &path) const
{
    this->_painter->setPen(path.getPen());
    this->_painter->setBrush(path.getBrush());
    // Both payloads are drawn unconditionally; an empty path or a null pixmap simply paints nothing.
    this->_painter->drawPath(path.getPath());
    this->_painter->drawPixmap(path.getImagePosition(), path.getPixmap());
    return finErrorKits::EC_SUCCESS;
}


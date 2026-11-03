/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finGraphPanelWidget.cpp
 *  \brief Implementation of the QPaintDevice-based graph panel.
 *
 *  See finGraphPanelWidget.h for the class documentation. This file stores the borrowed paint
 *  device, creates a stack-local QPainter around a single drawing pass, and guards the drawing
 *  hooks against a missing device by throwing.
 */

#include "finGraphPanelWidget.h"

#include "finGraphPanelPainter.h"


finGraphPanelWidget::finGraphPanelWidget()
{
    this->_widget = nullptr;
}

finErrorCode finGraphPanelWidget::setPainter(QPainter *painter)
{
    // The panel draws through a painter it creates itself in draw(), so the inherited attachment
    // API is rejected for every argument; both branches below therefore report the same code.
    if ( painter == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    return finErrorKits::EC_INVALID_PARAM;
}

QPaintDevice *finGraphPanelWidget::getWidget() const
{
    return this->_widget;
}

void finGraphPanelWidget::setWidget(QPaintDevice *widget)
{
    this->_widget = widget;
}

finErrorCode finGraphPanelWidget::draw()
{
    if ( this->_widget == nullptr )
        finThrow(finErrorKits::EC_STATE_ERROR, QString("Widget is not set."));

    QPainter painter(this->_widget);
    // The painter lives on this stack frame; bind it only for the delegated call and detach it
    // before it is destroyed so no dangling pointer survives the draw pass.
    this->_painter = &painter;
    finErrorCode errcode = finGraphPanelPainter::draw();
    this->_painter = nullptr;

    return errcode;
}

finErrorCode finGraphPanelWidget::applyGraphConfig() const
{
    if ( this->_widget == nullptr )
        finThrow(finErrorKits::EC_STATE_ERROR, QString("Widget is not set."));

    return finGraphPanelPainter::applyGraphConfig();
}

finErrorCode finGraphPanelWidget::drawFigurePath(const finFigurePath &path) const
{
    if ( this->_widget == nullptr )
        finThrow(finErrorKits::EC_STATE_ERROR, QString("Widget is not set."));

    return finGraphPanelPainter::drawFigurePath(path);
}

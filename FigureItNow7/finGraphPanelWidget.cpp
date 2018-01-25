/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finGraphPanelWidget.h"

#include "finGraphPanelPainter.h"


finGraphPanelWidget::finGraphPanelWidget()
{
    this->_widget = NULL;
}

finErrorCode finGraphPanelWidget::setPainter(QPainter *painter)
{
    if ( painter == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    return finErrorKits::EC_INVALID_PARAM;
}

QPaintDevice *finGraphPanelWidget::getWidget() const
{
    return this->_widget;
}

finErrorCode finGraphPanelWidget::setWidget(QPaintDevice *widget)
{
    this->_widget = widget;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finGraphPanelWidget::draw()
{
    if ( this->_widget == NULL )
        return finErrorKits::EC_STATE_ERROR;

    QPainter painter(this->_widget);
    this->_painter = &painter;
    finErrorCode errcode = finGraphPanelPainter::draw();
    this->_painter = NULL;

    return errcode;
}

finErrorCode finGraphPanelWidget::applyGraphConfig() const
{
    if ( this->_widget == NULL )
        return finErrorKits::EC_STATE_ERROR;

    return finGraphPanelPainter::applyGraphConfig();
}

finErrorCode finGraphPanelWidget::drawFigurePath(const finFigurePath &path) const
{
    if ( this->_widget == NULL )
        return finErrorKits::EC_STATE_ERROR;

    return finGraphPanelPainter::drawFigurePath(path);
}

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161231    0  Yulong Yu    Create this file.
 */

#include "finFigureConfig.h"

finFigureConfig *finFigureConfig::_defFigCfg = NULL;

finFigureConfig::finFigureConfig()
    : _borderPen(), _fillBrush(Qt::transparent), _font(QString("Arial"), 12)
{
    /* Do Nothing. */
}

QPen finFigureConfig::getBorderPen() const
{
    return this->_borderPen;
}

QBrush finFigureConfig::getFillBrush() const
{
    return this->_fillBrush;
}

QFont finFigureConfig::getFont() const
{
    return this->_font;
}

double finFigureConfig::getDotSize() const
{
    return this->_borderPen.width();
}

QColor finFigureConfig::getBorderColor() const
{
    return this->_borderPen.color();
}

QColor finFigureConfig::getFillColor() const
{
    return this->_fillBrush.color();
}

finErrorCode finFigureConfig::setBorderPen(const QPen &pen)
{
    this->_borderPen = pen;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillBrush(const QBrush &brush)
{
    this->_fillBrush = brush;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFont(const QFont &font)
{
    this->_font = font;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSize(double size)
{
    if ( size <= 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_borderPen.setWidthF(size);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColor(const QColor &color)
{
    this->_borderPen.setColor(color);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColor(const QColor &color)
{
    this->_fillBrush.setColor(color);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFigureConfig(finFigureConfig *outcfg) const
{
    if ( outcfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    outcfg->_borderPen = this->_borderPen;
    outcfg->_fillBrush = this->_fillBrush;
    outcfg->_font = this->_font;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finFigureConfig *finFigureConfig::getDefaultFigureConfig()
{
    if ( finFigureConfig::_defFigCfg == NULL )
        finFigureConfig::_defFigCfg = new finFigureConfig();

    return finFigureConfig::_defFigCfg;
}

finErrorCode finFigureConfig::releaseDefaultFigureConfig()
{
    if ( finFigureConfig::_defFigCfg == NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFromDefaultFigureConfig(finFigureConfig *outfig)
{
    return finFigureConfig::getDefaultFigureConfig()->cloneFigureConfig(outfig);
}

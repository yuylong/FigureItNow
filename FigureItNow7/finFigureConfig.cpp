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
    : _borderPen(Qt::black, 1), _fillBrush(Qt::transparent), _startArrow(), _endArrow(),
      _textPen(Qt::transparent, 0), _textBrush(Qt::black), _font(QString("Arial"), 12)
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

finFigureArrow finFigureConfig::getStartArrow() const
{
    return this->_startArrow;
}

finFigureArrow finFigureConfig::getEndArrow() const
{
    return this->_endArrow;
}

QPen finFigureConfig::getTextPen() const
{
    return this->_textPen;
}

QBrush finFigureConfig::getTextBrush() const
{
    return this->_textBrush;
}

QFont finFigureConfig::getFont() const
{
    return this->_font;
}

QMarginsF finFigureConfig::getTextMargins() const
{
    return this->_textMargins;
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

finFigureArrowType finFigureConfig::getStartArrowType() const
{
    return this->_startArrow.getType();
}

finFigureArrowType finFigureConfig::getEndArrowType() const
{
    return this->_endArrow.getType();
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

finErrorCode finFigureConfig::setStartArrow(const finFigureArrow &arrow)
{
    this->_startArrow = arrow;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrow(const finFigureArrow &arrow)
{
    this->_endArrow = arrow;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextPen(const QPen &pen)
{
    this->_textPen = pen;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextBrush(const QBrush &brush)
{
    this->_textBrush = brush;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFont(const QFont &font)
{
    this->_font = font;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextMargins(const QMarginsF &margins)
{
    this->_textMargins = margins;
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

finErrorCode finFigureConfig::setStartArrowType(finFigureArrowType type)
{
    this->_startArrow.setType(type);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrowType(finFigureArrowType type)
{
    this->_endArrow.setType(type);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFigureConfig(finFigureConfig *outcfg) const
{
    if ( outcfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    outcfg->_borderPen = this->_borderPen;
    outcfg->_fillBrush = this->_fillBrush;
    outcfg->_startArrow = this->_startArrow;
    outcfg->_endArrow = this->_endArrow;
    outcfg->_textPen = this->_textPen;
    outcfg->_textBrush = this->_textBrush;
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

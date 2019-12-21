/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finFigureConfig.h"


finFigureConfig *finFigureConfig::_defFigCfg = nullptr;

finFigureConfig::finFigureConfig()
    : _borderPen(Qt::black, 1), _fillBrush(Qt::transparent), _startArrow(), _endArrow(),
      _textPen(Qt::transparent, 0), _textBrush(Qt::black), _font(QString("Arial"), 12)
{
    /* Do Nothing. */
}

const QPen &finFigureConfig::getBorderPen() const
{
    return this->_borderPen;
}

const QBrush &finFigureConfig::getFillBrush() const
{
    return this->_fillBrush;
}

const finFigureArrow &finFigureConfig::getStartArrow() const
{
    return this->_startArrow;
}

const finFigureArrow &finFigureConfig::getEndArrow() const
{
    return this->_endArrow;
}

const QPen &finFigureConfig::getTextPen() const
{
    return this->_textPen;
}

const QBrush &finFigureConfig::getTextBrush() const
{
    return this->_textBrush;
}

const QFont &finFigureConfig::getFont() const
{
    return this->_font;
}

const QMarginsF &finFigureConfig::getTextMargins() const
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

double finFigureConfig::getStartArrowSize() const
{
    return this->_startArrow.getLength();
}

double finFigureConfig::getStartArrowRadian() const
{
    return this->_startArrow.getRadian();
}

finFigureArrowType finFigureConfig::getEndArrowType() const
{
    return this->_endArrow.getType();
}

double finFigureConfig::getEndArrowSize() const
{
    return this->_endArrow.getLength();
}

double finFigureConfig::getEndArrowRadian() const
{
    return this->_endArrow.getRadian();
}

QString finFigureConfig::getFontName() const
{
    return this->_font.family();
}

double finFigureConfig::getFontPointSize() const
{
    return this->_font.pointSizeF();
}

bool finFigureConfig::getFontBold() const
{
    return this->_font.bold();
}

bool finFigureConfig::getFontItalic() const
{
    return this->_font.italic();
}

QColor finFigureConfig::getFontColor() const
{
    return this->_textBrush.color();
}

finErrorCode finFigureConfig::setBorderPen(const QPen &pen)
{
    this->_borderPen = pen;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillBrush(const QBrush &brush)
{
    this->_fillBrush = brush;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setStartArrow(const finFigureArrow &arrow)
{
    this->_startArrow = arrow;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrow(const finFigureArrow &arrow)
{
    this->_endArrow = arrow;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextPen(const QPen &pen)
{
    this->_textPen = pen;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextBrush(const QBrush &brush)
{
    this->_textBrush = brush;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFont(const QFont &font)
{
    this->_font = font;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setTextMargins(const QMarginsF &margins)
{
    this->_textMargins = margins;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSize(double size)
{
    if ( size <= 0.0 )
        return finErrorKits::EC_INVALID_PARAM;

    this->_borderPen.setWidthF(size);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColor(const QColor &color)
{
    this->_borderPen.setColor(color);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColor(const QColor &color)
{
    this->_fillBrush.setColor(color);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setStartArrowType(finFigureArrowType type)
{
    this->_startArrow.setType(type);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setStartArrowSize(double size)
{
    this->_startArrow.setLength(size);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setStartArrowRadian(double rad)
{
    this->_endArrow.setRadian(rad);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrowType(finFigureArrowType type)
{
    this->_endArrow.setType(type);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrowSize(double size)
{
    this->_endArrow.setLength(size);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndArrowRadian(double rad)
{
    this->_endArrow.setRadian(rad);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontName(const QString &ftname)
{
    this->_font.setFamily(ftname);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontPointSize(double size)
{
    this->_font.setPointSizeF(size);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontBold(bool bold)
{
    this->_font.setBold(bold);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontItalic(bool italic)
{
    this->_font.setItalic(italic);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontColor(const QColor &color)
{
    this->_textBrush.setColor(color);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFigureConfig(finFigureConfig *outcfg) const
{
    if ( outcfg == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    outcfg->_borderPen = this->_borderPen;
    outcfg->_fillBrush = this->_fillBrush;
    outcfg->_startArrow = this->_startArrow;
    outcfg->_endArrow = this->_endArrow;
    outcfg->_textPen = this->_textPen;
    outcfg->_textBrush = this->_textBrush;
    outcfg->_font = this->_font;
    return finErrorKits::EC_SUCCESS;
}

finFigureConfig *finFigureConfig::getDefaultFigureConfig()
{
    if ( finFigureConfig::_defFigCfg == nullptr )
        finFigureConfig::_defFigCfg = new finFigureConfig();

    return finFigureConfig::_defFigCfg;
}

finErrorCode finFigureConfig::releaseDefaultFigureConfig()
{
    if ( finFigureConfig::_defFigCfg == nullptr )
        return finErrorKits::EC_DUPLICATE_OP;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFromDefaultFigureConfig(finFigureConfig *outfig)
{
    return finFigureConfig::getDefaultFigureConfig()->cloneFigureConfig(outfig);
}

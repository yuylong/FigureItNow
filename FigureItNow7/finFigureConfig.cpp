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

void finFigureConfig::setBorderPen(const QPen &pen)
{
    this->_borderPen = pen;
}

void finFigureConfig::setFillBrush(const QBrush &brush)
{
    this->_fillBrush = brush;
}

void finFigureConfig::setStartArrow(const finFigureArrow &arrow)
{
    this->_startArrow = arrow;
}

void finFigureConfig::setEndArrow(const finFigureArrow &arrow)
{
    this->_endArrow = arrow;
}

void finFigureConfig::setTextPen(const QPen &pen)
{
    this->_textPen = pen;
}

void finFigureConfig::setTextBrush(const QBrush &brush)
{
    this->_textBrush = brush;
}

void finFigureConfig::setFont(const QFont &font)
{
    this->_font = font;
}

void finFigureConfig::setTextMargins(const QMarginsF &margins)
{
    this->_textMargins = margins;
}

void finFigureConfig::setDotSize(double size)
{
    if ( size <= 0.0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Dot size must be positive.");

    this->_borderPen.setWidthF(size);
}

void finFigureConfig::setBorderColor(const QColor &color)
{
    this->_borderPen.setColor(color);
}

void finFigureConfig::setFillColor(const QColor &color)
{
    this->_fillBrush.setColor(color);
}

void finFigureConfig::setStartArrowType(finFigureArrowType type)
{
    this->_startArrow.setType(type);
}

void finFigureConfig::setStartArrowSize(double size)
{
    this->_startArrow.setLength(size);
}

void finFigureConfig::setStartArrowRadian(double rad)
{
    this->_endArrow.setRadian(rad);
}

void finFigureConfig::setEndArrowType(finFigureArrowType type)
{
    this->_endArrow.setType(type);
}

void finFigureConfig::setEndArrowSize(double size)
{
    this->_endArrow.setLength(size);
}

void finFigureConfig::setEndArrowRadian(double rad)
{
    this->_endArrow.setRadian(rad);
}

void finFigureConfig::setFontName(const QString &ftname)
{
    this->_font.setFamily(ftname);
}

void finFigureConfig::setFontPointSize(double size)
{
    this->_font.setPointSizeF(size);
}

void finFigureConfig::setFontBold(bool bold)
{
    this->_font.setBold(bold);
}

void finFigureConfig::setFontItalic(bool italic)
{
    this->_font.setItalic(italic);
}

void finFigureConfig::setFontColor(const QColor &color)
{
    this->_textBrush.setColor(color);
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

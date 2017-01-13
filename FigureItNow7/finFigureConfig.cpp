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
{
    this->_dotSize = QSizeF(1.0, 1.0);
    this->_endShape = finFigureConfig::FIN_ES_ROUND;

    this->_borderColor = QColor(Qt::black);
    this->_fillColor = QColor(Qt::transparent);
    this->_linePattern = 0xFFFFFFFF;

    this->_font = QFont(QString("Arial"), 12);
}

QSizeF finFigureConfig::getDotSize() const
{
    return this->_dotSize;
}

double finFigureConfig::getDotSizeX() const
{
    return this->_dotSize.width();
}

double finFigureConfig::getDotSizeY() const
{
    return this->_dotSize.height();
}

finFigureEndShape finFigureConfig::getEndShape() const
{
    return this->_endShape;
}

QColor finFigureConfig::getBorderColor() const
{
    return this->_borderColor;
}

QColor finFigureConfig::getFillColor() const
{
    return this->_fillColor;
}

quint32 finFigureConfig::getLinePattern() const
{
    return this->_linePattern;
}

QPen finFigureConfig::getBorderPen() const
{
    QPen retpen;
    retpen.setColor(this->_borderColor);
    retpen.setStyle(Qt::SolidLine);
    retpen.setWidthF(this->_dotSize.width());

    return retpen;
}

QBrush finFigureConfig::getFillBrush() const
{
    QBrush retbrush;
    retbrush.setColor(this->_fillColor);
    retbrush.setStyle(Qt::SolidPattern);
    return retbrush;
}

QFont finFigureConfig::getFont() const
{
    return this->_font;
}

finErrorCode finFigureConfig::setDotSize(const QSizeF &size)
{
    this->_dotSize = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSizeX(double sizex)
{
    if ( sizex < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSize.setWidth(sizex);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSizeY(double sizey)
{
    if ( sizey < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSize.setHeight(sizey);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndShape(finFigureEndShape endshape)
{
    this->_endShape = endshape;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColor(const QColor &brcolor)
{
    this->_borderColor = brcolor;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColor(const QColor &filcolor)
{
    this->_fillColor = filcolor;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setLinePattern(quint32 lnpat)
{
    this->_linePattern = lnpat;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFont(const QFont &font)
{
    this->_font = font;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFigureConfig(finFigureConfig *outcfg) const
{
    if ( outcfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    outcfg->setDotSize(this->_dotSize);
    outcfg->setEndShape(this->_endShape);
    outcfg->setBorderColor(this->_borderColor);
    outcfg->setFillColor(this->_fillColor);
    outcfg->setLinePattern(this->_linePattern);
    outcfg->setFont(this->_font);
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

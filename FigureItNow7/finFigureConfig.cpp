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
    this->_dotSizeX = 1.0;
    this->_dotSizeY = 1.0;
    this->_endShape = finFigureConfig::FIN_ES_ROUND;

    this->_borderColor[finFigureConfig::FIN_CI_RED] = 0.0;
    this->_borderColor[finFigureConfig::FIN_CI_GREEN] = 0.0;
    this->_borderColor[finFigureConfig::FIN_CI_BLUE] = 0.0;
    this->_borderColor[finFigureConfig::FIN_CI_TRANSPARENCY] = 0.0;
    this->_fillColor[finFigureConfig::FIN_CI_RED] = 1.0;
    this->_fillColor[finFigureConfig::FIN_CI_GREEN] = 1.0;
    this->_fillColor[finFigureConfig::FIN_CI_RED] = 1.0;
    this->_fillColor[finFigureConfig::FIN_CI_GREEN] = 1.0;
    this->_linePattern = 0xFFFFFFFF;

    this->_fontName = QString("Arial");
    this->_fontSize = 12.0;
}

double finFigureConfig::getDotSizeX() const
{
    return this->_dotSizeX;
}

double finFigureConfig::getDotSizeY() const
{
    return this->_dotSizeY;
}

finFigureEndShape finFigureConfig::getEndShape() const
{
    return this->_endShape;
}

bool finFigureConfig::isBorderTransparent() const
{
    return (this->_borderColor[finFigureConfig::FIN_CI_TRANSPARENCY] >= 1.0);
}

bool finFigureConfig::isFillTransparent() const
{
    return (this->_fillColor[finFigureConfig::FIN_CI_TRANSPARENCY] >= 1.0);
}

double finFigureConfig::getBorderColorSingle(finFigureColorIdx coloridx) const
{
    if ( coloridx >= finFigureConfig::FIN_CI_MAX )
        return -1.0;

    return this->_borderColor[coloridx];
}

double finFigureConfig::getFillColorSingle(finFigureColorIdx coloridx) const
{
    if ( coloridx >= finFigureConfig::FIN_CI_MAX )
            return -1.0;

    return this->_fillColor[coloridx];
}

quint32 finFigureConfig::getLinePattern() const
{
    return this->_linePattern;
}

QString finFigureConfig::getFontName() const
{
    return this->_fontName;
}

double finFigureConfig::getFontSize() const
{
    return this->_fontSize;
}

finErrorCode finFigureConfig::setDotSize(double size)
{
    if ( size < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSizeX = size;
    this->_dotSizeY = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSize(double sizex, double sizey)
{
    if ( sizex < 0.0 || sizey < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSizeX = sizex;
    this->_dotSizeY = sizey;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSizeX(double sizex)
{
    if ( sizex < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSizeX = sizex;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setDotSizeY(double sizey)
{
    if ( sizey < 0.0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_dotSizeY = sizey;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setEndShape(finFigureEndShape endshape)
{
    this->_endShape = endshape;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColor(double bdrcolor[])
{
    for ( int i = 0; i < finFigureConfig::FIN_CI_MAX; i++ ) {
        this->_borderColor[i] = bdrcolor[i];
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColor(double red, double green, double blue, double transparent)
{
    this->_borderColor[finFigureConfig::FIN_CI_RED] = red;
    this->_borderColor[finFigureConfig::FIN_CI_GREEN] = green;
    this->_borderColor[finFigureConfig::FIN_CI_BLUE] = blue;
    this->_borderColor[finFigureConfig::FIN_CI_TRANSPARENCY] = transparent;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setBorderColorSingle(double colorval, finFigureColorIdx coloridx)
{
    if ( coloridx >= finFigureConfig::FIN_CI_MAX )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_borderColor[coloridx] = colorval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColor(double filcolor[])
{
    for ( int i = 0; i < finFigureConfig::FIN_CI_MAX; i++ ) {
        this->_fillColor[i] = filcolor[i];
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColor(double red, double green, double blue, double transparent)
{
    this->_fillColor[finFigureConfig::FIN_CI_RED] = red;
    this->_fillColor[finFigureConfig::FIN_CI_GREEN] = green;
    this->_fillColor[finFigureConfig::FIN_CI_BLUE] = blue;
    this->_fillColor[finFigureConfig::FIN_CI_TRANSPARENCY] = transparent;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFillColorSingle(double colorval, finFigureColorIdx coloridx)
{
    if ( coloridx >= finFigureConfig::FIN_CI_MAX )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_fillColor[coloridx] = colorval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setLinePattern(quint32 lnpat)
{
    this->_linePattern = lnpat;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontName(const QString &fontname)
{
    this->_fontName = fontname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::setFontSize(double fontsize)
{
    this->_fontSize = fontsize;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureConfig::cloneFigureConfig(finFigureConfig *outcfg) const
{
    if ( outcfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    outcfg->setDotSize(this->_dotSizeX, this->_dotSizeY);
    outcfg->setEndShape(this->_endShape);
    outcfg->setBorderColor((double *)this->_borderColor);
    outcfg->setFillColor((double *)this->_fillColor);
    outcfg->setLinePattern(this->_linePattern);
    outcfg->setFontName(this->_fontName);
    outcfg->setFontSize(this->_fontSize);
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

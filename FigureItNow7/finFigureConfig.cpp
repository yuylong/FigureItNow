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

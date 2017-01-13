/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170102    0  Yulong Yu    Create this file.
 */

#include "finGraphConfig.h"

#include <qmath.h>

finGraphConfig::finGraphConfig()
{
    this->_unitPixelSize = 72.0;
    this->_panelSize = QSizeF(640.0 / this->_unitPixelSize, 480.0 / this->_unitPixelSize);
    this->_bgColor = QColor(Qt::white);

    this->_originPoint = QPointF(320.0 / this->_unitPixelSize, 240.0 / this->_unitPixelSize);
    this->_axisUnitSize = 20.0;

    this->_enableAxisZ = false;
    this->_axisRadZ = (-3/4) * M_PI;
}

double finGraphConfig::getUnitPixelSize() const
{
    return this->_unitPixelSize;
}

QSizeF finGraphConfig::getPanelSize() const
{
    return this->_panelSize;
}

QSizeF finGraphConfig::getPanelPixelSize() const
{
    QSizeF retsize;
    retsize.setWidth(this->_panelSize.width() * this->_unitPixelSize);
    retsize.setHeight(this->_panelSize.height() * this->_unitPixelSize);
    return retsize;
}

QRectF finGraphConfig::getWholePanelRect() const
{
    return QRectF(QPointF(0.0, 0.0), this->getPanelSize());
}

QRectF finGraphConfig::getWholePanelPixelRect() const
{
    return QRectF(QPointF(0.0, 0.0), this->getPanelPixelSize());
}

QColor finGraphConfig::getBackgroundColor() const
{
    return this->_bgColor;
}

QBrush finGraphConfig::getBackgroundBrush() const
{
    return QBrush(this->_bgColor, Qt::SolidPattern);
}

QPointF finGraphConfig::getOriginPoint() const
{
    return this->_originPoint;
}

double finGraphConfig::getAxisUnitSize() const
{
    return this->_axisUnitSize;
}

bool finGraphConfig::isEnabledAxisZ() const
{
    return this->_enableAxisZ;
}

double finGraphConfig::getAxisRadZ() const
{
    return this->_axisRadZ;
}

finGraphTrans *finGraphConfig::getTransform() const
{
    return this->_transform;
}

finErrorCode finGraphConfig::setUnitPixelSize(double size)
{
    this->_unitPixelSize = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setPanelSize(const QSizeF &size)
{
    this->_panelSize = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setPanelSize(double sizex, double sizey)
{
    this->_panelSize.setWidth(sizex);
    this->_panelSize.setHeight(sizey);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setBackgroundColor(const QColor &color)
{
    this->_bgColor = color;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

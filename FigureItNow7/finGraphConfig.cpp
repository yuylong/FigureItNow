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
    this->_axisUnitSize = 40.0 / this->_unitPixelSize;

    this->_enableAxisZ = false;
    this->_axisRadZ = (-3.0/4.0) * M_PI;
    this->_axisScaleZ = 0.618;

    this->_transform = NULL;

    this->_renderHints = (QPainter::Antialiasing | QPainter::TextAntialiasing);
}

finErrorCode finGraphConfig::copyGraphConfig(const finGraphConfig *srccfg)
{
    this->_unitPixelSize = srccfg->_unitPixelSize;
    this->_panelSize = srccfg->_panelSize;
    this->_bgColor = srccfg->_bgColor;

    this->_originPoint = srccfg->_originPoint;
    this->_axisUnitSize = srccfg->_axisUnitSize;

    this->_enableAxisZ = srccfg->_enableAxisZ;
    this->_axisRadZ = srccfg->_axisRadZ;
    this->_axisScaleZ = srccfg->_axisScaleZ;

    //this->_transform = NULL;

    this->_renderHints = srccfg->_renderHints;
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

QPointF finGraphConfig::getOriginPixelPoint() const
{
    return this->_originPoint * this->_unitPixelSize;
}

double finGraphConfig::getAxisUnitSize() const
{
    return this->_axisUnitSize;
}

double finGraphConfig::getAxisUnitPixelSize() const
{
    return this->_axisUnitSize * this->_unitPixelSize;
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

QPainter::RenderHints finGraphConfig::getRenderHints() const
{
    return this->_renderHints;
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

QPointF finGraphConfig::transformPoint3D(double x, double y, double z)
{
    QPointF srcpt(x, y);
    QPointF zshift(z * this->_axisScaleZ * cos(this->_axisRadZ), z * this->_axisScaleZ * sin(this->_axisRadZ));

    return this->transformPoint(srcpt + zshift);
}

QPointF finGraphConfig::transformPoint3D(const finFigurePoint3D &pt)
{
    return this->transformPoint3D(pt.getX(), pt.getY(), pt.getZ());
}

QPointF finGraphConfig::transformPixelPoint3D(double x, double y, double z)
{
    return this->transformPoint3D(x, y, z) * this->_unitPixelSize;
}

QPointF finGraphConfig::transformPixelPoint3D(const finFigurePoint3D &pt)
{
    return this->transformPixelPoint3D(pt.getX(), pt.getY(), pt.getZ());
}

QPointF finGraphConfig::transformPoint(const QPointF &srcpt)
{
    QPointF midpt;
    if ( this->_transform != NULL && this->_transform->getTransformType() != finGraphTrans::FIN_GT_TYPE_NONE )
        midpt = this->_transform->transPoint(srcpt);
    else
        midpt = srcpt;
    midpt.setY(-midpt.y());

    return midpt * this->_axisUnitSize + this->_originPoint;
}

QPointF finGraphConfig::transformPixelPoint(const QPointF &srcpt)
{
    return this->transformPoint(srcpt) * this->_unitPixelSize;
}

QPointF finGraphConfig::arcTransformPoint(const QPointF &srcpt)
{
    QPointF midpt = srcpt;
    midpt = (midpt - this->_originPoint) / this->_axisUnitSize;
    midpt.setY(-midpt.y());

    if ( this->_transform != NULL && this->_transform->getTransformType() != finGraphTrans::FIN_GT_TYPE_NONE )
        return this->_transform->arcTransPoint(midpt);
    else
        return midpt;
}

QPointF finGraphConfig::arcTransformPixelPoint(const QPointF &srcpt)
{
    return this->arcTransformPoint(srcpt / this->_unitPixelSize);
}


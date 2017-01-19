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
    this->_panelSize = QSizeF(640.0, 480.0);
    this->_bgColor = QColor(Qt::white);

    this->_originPoint = QPointF(320.0, 240.0);
    this->_axisUnitSize = 40.0;

    this->_axisRadZ = (-3.0/4.0) * M_PI;
    this->_axisScaleZ = 0.618;

    this->_transform = NULL;

    this->_renderHints = (QPainter::Antialiasing | QPainter::TextAntialiasing);
}

finErrorCode finGraphConfig::copyGraphConfig(const finGraphConfig *srccfg)
{
    this->_panelSize = srccfg->_panelSize;
    this->_bgColor = srccfg->_bgColor;

    this->_originPoint = srccfg->_originPoint;
    this->_axisUnitSize = srccfg->_axisUnitSize;

    this->_axisRadZ = srccfg->_axisRadZ;
    this->_axisScaleZ = srccfg->_axisScaleZ;

    //this->_transform = NULL;

    this->_renderHints = srccfg->_renderHints;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QSizeF finGraphConfig::getPanelPixelSize() const
{
    return this->_panelSize;
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

QPointF finGraphConfig::getOriginPixelPoint() const
{
    return this->_originPoint;
}

double finGraphConfig::getAxisUnitPixelSize() const
{
    return this->_axisUnitSize;
}

double finGraphConfig::getAxisRadZ() const
{
    return this->_axisRadZ;
}

double finGraphConfig::getAxisScaleZ() const
{
    return this->_axisScaleZ;
}

finGraphTrans *finGraphConfig::getTransform() const
{
    return this->_transform;
}

bool finGraphConfig::isLinearTransform() const
{
    if ( this->_transform != NULL && this->_transform->getTransformType() != finGraphTrans::FIN_GT_TYPE_NONE )
        return true;
    else
        return this->_transform->isLinear();
}

QPainter::RenderHints finGraphConfig::getRenderHints() const
{
    return this->_renderHints;
}

finErrorCode finGraphConfig::setPanelPixelSize(const QSizeF &size)
{
    this->_panelSize = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setPanelPixelSize(double sizex, double sizey)
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

finErrorCode finGraphConfig::setOriginPixelPoint(const QPointF &pt)
{
    this->_originPoint = pt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setOriginPixelPoint(double ptx, double pty)
{
    this->_originPoint.setX(ptx);
    this->_originPoint.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setAxisUnitPixelSize(double size)
{
    this->_axisUnitSize = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setAxisRadZ(double rad)
{
    this->_axisRadZ = rad;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setAxisScaleZ(double scale)
{
    this->_axisScaleZ = scale;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finGraphConfig::setRenderHints(QPainter::RenderHints hints)
{
    this->_renderHints = hints;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QPointF finGraphConfig::transformPixelPoint3D(double x, double y, double z)
{
    QPointF srcpt(x, y);
    QPointF zshift(z * this->_axisScaleZ * cos(this->_axisRadZ), z * this->_axisScaleZ * sin(this->_axisRadZ));

    return this->transformPixelPoint(srcpt + zshift);
}

QPointF finGraphConfig::transformPixelPoint3D(const finFigurePoint3D &pt)
{
    return this->transformPixelPoint3D(pt.getX(), pt.getY(), pt.getZ());
}

QPointF finGraphConfig::transformPixelPoint(const QPointF &srcpt)
{
    QPointF midpt;
    if ( this->_transform != NULL && this->_transform->getTransformType() != finGraphTrans::FIN_GT_TYPE_NONE )
        midpt = this->_transform->transPoint(srcpt);
    else
        midpt = srcpt;
    midpt.setY(-midpt.y());

    return midpt * this->_axisUnitSize + this->_originPoint;
}

QPointF finGraphConfig::arcTransformPixelPoint(const QPointF &srcpt)
{
    QPointF midpt = srcpt;
    midpt = (midpt - this->_originPoint) / this->_axisUnitSize;
    midpt.setY(-midpt.y());

    if ( this->_transform != NULL && this->_transform->getTransformType() != finGraphTrans::FIN_GT_TYPE_NONE )
        return this->_transform->arcTransPoint(midpt);
    else
        return midpt;
}


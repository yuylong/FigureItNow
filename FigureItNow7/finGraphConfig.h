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

#ifndef FINGRAPHCONFIG_H
#define FINGRAPHCONFIG_H

#include <QSizeF>
#include <QPointF>
#include <QColor>
#include <QRect>
#include <QBrush>
#include <QTransform>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finGraphTrans.h"

class finGraphConfig
{
protected:
    double _unitPixelSize;
    QSizeF _panelSize;
    QColor _bgColor;

    QPointF _originPoint;
    double _axisUnitSize;
    bool _enableAxisZ;
    double _axisRadZ;

    finGraphTrans *_transform;

public:
    finGraphConfig();

    double getUnitPixelSize() const;
    QSizeF getPanelSize() const;
    QSizeF getPanelPixelSize() const;
    QRectF getWholePanelRect() const;
    QRectF getWholePanelPixelRect() const;

    QColor getBackgroundColor() const;
    QBrush getBackgroundBrush() const;

    QPointF getOriginPoint() const;
    QPointF getOriginPixelPoint() const;
    double getAxisUnitSize() const;
    double getAxisUnitPixelSize() const;
    bool isEnabledAxisZ() const;
    double getAxisRadZ() const;

    finGraphTrans *getTransform() const;

    finErrorCode setUnitPixelSize(double size);
    finErrorCode setPanelSize(const QSizeF &size);
    finErrorCode setPanelSize(double sizex, double sizey);
    finErrorCode setBackgroundColor(const QColor &color);

    QPointF transformPoint3D(double x, double y, double z);
    QPointF transformPixelPoint3D(double x, double y, double z);
    QPointF transformPoint(const QPointF &srcpt);
    QPointF transfromPixelPoint(const QPointF &srcpt);
    QPointF arcTransformPoint(const QPointF &srcpt);
    QPointF arcTransformPixelPoint(const QPointF &srcpt);
};

#endif // FINGRAPHCONFIG_H

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
#include <QPainter>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finFigurePoint3D.h"
#include "finGraphTrans.h"

class finGraphConfig
{
protected:
    QSizeF _panelSize;
    QColor _bgColor;

    QPointF _originPoint;
    double _axisUnitSize;
    double _axisRadZ;
    double _axisScaleZ;
    finGraphTrans *_transform;

    QPainter::RenderHints _renderHints;

public:
    finGraphConfig();
    ~finGraphConfig();

    finErrorCode copyGraphConfig(const finGraphConfig *srccfg);

    const QSizeF &getPanelPixelSize() const;
    double getPanelPixelWidth() const;
    double getPanelPixelHeight() const;
    QRectF getWholePanelPixelRect() const;
    QList<QPointF> getCornerAxisPoints() const;
    const QColor &getBackgroundColor() const;
    QBrush getBackgroundBrush() const;

    QPointF getOriginPixelPoint() const;
    double getAxisUnitPixelSize() const;
    double getAxisRadZ() const;
    double getAxisScaleZ() const;

    bool isOriginPointOnPanel() const;

    finGraphTransType getTransformType() const;
    finGraphTrans *getTransform() const;
    bool isLinearTransform() const;

    QPainter::RenderHints getRenderHints() const;

    finErrorCode setPanelPixelSize(const QSizeF &size);
    finErrorCode setPanelPixelWidth(double width);
    finErrorCode setPanelPixelHeight(double height);
    finErrorCode setPanelPixelSize(double sizex, double sizey);
    finErrorCode setBackgroundColor(const QColor &color);

    finErrorCode setOriginPixelPoint(const QPointF &pt);
    finErrorCode setOriginPixelPoint(double ptx, double pty);
    finErrorCode setAxisUnitPixelSize(double size);
    finErrorCode setAxisRadZ(double rad);
    finErrorCode setAxisScaleZ(double scale);

    finErrorCode setTransformType(finGraphTransType type);
    finErrorCode cloneTransform(const finGraphTrans *srctrans);
    finErrorCode cloneTransform(const finGraphConfig *srccfg);
    QTransform getNakePixelTransformMatrix() const;

    finErrorCode setRenderHints(QPainter::RenderHints hints);

    QPointF transformPixelPoint3D(double x, double y, double z) const;
    QPointF transformPixelPoint3D(const finFigurePoint3D &pt) const;

    QPointF transformPixelPoint(const QPointF &srcpt) const;
    QPointF arcTransformPixelPoint(const QPointF &srcpt) const;
    QPainterPath transformPixelPath(const QPainterPath &path) const;
    QPainterPath arcTransformPixelPath(const QPainterPath &path) const;
    QPolygonF transformPixelPolygon(const QPolygonF &polygon) const;
    QPolygonF arcTransformPixelPolygon(const QPolygonF &polygon) const;
};

#endif // FINGRAPHCONFIG_H

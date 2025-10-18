/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
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

    void copyGraphConfig(const finGraphConfig *srccfg);

    const QSizeF &getPanelPixelSize() const;
    double getPanelPixelWidth() const;
    double getPanelPixelHeight() const;
    QRectF getWholePanelPixelRect() const;
    QList<QPointF> getCornerAxisPoints() const;
    const QColor &getBackgroundColor() const;
    QBrush getBackgroundBrush() const;

    const QPointF &getOriginPixelPoint() const;
    double getOriginPixelPointX() const;
    double getOriginPixelPointY() const;
    double getAxisUnitPixelSize() const;
    double getAxisRadZ() const;
    double getAxisScaleZ() const;

    bool isOriginPointOnPanel() const;

    finGraphTransType getTransformType() const;
    finGraphTrans *getTransform() const;
    bool isLinearTransform() const;

    QPainter::RenderHints getRenderHints() const;

    void setPanelPixelSize(const QSizeF &size);
    void setPanelPixelWidth(double width);
    void setPanelPixelHeight(double height);
    void setPanelPixelSize(double sizex, double sizey);
    void setBackgroundColor(const QColor &color);

    void setOriginPixelPoint(const QPointF &pt);
    void setOriginPixelPointX(double ptx);
    void setOriginPixelPointY(double pty);
    void setOriginPixelPoint(double ptx, double pty);
    void setAxisUnitPixelSize(double size);
    void setAxisRadZ(double rad);
    void setAxisScaleZ(double scale);

    void setTransformType(finGraphTransType type);
    void cloneTransform(const finGraphTrans *srctrans);
    void cloneTransform(const finGraphConfig *srccfg);
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

    static QString getRenderHintsName(const QPainter::RenderHints &hints);
    static QPainter::RenderHints parseRenderHints(const QString &name);
};

#endif // FINGRAPHCONFIG_H

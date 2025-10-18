/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finGraphConfig.h"

#include <qmath.h>


finGraphConfig::finGraphConfig()
{
    this->_panelSize = QSizeF(640.0, 480.0);
    this->_bgColor = QColor(Qt::white);

    this->_originPoint = QPointF(320.0, 240.0);
    this->_axisUnitSize = 40.0;
    this->_axisRadZ = (-3.0 / 4.0) * M_PI;
    this->_axisScaleZ = 0.618;

    this->_transform = nullptr;

    this->_renderHints = QPainter::Antialiasing | QPainter::SmoothPixmapTransform;
}

finGraphConfig::~finGraphConfig()
{
    if ( this->_transform != nullptr )
        delete this->_transform;
}

void finGraphConfig::copyGraphConfig(const finGraphConfig *srccfg)
{
    this->_panelSize = srccfg->_panelSize;
    this->_bgColor = srccfg->_bgColor;

    this->_originPoint = srccfg->_originPoint;
    this->_axisUnitSize = srccfg->_axisUnitSize;
    this->_axisRadZ = srccfg->_axisRadZ;
    this->_axisScaleZ = srccfg->_axisScaleZ;

    this->cloneTransform(srccfg);
    this->_renderHints = srccfg->_renderHints;
}

const QSizeF &finGraphConfig::getPanelPixelSize() const
{
    return this->_panelSize;
}

double finGraphConfig::getPanelPixelWidth() const
{
    return this->_panelSize.width();
}

double finGraphConfig::getPanelPixelHeight() const
{
    return this->_panelSize.height();
}

QRectF finGraphConfig::getWholePanelPixelRect() const
{
    return QRectF(QPointF(0.0, 0.0), this->getPanelPixelSize());
}

QList<QPointF> finGraphConfig::getCornerAxisPoints() const
{
    QRectF rect = this->getWholePanelPixelRect();
    QList<QPointF> ptlist;
    ptlist.append(this->arcTransformPixelPoint(rect.topLeft()));
    ptlist.append(this->arcTransformPixelPoint(rect.topRight()));
    ptlist.append(this->arcTransformPixelPoint(rect.bottomRight()));
    ptlist.append(this->arcTransformPixelPoint(rect.bottomLeft()));
    return ptlist;
}

const QColor &finGraphConfig::getBackgroundColor() const
{
    return this->_bgColor;
}

QBrush finGraphConfig::getBackgroundBrush() const
{
    return QBrush(this->_bgColor, Qt::SolidPattern);
}

const QPointF &finGraphConfig::getOriginPixelPoint() const
{
    return this->_originPoint;
}

double finGraphConfig::getOriginPixelPointX() const
{
    return this->_originPoint.x();
}

double finGraphConfig::getOriginPixelPointY() const
{
    return this->_originPoint.y();
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

bool finGraphConfig::isOriginPointOnPanel() const
{
    return this->getWholePanelPixelRect().contains(this->_originPoint);
}

finGraphTransType finGraphConfig::getTransformType() const
{
    if ( this->_transform == nullptr || this->_transform->getTransformType() == finGraphTrans::TP_NONE )
        return finGraphTrans::TP_NONE;
    else
        return this->_transform->getTransformType();
}

finGraphTrans *finGraphConfig::getTransform() const
{
    return this->_transform;
}

bool finGraphConfig::isLinearTransform() const
{
    if ( this->_transform == nullptr || this->_transform->getTransformType() == finGraphTrans::TP_NONE )
        return true;
    else
        return this->_transform->isLinear();
}

QPainter::RenderHints finGraphConfig::getRenderHints() const
{
    return this->_renderHints;
}

void finGraphConfig::setPanelPixelSize(const QSizeF &size)
{
    this->_panelSize = size;
}

void finGraphConfig::setPanelPixelWidth(double width)
{
    this->_panelSize.setWidth(width);
}

void finGraphConfig::setPanelPixelHeight(double height)
{
    this->_panelSize.setHeight(height);
}

void finGraphConfig::setPanelPixelSize(double sizex, double sizey)
{
    this->_panelSize.setWidth(sizex);
    this->_panelSize.setHeight(sizey);
}

void finGraphConfig::setBackgroundColor(const QColor &color)
{
    this->_bgColor = color;
}

void finGraphConfig::setOriginPixelPoint(const QPointF &pt)
{
    this->_originPoint = pt;
}

void finGraphConfig::setOriginPixelPointX(double ptx)
{
    this->_originPoint.setX(ptx);
}

void finGraphConfig::setOriginPixelPointY(double pty)
{
    this->_originPoint.setY(pty);
}

void finGraphConfig::setOriginPixelPoint(double ptx, double pty)
{
    this->_originPoint.setX(ptx);
    this->_originPoint.setY(pty);
}

void finGraphConfig::setAxisUnitPixelSize(double size)
{
    this->_axisUnitSize = size;
}

void finGraphConfig::setAxisRadZ(double rad)
{
    this->_axisRadZ = rad;
}

void finGraphConfig::setAxisScaleZ(double scale)
{
    this->_axisScaleZ = scale;
}

void finGraphConfig::setTransformType(finGraphTransType type)
{
    if ( this->getTransformType() == type )
        return;

    finGraphTrans *newtrans = nullptr;
    switch ( type ) {
      case finGraphTrans::TP_RECT:
        newtrans = new finGraphTransRect();
        if ( newtrans == nullptr )
            finThrow(finErrorKits::EC_OUT_OF_MEMORY, QString("Failed to create transform."));
        break;

      case finGraphTrans::TP_AFFINE:
        newtrans = new finGraphTransAffine();
        if ( newtrans == nullptr )
            finThrow(finErrorKits::EC_OUT_OF_MEMORY, QString("Failed to create transform."));
        break;

      default:
        break;
    }

    finGraphTrans *oldtrans = this->_transform;
    this->_transform = newtrans;
    if ( oldtrans != nullptr )
        delete oldtrans;
    return;
}

void finGraphConfig::cloneTransform(const finGraphTrans *srctrans)
{
    if ( srctrans == nullptr || srctrans->getTransformType() == finGraphTrans::TP_NONE ) {
        this->setTransformType(finGraphTrans::TP_NONE);
        return;
    }

    this->setTransformType(srctrans->getTransformType());

    finGraphTrans *mytrans = this->getTransform();
    if ( mytrans == nullptr )
        return;

    finErrorCode errcode = mytrans->cloneTransform(srctrans);
    if ( finErrorKits::isErrorResult(errcode) )
        finThrow(errcode, QString("Failed to clone transform."));
}

void finGraphConfig::cloneTransform(const finGraphConfig *srccfg)
{
    this->cloneTransform(srccfg->getTransform());
}

QTransform finGraphConfig::getNakePixelTransformMatrix() const
{
    QTransform trans, subtrans;
    trans.scale(this->_axisUnitSize, -this->_axisUnitSize);
    subtrans.translate(this->_originPoint.x(), this->_originPoint.y());
    return trans * subtrans;
}

finErrorCode finGraphConfig::setRenderHints(QPainter::RenderHints hints)
{
    this->_renderHints = hints;
    return finErrorKits::EC_SUCCESS;
}

QPointF finGraphConfig::transformPixelPoint3D(double x, double y, double z) const
{
    QPointF srcpt(x, y);
    QPointF zshift(z * this->_axisScaleZ * cos(this->_axisRadZ), z * this->_axisScaleZ * sin(this->_axisRadZ));

    return this->transformPixelPoint(srcpt + zshift);
}

QPointF finGraphConfig::transformPixelPoint3D(const finFigurePoint3D &pt) const
{
    return this->transformPixelPoint3D(pt.getX(), pt.getY(), pt.getZ());
}

QPointF finGraphConfig::transformPixelPoint(const QPointF &srcpt) const
{
    QPointF midpt;
    if ( this->_transform != nullptr && this->_transform->getTransformType() != finGraphTrans::TP_NONE )
        midpt = this->_transform->transPoint(srcpt);
    else
        midpt = srcpt;
    midpt.setY(-midpt.y());

    return midpt * this->_axisUnitSize + this->_originPoint;
}

QPointF finGraphConfig::arcTransformPixelPoint(const QPointF &srcpt) const
{
    QPointF midpt = srcpt;
    midpt = (midpt - this->_originPoint) / this->_axisUnitSize;
    midpt.setY(-midpt.y());

    if ( this->_transform != nullptr && this->_transform->getTransformType() != finGraphTrans::TP_NONE )
        return this->_transform->arcTransPoint(midpt);
    else
        return midpt;
}

QPainterPath finGraphConfig::transformPixelPath(const QPainterPath &path) const
{
    QPainterPath outpath = path;
    for ( int i = 0; i < outpath.elementCount(); i++ ) {
        QPointF srcpt = (QPointF)(outpath.elementAt(i));
        QPointF dstpt = this->transformPixelPoint(srcpt);
        outpath.setElementPositionAt(i, dstpt.x(), dstpt.y());
    }
    return outpath;
}

QPainterPath finGraphConfig::arcTransformPixelPath(const QPainterPath &path) const
{
    QPainterPath outpath = path;
    for ( int i = 0; i < outpath.elementCount(); i++ ) {
        QPointF srcpt = (QPointF)(outpath.elementAt(i));
        QPointF dstpt = this->arcTransformPixelPoint(srcpt);
        outpath.setElementPositionAt(i, dstpt.x(), dstpt.y());
    }
    return outpath;
}

QPolygonF finGraphConfig::transformPixelPolygon(const QPolygonF &polygon) const
{
    QPolygonF retpolygon;
    for ( int i = 0; i < polygon.count(); i++ ) {
        const QPointF &srcpt = polygon.at(i);
        retpolygon.append(this->transformPixelPoint(srcpt));
    }
    return retpolygon;
}

QPolygonF finGraphConfig::arcTransformPixelPolygon(const QPolygonF &polygon) const
{
    QPolygonF retpolygon;
    for ( int i = 0; i < polygon.count(); i++ ) {
        const QPointF &srcpt = polygon.at(i);
        retpolygon.append(this->arcTransformPixelPoint(srcpt));
    }
    return retpolygon;
}

QString finGraphConfig::getRenderHintsName(const QPainter::RenderHints &hints)
{
    if ( hints.testFlag(QPainter::Antialiasing) &&
         hints.testFlag(QPainter::SmoothPixmapTransform) ) {
        return QString("high_antialiasing");
    } else if ( hints.testFlag(QPainter::Antialiasing) ) {
        return QString("antialiasing");
    } else if ( hints.testFlag(QPainter::SmoothPixmapTransform) ) {
        return QString("smooth_image");
    } else {
        return QString("none");
    }
}

QPainter::RenderHints finGraphConfig::parseRenderHints(const QString &name)
{
    if ( QString::compare(name, QString("none"), Qt::CaseInsensitive) == 0 ) {
        // TODO: Use QFlag instead
        return QPainter::Antialiasing;
    } else if ( QString::compare(name, QString("high_antialiasing"), Qt::CaseInsensitive) == 0 ) {
        return (QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    } else if ( QString::compare(name, QString("antialiasing"), Qt::CaseInsensitive) == 0 ) {
        return (QPainter::Antialiasing);
    } else if ( QString::compare(name, QString("smooth_image"), Qt::CaseInsensitive) == 0 ) {
        return (QPainter::SmoothPixmapTransform);
    } else {
        // TODO: Use QFlag instead
        return QPainter::Antialiasing;
    }
}

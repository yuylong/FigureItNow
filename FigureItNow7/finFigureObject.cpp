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
 * 20170102    1  Yulong Yu    Add dot and line figure objects.
 */

#include "finFigureObject.h"

#include <qmath.h>
#include <QMarginsF>
#include <QFontMetricsF>

#include "finFigureAlg.h"


finFigureObject::finFigureObject()
{
    this->_type = finFigureObject::FIN_FO_TYPE_DUMMY;
}

bool finFigureObject::is3DFigure() const
{
    return false;
}

finFigureObjectType finFigureObject::getFigureType() const
{
    return this->_type;
}

const finFigureConfig *finFigureObject::getFigureConfig() const
{
    return &this->_figCfg;
}

finFigureConfig *finFigureObject::getFigureConfig()
{
    return &this->_figCfg;
}

finErrorCode finFigureObject::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

void finFigureObject::dump() const
{
    printf(" * Fig Type: dummy\n");
}

finFigureObjectDot::finFigureObjectDot()
{
    this->_type = finFigureObject::FIN_FO_TYPE_DOT;
    this->_point = QPointF(0.0, 0.0);
}

bool finFigureObjectDot::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectDot::getPoint() const
{
    return this->_point;
}

finErrorCode finFigureObjectDot::setPoint(const QPointF &qpt)
{
    this->_point = qpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectDot::setPoint(double ptx, double pty)
{
    this->_point.setX(ptx);
    this->_point.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectDot::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPointF pixpt = cfg->transformPixelPoint(this->_point);
    QPainterPath path;
    path.moveTo(pixpt);
    path.lineTo(pixpt);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectDot::dump() const
{
    printf(" * Fig Type: dot; Point: (%lf, %lf)\n", (double)this->_point.x(), (double)this->_point.y());
}

finFigureObjectLine::finFigureObjectLine()
{
    this->_type = finFigureObject::FIN_FO_TYPE_LINE;
    this->_pt1 = QPointF(0.0, 0.0);
    this->_pt2 = QPointF(0.0, 0.0);
}

bool finFigureObjectLine::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectLine::getPoint1() const
{
    return this->_pt1;
}

QPointF finFigureObjectLine::getPoint2() const
{
    return this->_pt2;
}

finErrorCode finFigureObjectLine::setPoint1(const QPointF &qpt)
{
    this->_pt1 = qpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine::setPoint1(double ptx, double pty)
{
    this->_pt1.setX(ptx);
    this->_pt1.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine::setPoint2(const QPointF &qpt)
{
    this->_pt2 = qpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine::setPoint2(double ptx, double pty)
{
    this->_pt2.setX(ptx);
    this->_pt2.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPointF pt1 = cfg->transformPixelPoint(this->_pt1);
    QPointF pt2 = cfg->transformPixelPoint(this->_pt2);
    QPointF spt1 = this->_figCfg.getStartArrow().lineShrinkPoint(pt1, pt2, &this->_figCfg);
    QPointF spt2 = this->_figCfg.getEndArrow().lineShrinkPoint(pt2, pt1, &this->_figCfg);

    QPainterPath path;
    path.moveTo(spt1);
    path.lineTo(spt2);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    this->_figCfg.getStartArrow().getPixelPath(pathlist, pt1, pt2, &this->_figCfg);
    this->_figCfg.getEndArrow().getPixelPath(pathlist, pt2, pt1, &this->_figCfg);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectLine::dump() const
{
    printf(" * Fig Type: line; (%lf, %lf) -- (%lf, %lf)\n",
           (double)this->_pt1.x(), (double)this->_pt1.y(), (double)this->_pt2.x(), (double)this->_pt2.y());
}

finFigureObjectPolyline::finFigureObjectPolyline()
    : _ptList()
{
    this->_type = finFigureObject::FIN_FO_TYPE_POLYLINE;
}

bool finFigureObjectPolyline::is3DFigure() const
{
    return false;
}

int finFigureObjectPolyline::getPointCount() const
{
    return this->_ptList.count();
}

QPointF finFigureObjectPolyline::getPointAt(int idx) const
{
    return this->_ptList.at(idx);
}

finErrorCode finFigureObjectPolyline::appendPoint(const QPointF &pt)
{
    this->_ptList.append(pt);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectPolyline::appendPoint(double ptx, double pty)
{
    this->_ptList.append(QPointF(ptx, pty));
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectPolyline::removePointAt(int idx)
{
    this->_ptList.removeAt(idx);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectPolyline::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_ptList.count() < 2 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint(this->_ptList.at(0)));
    for ( int i = 1; i < this->_ptList.count(); i++ ) {
        path.lineTo(cfg->transformPixelPoint(this->_ptList.at(i)));
    }

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectPolyline::dump() const
{
    printf(" * Fig Type: polyline; ");
    if ( this->_ptList.empty() ) {
        printf ("EMPTY\n");
        return;
    }

    printf("(%lf, %lf)", this->_ptList.at(0).x(), this->_ptList.at(1).y());
    for ( int i = 1; i < this->_ptList.count(); i++ ) {
        printf(" -- (%lf, %lf)", this->_ptList.at(i).x(), this->_ptList.at(i).y());
    }
    printf("\n");
}

finFigureObjectRect::finFigureObjectRect()
{
    this->_type = finFigureObject::FIN_FO_TYPE_RECT;
    this->_center = QPointF(0.0, 0.0);
    this->_size = QSizeF(0.0, 0.0);
    this->_radian = 0.0;
}

bool finFigureObjectRect::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectRect::getCenterPoint() const
{
    return this->_center;
}

QSizeF finFigureObjectRect::getSize() const
{
    return this->_size;
}

double finFigureObjectRect::getRadian() const
{
    return this->_radian;
}

QPointF finFigureObjectRect::getUpperLeftPoint() const
{
    QPointF shiftpt = QPointF(-this->_size.width() / 2.0, this->_size.height() / 2.0);
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

QPointF finFigureObjectRect::getUpperRightPoint() const
{
    QPointF shiftpt = QPointF(this->_size.width() / 2.0, this->_size.height() / 2.0);
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

QPointF finFigureObjectRect::getLowerLeftPoint() const
{
    QPointF shiftpt = QPointF(-this->_size.width() / 2.0, -this->_size.height() / 2.0);
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

QPointF finFigureObjectRect::getLowerRightPoint() const
{
    QPointF shiftpt = QPointF(this->_size.width() / 2.0, -this->_size.height() / 2.0);
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

finErrorCode finFigureObjectRect::setCenterPoint(const QPointF &ctrpt)
{
    this->_center = ctrpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectRect::setCenterPoint(double cx, double cy)
{
    this->_center.setX(cx);
    this->_center.setY(cy);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectRect::setSize(const QSizeF &size)
{
    this->_size = size;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectRect::setSize(double width, double height)
{
    this->_size.setWidth(width);
    this->_size.setHeight(height);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectRect::setRadian(double rad)
{
    // Make the degree falling into the range (-90, 90].
    rad = rad - floor(rad / M_PI) * M_PI;
    if ( rad > M_PI / 2 )
        rad = rad - M_PI;

    this->_radian = rad;
    this->_sinrad = sin(rad);
    this->_cosrad = cos(rad);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectRect::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    QPointF startptr = cfg->transformPixelPoint(this->getUpperLeftPoint());
    path.moveTo(startptr);
    path.lineTo(cfg->transformPixelPoint(this->getUpperRightPoint()));
    path.lineTo(cfg->transformPixelPoint(this->getLowerRightPoint()));
    path.lineTo(cfg->transformPixelPoint(this->getLowerLeftPoint()));
    path.lineTo(startptr);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectRect::dump() const
{
    printf(" * Fig Type: rect; C: (%lf, %lf) S: %lf x %lf rad: %lf\n",
           (double)this->_center.x(), (double)this->_center.y(),
           (double)this->_size.width(), (double)this->_size.height(), this->_radian);
}

finFigureObjectPolygon::finFigureObjectPolygon()
    : _ptList()
{
    this->_type = finFigureObject::FIN_FO_TYPE_POLYGON;
}

bool finFigureObjectPolygon::is3DFigure() const
{
    return false;
}

int finFigureObjectPolygon::getPointCount() const
{
    return this->_ptList.count();
}

QPointF finFigureObjectPolygon::getPointAt(int idx) const
{
    return this->_ptList.at(idx);
}

finErrorCode finFigureObjectPolygon::appendPoint(const QPointF &pt)
{
    this->_ptList.append(pt);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectPolygon::appendPoint(double ptx, double pty)
{
    this->_ptList.append(QPointF(ptx, pty));
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectPolygon::removePointAt(int idx)
{
    this->_ptList.removeAt(idx);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finFigureObjectPolygon::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_ptList.count() < 2 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    QPainterPath path;
    QPointF startpt = cfg->transformPixelPoint(this->_ptList.at(0));
    path.moveTo(startpt);
    for ( int i = 1; i < this->_ptList.count(); i++ ) {
        path.lineTo(cfg->transformPixelPoint(this->_ptList.at(i)));
    }
    path.lineTo(startpt);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectPolygon::dump() const
{
    printf(" * Fig Type: polygon; ");
    if ( this->_ptList.empty() ) {
        printf ("EMPTY\n");
        return;
    }

    printf("(%lf, %lf)", this->_ptList.at(0).x(), this->_ptList.at(1).y());
    for ( int i = 1; i < this->_ptList.count(); i++ ) {
        printf(" -- (%lf, %lf)", this->_ptList.at(i).x(), this->_ptList.at(i).y());
    }
    printf("\n");
}

finFigureObjectEllipse::finFigureObjectEllipse()
{
    this->_type = finFigureObject::FIN_FO_TYPE_ELLIPSE;
    this->_center = QPointF(0.0, 0.0);
    this->_longR = 0.0;
    this->_shortR = 0.0;
    this->_radian = 0.0;
}

bool finFigureObjectEllipse::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectEllipse::getCenterPoint() const
{
    return this->_center;
}

double finFigureObjectEllipse::getLongRadius() const
{
    return this->_longR;
}

double finFigureObjectEllipse::getShortRadius() const
{
    return this->_shortR;
}

double finFigureObjectEllipse::getRadian() const
{
    return this->_radian;
}

finErrorCode finFigureObjectEllipse::setCenterPoint(const QPointF &ctrpt)
{
    this->_center = ctrpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectEllipse::setCenterPoint(double cx, double cy)
{
    this->_center.setX(cx);
    this->_center.setY(cy);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectEllipse::setLongRadius(double lr)
{
    this->_longR = lr;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectEllipse::setShortRadius(double sr)
{
    this->_shortR = sr;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectEllipse::setRadian(double rad)
{
    // Make the degree falling into the range (-90, 90].
    rad = rad - floor(rad / M_PI) * M_PI;
    if ( rad > M_PI / 2 )
        rad = rad - M_PI;

    this->_radian = rad;
    this->_sinrad = sin(rad);
    this->_cosrad = cos(rad);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QPointF finFigureObjectEllipse::getEllipsePointAtRad(double rad) const
{
    QPointF shiftpt = QPointF(this->_longR * cos(rad), this->_shortR * sin(rad));
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

finErrorCode finFigureObjectEllipse::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    QPointF startpt = cfg->transformPixelPoint(this->getEllipsePointAtRad(0.0));
    static const double drawstep = M_PI / 36.0;

    path.moveTo(startpt);
    for ( double rad = drawstep; rad < 2 * M_PI; rad += drawstep)
        path.lineTo(cfg->transformPixelPoint(this->getEllipsePointAtRad(rad)));
    path.lineTo(startpt);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectEllipse::dump() const
{
    printf(" * Fig Type: rect; C: (%lf, %lf) R: %lf x %lf rad: %lf\n",
           (double)this->_center.x(), (double)this->_center.y(),
           (double)this->_longR, (double)this->_shortR, this->_radian);
}

finFigureObjectText::finFigureObjectText()
{
    this->_type = finFigureObject::FIN_FO_TYPE_TEXT;
    this->_basePtr = QPointF(0.0, 0.0);
    this->_flag = Qt::AlignCenter;
    this->_scale = 1.0;
    this->_rad = 0.0;
    this->_text = QString();
    this->_isPinned = false;
}

bool finFigureObjectText::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectText::getBasePoint() const
{
    return this->_basePtr;
}

int finFigureObjectText::getFontMetricFlags() const
{
    return this->_flag;
}

double finFigureObjectText::getScale() const
{
    return this->_scale;
}

double finFigureObjectText::getRadian() const
{
    return this->_rad;
}

QString finFigureObjectText::getText() const
{
    return this->_text;
}

bool finFigureObjectText::isPinned() const
{
    return this->_isPinned;
}

finErrorCode finFigureObjectText::setBasePoint(const QPointF &pt)
{
    this->_basePtr = pt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setBasePoint(double ptx, double pty)
{
    this->_basePtr.setX(ptx);
    this->_basePtr.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setFontMetricFlags(int flag)
{
    this->_flag = flag;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setScale(double scale)
{
    this->_scale = scale;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setRadian(double rad)
{
    // Make the degree falling into the range (-180, 180].
    rad = rad - floor(rad / (2 * M_PI)) * 2 * M_PI;
    if ( rad > M_PI )
        rad = rad - 2 * M_PI;

    this->_rad = rad;
    this->_sinrad = sin(rad);
    this->_cosrad = cos(rad);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setText(const QString &text)
{
    this->_text = text;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectText::setIsPinned(bool pinned)
{
    this->_isPinned = pinned;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QRectF finFigureObjectText::getBoundingRect() const
{
    QFontMetricsF metric(this->_figCfg.getFont());
    return metric.boundingRect(this->_text).marginsAdded(this->_figCfg.getTextMargins());
}

QPointF finFigureObjectText::getBasePointOffset(const QRectF &boundrect) const
{
    QPointF bloff = QPointF(0.0, 0.0);
    if ( this->_flag & Qt::AlignLeft ) {
        bloff.setX(-boundrect.x());
    } else if ( this->_flag & Qt::AlignRight ) {
        bloff.setX(-boundrect.x() - boundrect.width());
    } else if ( this->_flag & Qt::AlignJustify ) {
        bloff.setX(0.0);
    } else {
        bloff.setX(-boundrect.x() - boundrect.width() / 2.0);
    }
    if ( this->_flag &  Qt::AlignTop ) {
        bloff.setY(-boundrect.y());
    } else if ( this->_flag & Qt::AlignBottom ) {
        bloff.setY(-boundrect.y() - boundrect.height());
    } else if ( this->_flag & Qt::AlignBaseline ) {
        bloff.setY(0.0);
    } else {
        bloff.setY(-boundrect.y() - boundrect.height() / 2.0);
    }
    return bloff;
}

QPainterPath finFigureObjectText::getPinnedPixelTextPath(finGraphConfig *cfg) const
{
    QPainterPath path;
    path.addText(0.0, 0.0, this->_figCfg.getFont(), this->_text);

    QRectF bndrect = path.boundingRect().marginsAdded(this->_figCfg.getTextMargins());
    QPointF bloff = this->getBasePointOffset(bndrect);

    QTransform trans, subtrans;
    trans.translate(bloff.x(), bloff.y());
    subtrans.scale(this->_scale / cfg->getAxisUnitPixelSize(), -1.0 / cfg->getAxisUnitPixelSize());
    trans *= subtrans;

    subtrans.reset();
    subtrans.rotateRadians(this->_rad);
    trans *= subtrans;

    subtrans.reset();
    subtrans.translate(this->_basePtr.x(), this->_basePtr.y());
    trans *= subtrans;
    path = trans.map(path);

    for ( int i = 0; i < path.elementCount(); i++ ) {
        QPointF matpt = (QPointF)(path.elementAt(i));
        QPointF pixpt = cfg->transformPixelPoint(matpt);
        path.setElementPositionAt(i, pixpt.x(), pixpt.y());
    }
    return path;
}

QPainterPath finFigureObjectText::getUnpinnedPixelTextPath(finGraphConfig *cfg) const
{
    QPainterPath path;
    path.addText(0.0, 0.0, this->_figCfg.getFont(), this->_text);

    QRectF bndrect = path.boundingRect().marginsAdded(this->_figCfg.getTextMargins());
    QPointF bloff = this->getBasePointOffset(bndrect);

    QTransform trans, subtrans;
    trans.translate(bloff.x(), bloff.y());
    subtrans.scale(this->_scale, 1.0);
    trans *= subtrans;

    subtrans.reset();
    subtrans.rotateRadians(-this->_rad);
    trans *= subtrans;

    QPointF pixbp = cfg->transformPixelPoint(this->_basePtr);
    subtrans.reset();
    subtrans.translate(pixbp.x(), pixbp.y());
    trans *= subtrans;

    return trans.map(path);
}

QPainterPath finFigureObjectText::getPixelTextPath(finGraphConfig *cfg) const
{
    if ( cfg == NULL )
        return QPainterPath();

    if ( this->_isPinned )
        return getPinnedPixelTextPath(cfg);
    else
        return getUnpinnedPixelTextPath(cfg);
}

finErrorCode
finFigureObjectText::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path = this->getPixelTextPath(cfg);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getTextPen());
    figpath.setBrush(this->_figCfg.getTextBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectText::dump() const
{
    printf(" * Fig Type: text; C: (%lf, %lf) F: %d S: %lf rad: %lf T: \"%s\"\n",
           this->_basePtr.x(), this->_basePtr.y(), this->_flag, this->_scale, this->_rad,
           this->_text.toLatin1().data());
}

finFigureObjectImage::finFigureObjectImage()
{
    this->_type = finFigureObject::FIN_FO_TYPE_TEXT;
    this->_basePtr = QPointF(0.0, 0.0);
    this->_flag = Qt::AlignCenter;
    this->_scaleX = 1.0;
    this->_scaleY = 1.0;
    this->_rad = 0.0;
    this->_img = QImage();
    this->_isPinned = false;
}

bool finFigureObjectImage::is3DFigure() const
{
    return false;
}

QPointF finFigureObjectImage::getBasePoint() const
{
    return this->_basePtr;
}

int finFigureObjectImage::getAlignFlags() const
{
    return this->_flag;
}

double finFigureObjectImage::getScaleX() const
{
    return this->_scaleX;
}

double finFigureObjectImage::getScaleY() const
{
    return this->_scaleY;
}

double finFigureObjectImage::getRadian() const
{
    return this->_rad;
}

QImage finFigureObjectImage::getImage() const
{
    return this->_img;
}

bool finFigureObjectImage::isPinned() const
{
    return this->_isPinned;
}

finErrorCode finFigureObjectImage::setBasePoint(const QPointF &pt)
{
    this->_basePtr = pt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setBasePoint(double ptx, double pty)
{
    this->_basePtr.setX(ptx);
    this->_basePtr.setY(pty);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setAlignFlags(int flag)
{
    this->_flag = flag;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setScaleX(double scale)
{
    this->_scaleX = scale;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setScaleY(double scale)
{
    this->_scaleY = scale;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setRadian(double rad)
{
    // Make the degree falling into the range (-180, 180].
    rad = rad - floor(rad / (2 * M_PI)) * 2 * M_PI;
    if ( rad > M_PI )
        rad = rad - 2 * M_PI;

    this->_rad = rad;
    this->_sinrad = sin(rad);
    this->_cosrad = cos(rad);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setImage(const QImage &image)
{
    this->_img = image;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectImage::setIsPinned(bool pinned)
{
    this->_isPinned = pinned;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QRectF finFigureObjectImage::getBoundingRect() const
{
    return this->_img.rect();
}

finErrorCode finFigureObjectImage::getUnpinnedPixelFigurePath(QList<finFigurePath> *pathlist,
                                                              finGraphConfig *cfg) const
{
    QTransform trans, subtrans;
    trans.scale(this->_scaleX, this->_scaleY);
    subtrans.rotateRadians(-this->_rad);
    trans *= subtrans;

    QPolygonF rtptlist(QRectF(this->_img.rect()));
    QPointF offpt (0.0, 0.0);
    for ( int i = 0; i < rtptlist.count(); i++ ) {
        QPointF curpt = trans.map(rtptlist.at(i));
        if ( curpt.x() < offpt.x() )
            offpt.setX(curpt.x());
        if ( curpt.y() < offpt.y() )
             offpt.setY(curpt.y());
    }
    subtrans.reset();
    subtrans.translate(-offpt.x(), -offpt.y());
    trans *= subtrans;
    QImage outimg = this->_img.transformed(trans);

    QPointF imgoffpt = QPointF(0.0, 0.0);
    if ( this->_flag & Qt::AlignRight ) {
        imgoffpt.setX(imgoffpt.x() + this->_img.width());
    } else if ( this->_flag & Qt::AlignHCenter ) {
        imgoffpt.setX(imgoffpt.x() + this->_img.width() / 2.0);
    }
    if ( this->_flag & Qt::AlignBottom ) {
        imgoffpt.setY(imgoffpt.y() + this->_img.height());
    } else if ( this->_flag & Qt::AlignVCenter ) {
        imgoffpt.setY(imgoffpt.y() + this->_img.height() / 2.0);
    }
    QPointF imgpos = cfg->transformPixelPoint(this->_basePtr) - trans.map(imgoffpt);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getTextPen());
    figpath.setBrush(this->_figCfg.getTextBrush());
    figpath.setImagePosition(imgpos);
    figpath.setImage(outimg);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QTransform finFigureObjectImage::getParameterTransformMatrix() const
{
    QTransform trans, subtrans;
    trans.translate(-this->_basePtr.x(), -this->_basePtr.y());
    subtrans.scale(this->_scaleX, this->_scaleY);
    trans *= subtrans;

    subtrans.reset();
    subtrans.rotateRadians(this->_rad);
    trans *= subtrans;

    subtrans.reset();
    subtrans.translate(this->_basePtr.x(), this->_basePtr.y());
    return trans * subtrans;
}

finErrorCode finFigureObjectImage::getPinnedPixelFigurePath(QList<finFigurePath> *pathlist,
                                                            finGraphConfig *cfg) const
{
    QSizeF imgmathsize = QSizeF(this->_img.width() / cfg->getAxisUnitPixelSize(),
                                this->_img.height() / cfg->getAxisUnitPixelSize());
    QPointF imgctrpt = this->_basePtr;
    if ( this->_flag & Qt::AlignRight ) {
        imgctrpt.setX(this->_basePtr.x() + imgmathsize.width() / 2.0);
    } else if ( this->_flag & Qt::AlignLeft ) {
        imgctrpt.setX(this->_basePtr.x() - imgmathsize.width() / 2.0);
    }
    if ( this->_flag & Qt::AlignBottom ) {
        imgctrpt.setY(this->_basePtr.y() + imgmathsize.height() / 2.0);
    } else if ( this->_flag & Qt::AlignTop ) {
        imgctrpt.setY(this->_basePtr.y() - imgmathsize.height() / 2.0);
    }

    // I do not use the QT wrapped functions due to the corresponding point positions.
    QPolygonF imgpg;
    imgpg.append(QPointF(0.0, 0.0));
    imgpg.append(QPointF(this->_img.width(), 0.0));
    imgpg.append(QPointF(this->_img.width(), this->_img.height()));
    imgpg.append(QPointF(0.0, this->_img.height()));

    QPolygonF imgmatpg;
    imgmatpg.append(imgctrpt + QPointF(-imgmathsize.width() / 2.0, imgmathsize.height() / 2.0));
    imgmatpg.append(imgctrpt + QPointF(imgmathsize.width() / 2.0, imgmathsize.height() / 2.0));
    imgmatpg.append(imgctrpt + QPointF(imgmathsize.width() / 2.0, -imgmathsize.height() / 2.0));
    imgmatpg.append(imgctrpt + QPointF(-imgmathsize.width() / 2.0, -imgmathsize.height() / 2.0));

    QTransform partrans = this->getParameterTransformMatrix();
    imgmatpg = partrans.map(imgmatpg);

    QPolygonF imgpixpg;
    for ( int i = 0; i < imgmatpg.count(); i++ ) {
        QPointF curpt = cfg->transformPixelPoint(imgmatpg.at(i));
        imgpixpg.append(curpt);
    }
    QPointF imgpos = imgpixpg.boundingRect().topLeft();

    QTransform img2mattrans = finFigureAlg::fourPointMatrix(imgpg.toList(), imgmatpg.toList());
    QTransform mat2pixtrans = finFigureAlg::fourPointMatrix(imgmatpg.toList(), imgpixpg.toList());
    QTransform img2pixtrans = img2mattrans * mat2pixtrans;

    QImage outimg = this->_img.transformed(img2pixtrans);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getTextPen());
    figpath.setBrush(this->_figCfg.getTextBrush());
    figpath.setImagePosition(imgpos);
    figpath.setImage(outimg);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode
finFigureObjectImage::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( this->_img.isNull() )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    if ( this->_isPinned )
        return this->getPinnedPixelFigurePath(pathlist, cfg);
    else
        return this->getUnpinnedPixelFigurePath(pathlist, cfg);
}

void finFigureObjectImage::dump() const
{
    printf(" * Fig Type: image; P:%d C: (%lf, %lf) F: %d S: %lf x %lf rad: %lf ImgSize: %dx%d\n",
           this->_isPinned, this->_basePtr.x(), this->_basePtr.y(), this->_flag,
           this->_scaleX, this->_scaleY, this->_rad,
           this->_img.size().width(), this->_img.size().height());
}

finFigureObjectAxis::finFigureObjectAxis()
{
    this->_minX = 1.0;
    this->_maxX = -1.0;
    this->_minY = 1.0;
    this->_maxY = -1.0;
    this->_stepX = -1.0;
    this->_stepY = -1.0;
    this->_titleX = QString("x");
    this->_titleY = QString("y");
}

bool finFigureObjectAxis::is3DFigure() const
{
    return false;
}

double finFigureObjectAxis::getRangeMinX() const
{
    return this->_minX;
}

double finFigureObjectAxis::getRangeMaxX() const
{
    return this->_maxX;
}

double finFigureObjectAxis::getRangeMinY() const
{
    return this->_minY;
}

double finFigureObjectAxis::getRangeMaxY() const
{
    return this->_maxY;
}

bool finFigureObjectAxis::isAutoRangeX() const
{
    return (this->_minX >= this->_maxX);
}

bool finFigureObjectAxis::isAutoRangeY() const
{
    return (this->_minY >= this->_maxY);
}

double finFigureObjectAxis::getStepX() const
{
    return this->_stepX;
}

double finFigureObjectAxis::getStepY() const
{
    return this->_stepY;
}

bool finFigureObjectAxis::isAutoStepX() const
{
    return (this->_stepX <= 0.0);
}

bool finFigureObjectAxis::isAutoStepY() const
{
    return (this->_stepY <= 0.0);
}

QString finFigureObjectAxis::getTitleX() const
{
    return this->_titleX;
}

QString finFigureObjectAxis::getTitleY() const
{
    return this->_titleY;
}

finErrorCode finFigureObjectAxis::setAutoRangeX()
{
    this->_minX = 1.0;
    this->_maxX = -1.0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setRangeX(double minx, double maxx)
{
    this->_minX = minx;
    this->_maxX = maxx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setAutoRangeY()
{
    this->_minY = 1.0;
    this->_maxY = -1.0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setRangeY(double miny, double maxy)
{
    this->_minY = miny;
    this->_maxY = maxy;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setAutoStepX()
{
    this->_stepX = -1.0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setStepX(double step)
{
    this->_stepX = step;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setAutoStepY()
{
    this->_stepY = -1.0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setStepY(double step)
{
    this->_stepY = step;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setTitleX(const QString &title)
{
    this->_titleX = title;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setTitleY(const QString &title)
{
    this->_titleY = title;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

double finFigureObjectAxis::_defMinX = -10.0;
double finFigureObjectAxis::_defMaxX = 10.0;
double finFigureObjectAxis::_defMinY = -10.0;
double finFigureObjectAxis::_defMaxY = 10.0;

QRectF finFigureObjectAxis::getDefaultRangeRect()
{
    return QRectF(finFigureObjectAxis::_defMinX, finFigureObjectAxis::_defMinY,
                  (finFigureObjectAxis::_defMaxX - finFigureObjectAxis::_defMinX),
                  (finFigureObjectAxis::_defMaxY - finFigureObjectAxis::_defMinY));
}

double finFigureObjectAxis::getSecondMinNum(double *ary, int cnt) const
{
    if ( cnt <= 0 )
        return 0.0;
    else if ( cnt == 1)
        return ary[0];

    double min = (ary[0] < ary[1] ? ary[0] : ary[1]);
    double min2 = (ary[0] < ary[1] ? ary[1] : ary[0]);

    for ( int i = 2; i < cnt; i++ ) {
        if ( ary[i] < min ) {
            min2 = min;
            min = ary[i];
        } else if ( ary[i] < min2 ) {
            min2 = ary[i];
        }
    }
    return min2;
}

double finFigureObjectAxis::getSecondMaxNum(double *ary, int cnt) const
{
    if ( cnt <= 0 )
        return 0.0;
    else if ( cnt == 1)
        return ary[0];

    double max = (ary[0] > ary[1] ? ary[0] : ary[1]);
    double max2 = (ary[0] > ary[1] ? ary[1] : ary[0]);

    for ( int i = 2; i < cnt; i++ ) {
        if ( ary[i] > max ) {
            max2 = max;
            max = ary[i];
        } else if ( ary[i] > max2 ) {
            max2 = ary[i];
        }
    }
    return max2;
}

QRectF finFigureObjectAxis::getCrossPointRange(const QList<QPointF> &panelrect) const
{
    if ( panelrect.count() < 4 )
        return finFigureObjectAxis::getDefaultRangeRect();

    double xcandrange[2], ycandrange[2];

    double posary[4];
    for ( int i = 0; i < 4; i++ )
        posary[i] = panelrect.at(i).x();
    xcandrange[0] = this->getSecondMinNum(posary, 4);
    xcandrange[1] = this->getSecondMaxNum(posary, 4);

    for ( int i = 0; i < 4; i++ )
        posary[i] = panelrect.at(i).y();
    ycandrange[0] = this->getSecondMinNum(posary, 4);
    ycandrange[1] = this->getSecondMaxNum(posary, 4);

    if ( !this->isAutoRangeX() && xcandrange[0] <= this->_maxX && xcandrange[1] >= this->_minX ) {
        if ( xcandrange[0] < this->_minX )
            xcandrange[0] = this->_minX;
        if ( xcandrange[1] > this->_maxX )
            xcandrange[1] = this->_maxX;
    }

    if ( !this->isAutoRangeY() && ycandrange[0] <= this->_maxY && ycandrange[1] >= this->_minY ) {
        if ( ycandrange[0] < this->_minY )
            ycandrange[0] = this->_minY;
        if ( ycandrange[1] > this->_maxY )
            ycandrange[1] = this->_maxY;
    }

    return QRectF(xcandrange[0], ycandrange[0],
                  (xcandrange[1] - xcandrange[0]), (ycandrange[1] - ycandrange[0]));
}

double finFigureObjectAxis::getGivenAxisCrossPosition(double minnum, double maxnum) const
{
    if ( minnum < 0.0 && maxnum < 0.0 )
        return maxnum;
    else if ( minnum > 0.0 && maxnum > 0.0 )
        return minnum;
    else
        return 0.0;
}

QPointF finFigureObjectAxis::getAxisCrossPoint(const QList<QPointF> &panelrect) const
{
    QRectF candrect = getCrossPointRange(panelrect);

    double ptx = this->getGivenAxisCrossPosition(candrect.left(), candrect.right());
    double pty = this->getGivenAxisCrossPosition(candrect.top(), candrect.bottom());
    printf("CP:(%lf,%lf)\n", ptx,pty);fflush(stdout);
    return QPointF(ptx, pty);
}

QRectF finFigureObjectAxis::getAxisDrawRange(const QList<QPointF> &panelrect, const QPointF& crosspt) const
{
    finFigAlgLine2D xline = finFigureAlg::horLineFromYVal(crosspt.y());
    QList<QPointF> xrngpt = finFigureAlg::polygonCrossPoint(xline, panelrect);
    if ( xrngpt.count() < 2 )
        return finFigureObjectAxis::getDefaultRangeRect();

    double xrng[2];
    xrng[0] = (xrngpt.at(0).x() < xrngpt.at(1).x() ? xrngpt.at(0).x() : xrngpt.at(1).x());
    xrng[1] = (xrngpt.at(0).x() < xrngpt.at(1).x() ? xrngpt.at(1).x() : xrngpt.at(0).x());
    printf("%lf, %lf\n", xrng[0], xrng[1]);
    for ( int i = 0; i < xrngpt.count(); i++ ) {
        if ( xrngpt.at(i).x() < xrng[0] )
            xrng[0] = xrngpt.at(i).x();
        if ( xrngpt.at(i).x() > xrng[1] )
            xrng[1] = xrngpt.at(i).x();
    }
    if ( !this->isAutoRangeX() && xrng[0] <= this->_maxX && xrng[1] >= this->_minX ) {
        if ( xrng[0] < this->_minX )
            xrng[0] = this->_minX;
        if ( xrng[1] > this->_maxX )
            xrng[1] = this->_maxX;
    }

    finFigAlgLine2D yline = finFigureAlg::vecLineFromXVal(crosspt.x());
    QList<QPointF> yrngpt = finFigureAlg::polygonCrossPoint(yline, panelrect);
    if ( yrngpt.count() < 2 )
        return finFigureObjectAxis::getDefaultRangeRect();

    double yrng[2];
    yrng[0] = (yrngpt.at(0).y() < yrngpt.at(1).y() ? yrngpt.at(0).y() : xrngpt.at(1).y());
    yrng[1] = (yrngpt.at(0).y() < yrngpt.at(1).y() ? yrngpt.at(1).y() : xrngpt.at(0).y());
    for ( int i = 0; i < yrngpt.count(); i++ ) {
        if ( yrngpt.at(i).y() < yrng[0] )
            yrng[0] = yrngpt.at(i).y();
        if ( yrngpt.at(i).y() > yrng[1] )
            yrng[1] = yrngpt.at(i).y();
    }
    if ( !this->isAutoRangeY() && yrng[0] <= this->_maxY && yrng[1] >= this->_minY ) {
        if ( yrng[0] < this->_minY )
            yrng[0] = this->_minY;
        if ( yrng[1] > this->_maxY )
            yrng[1] = this->_maxY;
    }
    printf("Range:x:%lf,%lf  y:%lf,%lf\n", xrng[0], xrng[1], yrng[0], yrng[1]);fflush(stdout);
    return QRectF(xrng[0], yrng[0], (xrng[1] - xrng[0]), (yrng[1] - yrng[0]));
}

finErrorCode finFigureObjectAxis::getLinesPath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                                               const QPointF &crosspt, const QRectF &drawrange) const
{
    finErrorCode errcode;
    finFigureObjectLine foline;
    this->_figCfg.cloneFigureConfig(foline.getFigureConfig());

    foline.setPoint1(drawrange.left(), crosspt.y());
    foline.setPoint2(drawrange.right(), crosspt.y());
    errcode = foline.getPixelFigurePath(pathlist, cfg);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    foline.setPoint1(crosspt.x(), drawrange.top());
    foline.setPoint2(crosspt.x(), drawrange.bottom());
    errcode = foline.getPixelFigurePath(pathlist, cfg);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

double finFigureObjectAxis::getTickStep(bool isx, const QPointF &crosspt, finGraphConfig *cfg) const
{
    QPointF unitvec;
    if ( isx ) {
        if ( !this->isAutoStepX() )
            return this->getStepX();
        else
            unitvec = QPointF(1.0, 0.0);
    } else {
        if ( !this->isAutoRangeY() )
            return this->getStepY();
        else
            unitvec = QPointF(0.0, 1.0);
    }

    QPointF xpixpt = cfg->transformPixelPoint(crosspt);
    static const double bestpixsteplen = 32.0;

    int cnt = 0;
    QPointF steppt = crosspt + unitvec;
    QPointF steppixpt = cfg->transformPixelPoint(steppt);
    double length = finFigureAlg::pointsDistance(steppixpt, xpixpt);

    if ( length < bestpixsteplen ) {
        int i = 2, stepi = 1;
        while ( cnt < 100 ) {
            steppt = crosspt + i * unitvec;
            steppixpt = cfg->transformPixelPoint(steppt);
            length = finFigureAlg::pointsDistance(steppixpt, xpixpt);
            if ( length >= bestpixsteplen )
                return (double)i;

            stepi = i * (bestpixsteplen - length) / (1.75 * length);
            i += (stepi < 1 ? 1 : stepi);
            cnt++;
        }
    } else if ( length > 2 * bestpixsteplen ) {
        double step = 0.75, prevstep = 1.0;
        while ( cnt < 100 ) {
            steppt = crosspt + step * unitvec;
            steppixpt = cfg->transformPixelPoint(steppt);
            length = finFigureAlg::pointsDistance(steppixpt, xpixpt);
            if ( length == bestpixsteplen )
                return step;
            else if ( length < bestpixsteplen )
                return prevstep;

            prevstep = step;
            step *= bestpixsteplen / length;
            cnt++;
        }
    } else {
        return 1.0;
    }
    return 1.0;
}

QPointF finFigureObjectAxis::getStepPixelVector(const QPointF &steppt, const QPointF &crosspt,
                                                finGraphConfig *cfg) const
{
    QPointF steppixpt = cfg->transformPixelPoint(steppt);
    QPointF xpixpt = cfg->transformPixelPoint(crosspt);
    return steppixpt - xpixpt;
}

QPointF finFigureObjectAxis::getTickPixelVector(const QPointF &steppixvec) const
{
    double ticksize = 3.0 + this->_figCfg.getDotSize() / 2.0;
    QPointF tickvec = finFigureAlg::getVerticalVector(steppixvec, ticksize);
    if ( steppixvec.x() > 0 )
        return tickvec;
    else
        return -tickvec;
}

double finFigureObjectAxis::getAxisPixelRadian(const QPointF &steppixvec) const
{
    return finFigureAlg::getVectorRadian(steppixvec);
}

finErrorCode finFigureObjectAxis::setupTickLabel(const QPointF &steppixvec, finFigureObjectText *fotext) const
{
    finFigureConfig *textcfg = fotext->getFigureConfig();
    this->_figCfg.cloneFigureConfig(textcfg);

    QFont lblfont = textcfg->getFont();
    double basefontsize = lblfont.pointSizeF();
    lblfont.setPointSizeF(basefontsize / 1.5);
    textcfg->setFont(lblfont);

    double marginval = basefontsize / 3.0;
    QMarginsF margins = QMargins(marginval * 1.5, marginval, marginval * 1.5, marginval);
    textcfg->setTextMargins(margins);

    double xrad = -finFigureAlg::getVectorRadian(steppixvec);
    const double cutbs = M_PI / 8.0;

    if ( xrad > -cutbs && xrad < cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignHCenter | Qt::AlignTop);
        fotext->setRadian(xrad);
    } else if ( (xrad >= cutbs && xrad <= 3.0 * cutbs) || (xrad >= -7.0 * cutbs && xrad <= -5.0 * cutbs) ) {
        fotext->setFontMetricFlags(Qt::AlignLeft | Qt::AlignTop);
        fotext->setRadian(0.0);
    } else if ( xrad > 3.0 * cutbs && xrad < 4.0 * cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignLeft | Qt::AlignVCenter);
        fotext->setRadian(xrad - M_PI / 2.0);
    } else if ( xrad >= 4.0 * cutbs && xrad < 5.0 * cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignRight | Qt::AlignVCenter);
        fotext->setRadian(xrad - M_PI / 2.0);
    } else if ( (xrad >= 5.0 * cutbs && xrad <= 7.0 * cutbs) || (xrad >= -3.0 * cutbs && xrad <= -cutbs) ) {
        fotext->setFontMetricFlags(Qt::AlignRight | Qt::AlignTop);
        fotext->setRadian(0.0);
    } else if ( xrad > 7.0 * cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignHCenter | Qt::AlignTop);
        fotext->setRadian(xrad - M_PI);
    } else if ( xrad >= -4.0 * cutbs && xrad < -3.0 * cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignRight | Qt::AlignVCenter);
        fotext->setRadian(xrad + M_PI / 2.0);
    } else if ( xrad > -5.0 * cutbs && xrad < -4.0 * cutbs ) {
        fotext->setFontMetricFlags(Qt::AlignLeft | Qt::AlignVCenter);
        fotext->setRadian(xrad + M_PI / 2.0);
    } else /*if ( xrad < -7.0 * cutbs )*/ {
        fotext->setFontMetricFlags(Qt::AlignHCenter | Qt::AlignTop);
        fotext->setRadian(xrad + M_PI);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::getTickPath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                                              const QPointF &crosspt, const QRectF &drawrange) const
{
    QPainterPath path;
    QPainterPath textpath;
    finFigureObjectText fotext;

    double xstep = this->getTickStep(true, crosspt, cfg);
    QPointF xstepvec = QPointF(xstep, 0.0);
    QPointF xsteppixvec = this->getStepPixelVector(crosspt + xstepvec, crosspt, cfg);
    QPointF xtickvec = this->getTickPixelVector(xsteppixvec);
    this->setupTickLabel(xsteppixvec, &fotext);

    for ( double x = crosspt.x() + xstep; x < drawrange.right(); x += xstep ) {
        QPointF tickpt = QPointF(x, crosspt.y());
        QPointF tickpixpt = cfg->transformPixelPoint(tickpt);

        path.moveTo(tickpixpt);
        path.lineTo(tickpixpt + xtickvec);

        fotext.setBasePoint(tickpt);
        fotext.setText(QString::number(x, 'g', 6));
        textpath.addPath(fotext.getPixelTextPath(cfg));
    }

    for ( double x = crosspt.x() - xstep; x > drawrange.left(); x -= xstep ) {
        QPointF tickpt = QPointF(x, crosspt.y());
        QPointF tickpixpt = cfg->transformPixelPoint(tickpt);

        path.moveTo(tickpixpt);
        path.lineTo(tickpixpt + xtickvec);

        fotext.setBasePoint(tickpt);
        fotext.setText(QString::number(x, 'g', 6));
        textpath.addPath(fotext.getPixelTextPath(cfg));
    }

    double ystep = this->getTickStep(false, crosspt, cfg);
    QPointF ystepvec = QPointF(0.0, ystep);
    QPointF ysteppixvec = this->getStepPixelVector(crosspt + ystepvec, crosspt, cfg);
    QPointF ytickvec = this->getTickPixelVector(ysteppixvec);
    this->setupTickLabel(ysteppixvec, &fotext);

    for ( double y = crosspt.y() + ystep; y < drawrange.bottom(); y += ystep ) {
        QPointF tickpt = QPointF(crosspt.x(), y);
        QPointF tickpixpt = cfg->transformPixelPoint(tickpt);

        path.moveTo(tickpixpt);
        path.lineTo(tickpixpt + ytickvec);

        fotext.setBasePoint(tickpt);
        fotext.setText(QString::number(y, 'g', 6));
        textpath.addPath(fotext.getPixelTextPath(cfg));
    }

    for ( double y = crosspt.y() - ystep; y > drawrange.top(); y -= ystep ) {
        QPointF tickpt = QPointF(crosspt.x(), y);
        QPointF tickpixpt = cfg->transformPixelPoint(tickpt);

        path.moveTo(tickpixpt);
        path.lineTo(tickpixpt + ytickvec);

        fotext.setBasePoint(tickpt);
        fotext.setText(QString::number(y, 'g', 6));
        textpath.addPath(fotext.getPixelTextPath(cfg));
    }

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    finFigurePath textfigpath;
    textfigpath.setPen(this->_figCfg.getTextPen());
    textfigpath.setBrush(this->_figCfg.getTextBrush());
    textfigpath.setPath(textpath);
    pathlist->append(textfigpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

QPainterPath finFigureObjectAxis::getAxisTitlePath(const QPointF &axisstartpt, const QPointF &axisendpt,
                                                   const QString &title, finGraphConfig *cfg) const
{
    QPointF axpixpt1 = cfg->transformPixelPoint(axisstartpt);
    QPointF axpixpt2 = cfg->transformPixelPoint(axisendpt);

    QPointF axpixvec = axpixpt2 - axpixpt1;
    double axpixrad = -finFigureAlg::getVectorRadian(axpixvec);
    QPointF txtpixpt = finFigureAlg::movePointInside(axpixpt2, axpixpt1,
                                                     this->_figCfg.getEndArrow().getLength());

    finFigureObjectText fotext;
    finFigureConfig *textcfg = fotext.getFigureConfig();
    this->_figCfg.cloneFigureConfig(textcfg);

    QFont lblfont = textcfg->getFont();
    lblfont.setBold(true);
    lblfont.setItalic(true);
    textcfg->setFont(lblfont);

    double marginval = lblfont.pointSizeF() / 3.0;
    QMarginsF margins = QMargins(marginval * 1.5, marginval, marginval * 1.5, marginval);
    textcfg->setTextMargins(margins);

    fotext.setText(title);
    QRectF textrect = fotext.getBoundingRect();

    const double cutbs = M_PI / 8.0;
    if ( axpixrad > -cutbs && axpixrad < cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignRight | Qt::AlignBottom);
        fotext.setRadian(axpixrad);
    } else if ( axpixrad >= cutbs && axpixrad <= 3.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignRight | Qt::AlignBottom);
        fotext.setRadian(0.0);
    } else if ( axpixrad > 3.0 * cutbs && axpixrad < 4.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignRight | Qt::AlignTop);
        fotext.setRadian(axpixrad - M_PI / 2.0);
    } else if ( axpixrad >= 4.0 * cutbs && axpixrad < 5.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignTop);
        fotext.setRadian(axpixrad - M_PI / 2.0);
    } else if ( axpixrad >= 5.0 * cutbs && axpixrad <= 7.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignBottom);
        fotext.setRadian(0.0);
    } else if ( axpixrad > 7.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignBottom);
        fotext.setRadian(axpixrad - M_PI);
    } else if ( axpixrad >= -3.0 * cutbs && axpixrad <= -cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignBottom);
        fotext.setRadian(0.0);
    } else if ( axpixrad >= -4.0 * cutbs && axpixrad < -3.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignBottom);
        fotext.setRadian(axpixrad + M_PI / 2.0);
    } else if ( axpixrad > -5.0 * cutbs && axpixrad < -4.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignRight | Qt::AlignBottom);
        fotext.setRadian(axpixrad + M_PI / 2.0);
    } else if ( axpixrad >= -7.0 * cutbs && axpixrad <= -5.0 * cutbs ) {
        fotext.setFontMetricFlags(Qt::AlignRight | Qt::AlignBottom);
        fotext.setRadian(0.0);
    } else /*if ( axpixrad < -7.0 * cutbs )*/ {
        fotext.setFontMetricFlags(Qt::AlignLeft | Qt::AlignBottom);
        fotext.setRadian(axpixrad + M_PI);
    }

    fotext.setBasePoint(cfg->arcTransformPixelPoint(txtpixpt));
    return fotext.getPixelTextPath(cfg);
}

finErrorCode finFigureObjectAxis::getTitlePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg,
                                               const QPointF &crosspt, const QRectF &drawrange) const
{
    QPointF axisxpt1 = QPointF(drawrange.left(), crosspt.y());
    QPointF axisxpt2 = QPointF(drawrange.right(), crosspt.y());
    QPainterPath xpath = this->getAxisTitlePath(axisxpt1, axisxpt2, this->_titleX, cfg);

    QPointF axisypt1 = QPointF(crosspt.x(), drawrange.top());
    QPointF axisypt2 = QPointF(crosspt.x(), drawrange.bottom());
    QPainterPath ypath = this->getAxisTitlePath(axisypt1, axisypt2, this->_titleY, cfg);

    QPainterPath path;
    path.addPath(xpath);
    path.addPath(ypath);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getTextPen());
    figpath.setBrush(this->_figCfg.getTextBrush());
    figpath.setPath(path);
    pathlist->append(figpath);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    finErrorCode errcode;
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QList<QPointF> polygon = cfg->getCornerAxisPoints();
    QPointF xpt = this->getAxisCrossPoint(polygon);
    QRectF drawrect = this->getAxisDrawRange(polygon, xpt);

    errcode = this->getLinesPath(pathlist, cfg, xpt, drawrect);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->getTickPath(pathlist, cfg, xpt, drawrect);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    errcode = this->getTitlePath(pathlist, cfg, xpt, drawrect);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectAxis::dump() const
{
    printf(" * Fig Type: axis; ");

    printf("X[%s] ", this->_titleX.toLatin1().data());
    if ( this->isAutoRangeX() )
        printf("(AutoRange) ");
    else
        printf("(%lf - %lf) ", this->_minX, this->_maxX);
    if ( this->isAutoStepX() )
        printf(" step:AUTO; ");
    else
        printf(" step:%lf; ", this->_stepX);

    printf("Y[%s] ", this->_titleY.toLatin1().data());
    if ( this->isAutoRangeX() )
        printf("(AutoRange) ");
    else
        printf("(%lf - %lf) ", this->_minY, this->_maxY);
    if ( this->isAutoStepX() )
        printf("stp:AUTO\n");
    else
        printf("stp:%lf\n", this->_stepY);
}

finFigureObjectLine3D::finFigureObjectLine3D()
{
    this->_type = finFigureObject::FIN_FO_TYPE_LINE3D;
    this->_pt1 = finFigurePoint3D(0.0, 0.0, 0.0);
    this->_pt2 = finFigurePoint3D(0.0, 0.0, 0.0);
}

bool finFigureObjectLine3D::is3DFigure() const
{
    return true;
}

finFigurePoint3D finFigureObjectLine3D::getPoint1() const
{
    return this->_pt1;
}

finFigurePoint3D finFigureObjectLine3D::getPoint2() const
{
    return this->_pt2;
}

finErrorCode finFigureObjectLine3D::setPoint1(const finFigurePoint3D &qpt)
{
    this->_pt1 = qpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine3D::setPoint1(double ptx, double pty, double ptz)
{
    this->_pt1.setX(ptx);
    this->_pt1.setY(pty);
    this->_pt1.setZ(ptz);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine3D::setPoint2(const finFigurePoint3D &qpt)
{
    this->_pt2 = qpt;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine3D::setPoint2(double ptx, double pty, double ptz)
{
    this->_pt2.setX(ptx);
    this->_pt2.setY(pty);
    this->_pt2.setZ(ptz);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode
finFigureObjectLine3D::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint3D(this->_pt1));
    path.lineTo(cfg->transformPixelPoint3D(this->_pt2));

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finFigureObjectLine3D::dump() const
{
    printf(" * Fig Type: line3D; (%lf, %lf, %lf) -- (%lf, %lf, %lf)\n",
           this->_pt1.getX(), this->_pt1.getY(), this->_pt1.getZ(),
           this->_pt2.getX(), this->_pt2.getY(), this->_pt2.getZ());
}

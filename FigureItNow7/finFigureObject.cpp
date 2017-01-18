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

finErrorCode finFigureObject::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finFigureObject::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

bool finFigureObject::hasLinePath() const
{
    return false;
}

bool finFigureObject::hasShapePath() const
{
    return false;
}

bool finFigureObject::hasTextPath() const
{
    return false;
}

QPainterPath finFigureObject::getLinePath()
{
    return QPainterPath();
}

QPainterPath finFigureObject::getPixelLinePath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    return QPainterPath();
}

QPainterPath finFigureObject::getShapePath()
{
    return QPainterPath();
}

QPainterPath finFigureObject::getPixelShapePath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    return QPainterPath();
}

QPainterPath finFigureObject::getTextPath()
{
    return QPainterPath();
}

QPainterPath finFigureObject::getPixelTextPath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    return QPainterPath();
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

finErrorCode finFigureObjectDot::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    path.moveTo(this->_point);
    path.lineTo(this->_point);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

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

finErrorCode finFigureObjectLine::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    path.moveTo(this->_pt1);
    path.lineTo(this->_pt2);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectLine::getPixelFigurePath(QList<finFigurePath> *pathlist, finGraphConfig *cfg) const
{
    if ( pathlist == NULL || cfg == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint(this->_pt1));
    path.lineTo(cfg->transformPixelPoint(this->_pt2));

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

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

finErrorCode finFigureObjectPolyline::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_ptList.count() < 2 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    QPainterPath path;
    path.moveTo(this->_ptList.at(0));
    for ( int i = 1; i < this->_ptList.count(); i++ ) {
        path.lineTo(this->_ptList.at(i));
    }

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setPath(path);
    pathlist->append(figpath);

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

finErrorCode finFigureObjectRect::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    QPointF startptr = this->getUpperLeftPoint();
    path.moveTo(startptr);
    path.lineTo(this->getUpperRightPoint());
    path.lineTo(this->getLowerRightPoint());
    path.lineTo(this->getLowerLeftPoint());
    path.lineTo(startptr);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

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

finErrorCode finFigureObjectPolygon::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_ptList.count() < 2 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;

    QPainterPath path;
    path.moveTo(this->_ptList.last());
    for ( int i = 0; i < this->_ptList.count(); i++ ) {
        path.lineTo(this->_ptList.at(i));
    }

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

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

finErrorCode finFigureObjectEllipse::getFigurePath(QList<finFigurePath> *pathlist) const
{
    if ( pathlist == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    QPainterPath path;
    QPointF startpt = this->getEllipsePointAtRad(0.0);
    static const double drawstep = M_PI / 36.0;

    path.moveTo(startpt);
    for ( double rad = drawstep; rad < 2 * M_PI; rad += drawstep)
        path.lineTo(this->getEllipsePointAtRad(rad));
    path.lineTo(startpt);

    finFigurePath figpath;
    figpath.setPen(this->_figCfg.getBorderPen());
    figpath.setBrush(this->_figCfg.getFillBrush());
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorCodeKits::FIN_EC_SUCCESS;
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

bool finFigureObjectText::hasShapePath() const
{
    return false;
}

bool finFigureObjectText::hasTextPath() const
{
    return true;
}

QPainterPath finFigureObjectText::getTextPath()
{
    QPainterPath path;
    path.addText(0.0, 0.0, this->_figCfg.getFont(), this->_text);

    QRectF bndrect = path.boundingRect();
    QPointF bloff = QPointF(0.0, 0.0);
    if ( this->_flag & Qt::AlignLeft ) {
        bloff.setX(-bndrect.x());
    } else if ( this->_flag & Qt::AlignRight ) {
        bloff.setX(-bndrect.x() - bndrect.width());
    } else if ( this->_flag & Qt::AlignJustify ) {
        bloff.setX(0.0);
    } else {
        bloff.setX(-bndrect.x() - bndrect.width() / 2.0);
    }
    if ( this->_flag &  Qt::AlignTop ) {
        bloff.setY(-bndrect.y() - bndrect.height());
    } else if ( this->_flag & Qt::AlignBottom ) {
        bloff.setY(-bndrect.y());
    } else if ( this->_flag & Qt::AlignBaseline ) {
        bloff.setY(0.0);
    } else {
        bloff.setY(-bndrect.y() - bndrect.height() / 2.0);
    }
    path.translate(bloff);

    QTransform trans, subtrans1, subtrans2;
    trans.scale(this->_scale, 1.0);
    subtrans1.rotateRadians(this->_rad);
    trans *= subtrans1;
    subtrans2.translate(this->_basePtr.x(), this->_basePtr.y());
    trans *= subtrans2;

    return trans.map(path);
}

QPainterPath finFigureObjectText::getPixelTextPath(finGraphConfig *cfg)
{
    QPainterPath path;
    path.addText(0.0, 0.0, this->_figCfg.getFont(), this->_text);

    QRectF bndrect = path.boundingRect();
    QPointF bloff = QPointF(0.0, 0.0);
    if ( this->_flag & Qt::AlignLeft ) {
        bloff.setX(-bndrect.x());
    } else if ( this->_flag & Qt::AlignRight ) {
        bloff.setX(-bndrect.x() - bndrect.width());
    } else if ( this->_flag & Qt::AlignJustify ) {
        bloff.setX(0.0);
    } else {
        bloff.setX(-bndrect.x() - bndrect.width() / 2.0);
    }
    if ( this->_flag &  Qt::AlignTop ) {
        bloff.setY(-bndrect.y());
    } else if ( this->_flag & Qt::AlignBottom ) {
        bloff.setY(-bndrect.y() - bndrect.height());
    } else if ( this->_flag & Qt::AlignBaseline ) {
        bloff.setY(0.0);
    } else {
        bloff.setY(-bndrect.y() - bndrect.height() / 2.0);
    }
    path.translate(bloff);

    QTransform trans, subtrans1, subtrans2;
    trans.scale(this->_scale, 1.0);
    subtrans1.rotateRadians(-this->_rad);
    trans *= subtrans1;
    QPointF pixbp = cfg->transformPixelPoint(this->_basePtr);
    subtrans2.translate(pixbp.x(), pixbp.y());
    trans *= subtrans2;

    return trans.map(path);
}

void finFigureObjectText::dump() const
{
    printf(" * Fig Type: text; C: (%lf, %lf) F: %d S: %lf rad: %lf T: \"%s\"\n",
           this->_basePtr.x(), this->_basePtr.y(), this->_flag, this->_scale, this->_rad,
           this->_text.toLatin1().data());
}

finFigureObjectAxis::finFigureObjectAxis()
{
    this->_minX = 0.0;
    this->_maxX = 0.0;
    this->_minY = 0.0;
    this->_maxY = 0.0;
    this->_stepX = 0.0;
    this->_stepY = 0.0;
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
    this->_minX = 0.0;
    this->_maxX = 0.0;
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
    this->_minY = 0.0;
    this->_maxY = 0.0;
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
    this->_stepX = 0.0;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setStepX(double step)
{
    this->_stepX = step;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finFigureObjectAxis::setAutoStepY()
{
    this->_stepY = 0.0;
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

bool finFigureObjectLine3D::hasShapePath() const
{
    return true;
}

bool finFigureObjectLine3D::hasTextPath() const
{
    return false;
}

QPainterPath finFigureObjectLine3D::getPixelShapePath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint3D(this->_pt1));
    path.lineTo(cfg->transformPixelPoint3D(this->_pt2));
    return path;
}

void finFigureObjectLine3D::dump() const
{
    printf(" * Fig Type: line3D; (%lf, %lf, %lf) -- (%lf, %lf, %lf)\n",
           this->_pt1.getX(), this->_pt1.getY(), this->_pt1.getZ(),
           this->_pt2.getX(), this->_pt2.getY(), this->_pt2.getZ());
}

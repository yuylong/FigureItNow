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

QPainterPath finFigureObject::getPath()
{
    return QPainterPath();
}

QPainterPath finFigureObject::getPixelPath(finGraphConfig *cfg)
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

QPainterPath finFigureObjectDot::getPath()
{
    QPainterPath path;
    path.moveTo(this->_point);
    path.lineTo(this->_point);
    return path;
}

QPainterPath finFigureObjectDot::getPixelPath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint(this->_point));
    path.lineTo(cfg->transformPixelPoint(this->_point));
    return path;
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

QPainterPath finFigureObjectLine::getPath()
{
    QPainterPath path;
    path.moveTo(this->_pt1);
    path.lineTo(this->_pt2);
    return path;
}

QPainterPath finFigureObjectLine::getPixelPath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    QPainterPath path;
    path.moveTo(cfg->transformPixelPoint(this->_pt1));
    path.lineTo(cfg->transformPixelPoint(this->_pt2));
    return path;
}

void finFigureObjectLine::dump() const
{
    printf(" * Fig Type: line; (%lf, %lf) -- (%lf, %lf)\n",
           (double)this->_pt1.x(), (double)this->_pt1.y(), (double)this->_pt2.x(), (double)this->_pt2.y());
}

finFigureObjectLine3D::finFigureObjectLine3D()
{
    this->_type = finFigureObject::FIN_FO_TYPE_LINE3D;
    this->_pt1 = finFigurePoint3D(0.0, 0.0, 0.0);
    this->_pt2 = finFigurePoint3D(0.0, 0.0, 0.0);
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

QPainterPath finFigureObjectLine3D::getPixelPath(finGraphConfig *cfg)
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

finFigureObjectRect::finFigureObjectRect()
{
    this->_type = finFigureObject::FIN_FO_TYPE_RECT;
    this->_center = QPointF(0.0, 0.0);
    this->_size = QSizeF(0.0, 0.0);
    this->_radian = 0.0;
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

QPainterPath finFigureObjectRect::getPath()
{
    QPainterPath path;
    QPointF startptr = this->getUpperLeftPoint();
    path.moveTo(startptr);
    path.lineTo(this->getUpperRightPoint());
    path.lineTo(this->getLowerRightPoint());
    path.lineTo(this->getLowerLeftPoint());
    path.lineTo(startptr);
    return path;
}

QPainterPath finFigureObjectRect::getPixelPath(finGraphConfig *cfg)
{
    if ( cfg == NULL )
        return QPainterPath();

    QPainterPath path;
    QPointF startptr = cfg->transformPixelPoint(this->getUpperLeftPoint());
    path.moveTo(startptr);
    path.lineTo(cfg->transformPixelPoint(this->getUpperRightPoint()));
    path.lineTo(cfg->transformPixelPoint(this->getLowerRightPoint()));
    path.lineTo(cfg->transformPixelPoint(this->getLowerLeftPoint()));
    path.lineTo(startptr);
    return path;
}

void finFigureObjectRect::dump() const
{
    printf(" * Fig Type: rect; C: (%lf, %lf) S: %lf x %lf rad: %lf\n",
           (double)this->_center.x(), (double)this->_center.y(),
           (double)this->_size.width(), (double)this->_size.height(), this->_radian);
}

finFigureObjectEllipse::finFigureObjectEllipse()
{
    this->_type = finFigureObject::FIN_FO_TYPE_ELLIPSE;
    this->_center = QPointF(0.0, 0.0);
    this->_longR = 0.0;
    this->_shortR = 0.0;
    this->_radian = 0.0;
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

QPointF finFigureObjectEllipse::getEllipsePointAtRad(double rad)
{
    QPointF shiftpt = QPointF(this->_longR * cos(rad), this->_shortR * sin(rad));
    QPointF relpt = QPointF(shiftpt.x() * this->_cosrad + shiftpt.y() * this->_sinrad,
                            shiftpt.x() * this->_sinrad - shiftpt.y() * this->_cosrad);
    return this->_center + relpt;
}

QPainterPath finFigureObjectEllipse::getPath()
{
    QPainterPath path;
    QPointF startpt = this->getEllipsePointAtRad(0.0);
    static const double drawstep = M_PI / 36.0;

    path.moveTo(startpt);
    for ( double rad = drawstep; rad < 2 * M_PI; rad += drawstep)
        path.lineTo(this->getEllipsePointAtRad(rad));
    path.lineTo(startpt);
    return path;
}

QPainterPath finFigureObjectEllipse::getPixelPath(finGraphConfig *cfg)
{
    QPainterPath path;
    QPointF startpt = cfg->transformPixelPoint(this->getEllipsePointAtRad(0.0));
    static const double drawstep = M_PI / 36.0;

    path.moveTo(startpt);
    for ( double rad = drawstep; rad < 2 * M_PI; rad += drawstep)
        path.lineTo(cfg->transformPixelPoint(this->getEllipsePointAtRad(rad)));
    path.lineTo(startpt);
    return path;
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
    rad = rad - floor(rad / (2 *M_PI)) * 2 * M_PI;
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

QPainterPath finFigureObjectText::getPath()
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

QPainterPath finFigureObjectText::getPixelPath(finGraphConfig *cfg)
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
    printf("* Fig Type: rect; C: (%lf, %lf) F: %d S: %lf rad: %lf T: \"%s\"\n",
           this->_basePtr.x(), this->_basePtr.y(), this->_flag, this->_scale, this->_rad,
           this->_text.toLatin1().data());
}

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
 */

#ifndef FINFIGUREOBJECT_H
#define FINFIGUREOBJECT_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QPainterPath>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finFigurePoint3D.h"
#include "finGraphConfig.h"

class finFigureObject
{
public:
    enum finFigureObjectType {
        FIN_FO_TYPE_DUMMY,
        FIN_FO_TYPE_DOT,
        FIN_FO_TYPE_LINE,
        FIN_FO_TYPE_LINE3D,
        FIN_FO_TYPE_RECT,
        FIN_FO_TYPE_POLYLINE,
        FIN_FO_TYPE_POLYGON,
        FIN_FO_TYPE_ELLIPSE,
        FIN_FO_TYPE_TEXT,
        FIN_FO_TYPE_AXIS,
        FIN_FO_TYPE_MAX
    };

protected:
    finFigureObjectType _type;
    finFigureConfig _figCfg;

public:
    finFigureObject();
    virtual ~finFigureObject() { return; }

    finFigureObjectType getFigureType() const;
    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();

    virtual QPainterPath getPath();
    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};

typedef finFigureObject::finFigureObjectType finFigureObjectType;

class finFigureObjectDot : public finFigureObject
{
protected:
    QPointF _point;

public:
    finFigureObjectDot();
    virtual ~finFigureObjectDot() { return; }

    QPointF getPoint() const;

    finErrorCode setPoint(const QPointF &qpt);
    finErrorCode setPoint(double ptx, double pty);

    virtual QPainterPath getPath();
    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};

class finFigureObjectLine : public finFigureObject
{
protected:
    QPointF _pt1, _pt2;

public:
    finFigureObjectLine();
    virtual ~finFigureObjectLine() { return; }

    QPointF getPoint1() const;
    QPointF getPoint2() const;

    finErrorCode setPoint1(const QPointF &qpt);
    finErrorCode setPoint1(double ptx, double pty);
    finErrorCode setPoint2(const QPointF &qpt);
    finErrorCode setPoint2(double ptx, double pty);

    virtual QPainterPath getPath();
    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};

class finFigureObjectLine3D : public finFigureObject
{
protected:
    finFigurePoint3D _pt1, _pt2;

public:
    finFigureObjectLine3D();
    virtual ~finFigureObjectLine3D() { return; }

    finFigurePoint3D getPoint1() const;
    finFigurePoint3D getPoint2() const;

    finErrorCode setPoint1(const finFigurePoint3D &qpt);
    finErrorCode setPoint1(double ptx, double pty, double ptz);
    finErrorCode setPoint2(const finFigurePoint3D &qpt);
    finErrorCode setPoint2(double ptx, double pty, double ptz);

    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};


class finFigureObjectRect : public finFigureObject
{
protected:
    QPointF _center;
    QSizeF _size;
    double _radian;
    double _sinrad, _cosrad;

public:
    finFigureObjectRect();
    virtual ~finFigureObjectRect() { return; }

    QPointF getCenterPoint() const;
    QSizeF getSize() const;
    double getRadian() const;
    QPointF getUpperLeftPoint() const;
    QPointF getUpperRightPoint() const;
    QPointF getLowerLeftPoint() const;
    QPointF getLowerRightPoint() const;

    finErrorCode setCenterPoint(const QPointF &ctrpt);
    finErrorCode setCenterPoint(double cx, double cy);
    finErrorCode setSize(const QSizeF &size);
    finErrorCode setSize(double width, double height);
    finErrorCode setRadian(double rad);

    virtual QPainterPath getPath();
    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};

class finFigureObjectEllipse : public finFigureObject
{
protected:
    QPointF _center;
    double _longR, _shortR;
    double _radian;
    double _sinrad, _cosrad;

public:
    finFigureObjectEllipse();
    virtual ~finFigureObjectEllipse() { return; }

    QPointF getCenterPoint() const;
    double getLongRadius() const;
    double getShortRadius() const;
    double getRadian() const;

    finErrorCode setCenterPoint(const QPointF &ctrpt);
    finErrorCode setCenterPoint(double cx, double cy);
    finErrorCode setLongRadius(double lr);
    finErrorCode setShortRadius(double sr);
    finErrorCode setRadian(double rad);

    QPointF getEllipsePointAtRad(double rad);

    virtual QPainterPath getPath();
    virtual QPainterPath getPixelPath(finGraphConfig *cfg);

    virtual void dump() const;
};

class finFigureObjectText : public finFigureObject
{
protected:
    QPointF _basePtr;
    int _flag;
    double _scale;
    double _rad;
    double _sinrad, _cosrad;

    QString _text;

public:
    finFigureObjectText();
    virtual ~finFigureObjectText() { return; }

    QPointF getBasePoint() const;
    int getFontMetricFlags() const;
    double getScale() const;
    double getRadian() const;
    QString getText() const;

    finErrorCode setBasePoint(const QPointF &pt);
    finErrorCode setBasePoint(double ptx, double pty);
    finErrorCode setFontMetricFlags(int flag);
    finErrorCode setScale(double scale);
    finErrorCode setRadian(double rad);
    finErrorCode setText(const QString &text);

    virtual void dump() const;
};

#endif // FINFIGUREOBJECT_H

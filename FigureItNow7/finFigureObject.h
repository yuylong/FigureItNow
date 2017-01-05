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

#include "finErrorCode.h"
#include "finFigureConfig.h"

class finFigureObject
{
public:
    enum finFigureObjectType {
        FIN_FO_TYPE_DUMMY,
        FIN_FO_TYPE_DOT,
        FIN_FO_TYPE_LINE,
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

    virtual void dump() const;
};

class finFigureObjectRect : public finFigureObject
{
protected:
    QPointF _center;
    QSizeF _size;
    double _radian;

public:
    finFigureObjectRect();
    virtual ~finFigureObjectRect() { return; }

    QPointF getCenterPoint() const;
    QSizeF getSize() const;
    double getRadian() const;

    finErrorCode setCenterPoint(const QPointF &ctrpt);
    finErrorCode setSize(const QSizeF &size);
    finErrorCode setRadian(double rad);

    virtual void dump() const;
};

#endif // FINFIGUREOBJECT_H

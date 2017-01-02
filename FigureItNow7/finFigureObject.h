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
        FIN_FO_TYPE_MAX
    };

protected:
    finFigureObjectType _type;
    finFigureConfig _figCfg;

public:
    finFigureObject();

    finFigureObjectType getFigureType() const;
    const finFigureConfig *getFigureConfig() const;
    finFigureConfig *getFigureConfig();
};

typedef finFigureObject::finFigureObjectType finFigureObjectType;

class finFigureObjectDot : public finFigureObject
{
protected:
    QPointF _point;

public:
    finFigureObjectDot();

    QPointF getPoint() const;

    finErrorCode setPoint(const QPointF &qpt);
    finErrorCode setPoint(double ptx, double pty);
};

class finFigureObjectLine : public finFigureObject
{
protected:
    QPointF _pt1, _pt2;

public:
    finFigureObjectLine();
};

#endif // FINFIGUREOBJECT_H

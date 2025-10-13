/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGUREPOINT3D_H
#define FINFIGUREPOINT3D_H

#include "finErrorCode.h"


class finFigurePoint3D
{
protected:
    double _x, _y, _z;

public:
    finFigurePoint3D();
    finFigurePoint3D(const finFigurePoint3D &pt);
    finFigurePoint3D(double x, double y, double z);

    double getX() const;
    double getY() const;
    double getZ() const;

    void setX(double x);
    void setY(double y);
    void setZ(double z);

    finFigurePoint3D operator + (const finFigurePoint3D &pt);
    finFigurePoint3D operator - (const finFigurePoint3D &pt);
    finFigurePoint3D operator * (double scale);
    finFigurePoint3D operator / (double scale);
    finFigurePoint3D &operator = (const finFigurePoint3D &pt);
};

#endif // FINFIGUREPOINT3D_H

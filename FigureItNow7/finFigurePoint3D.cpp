/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finFigurePoint3D.h"

finFigurePoint3D::finFigurePoint3D()
{
    this->_x = 0;
    this->_y = 0;
    this->_z = 0;
}

finFigurePoint3D::finFigurePoint3D(const finFigurePoint3D &pt)
{
    this->_x = pt._x;
    this->_y = pt._y;
    this->_z = pt._z;
}

finFigurePoint3D::finFigurePoint3D(double x, double y, double z)
{
    this->_x = x;
    this->_y = y;
    this->_z = z;
}

double finFigurePoint3D::getX() const
{
    return this->_x;
}

double finFigurePoint3D::getY() const
{
    return this->_y;
}

double finFigurePoint3D::getZ() const
{
    return this->_z;
}

finErrorCode finFigurePoint3D::setX(double x)
{
    this->_x = x;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigurePoint3D::setY(double y)
{
    this->_y = y;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigurePoint3D::setZ(double z)
{
    this->_z = z;
    return finErrorKits::EC_SUCCESS;
}

finFigurePoint3D finFigurePoint3D::operator + (const finFigurePoint3D &pt)
{
    return finFigurePoint3D(this->_x + pt._x, this->_y + pt._y, this->_z + pt._z);
}

finFigurePoint3D finFigurePoint3D::operator - (const finFigurePoint3D &pt)
{
    return finFigurePoint3D(this->_x - pt._x, this->_y - pt._y, this->_z - pt._z);
}

finFigurePoint3D finFigurePoint3D::operator * (double scale)
{
    return finFigurePoint3D(this->_x * scale, this->_y * scale, this->_z * scale);
}

finFigurePoint3D finFigurePoint3D::operator / (double scale)
{
    return finFigurePoint3D(this->_x / scale, this->_y / scale, this->_z / scale);
}

finFigurePoint3D &finFigurePoint3D::operator = (const finFigurePoint3D &pt)
{
    this->_x = pt._x;
    this->_y = pt._y;
    this->_z = pt._z;
    return *this;
}

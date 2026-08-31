/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finFigurePoint3D.cpp
 *  \brief Implementations of the lightweight three-dimensional point value.
 *
 *  Implements coordinate initialization and access plus component-wise point arithmetic and scalar
 *  scaling for three-dimensional figure geometry.
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

void finFigurePoint3D::setX(double x)
{
    this->_x = x;
}

void finFigurePoint3D::setY(double y)
{
    this->_y = y;
}

void finFigurePoint3D::setZ(double z)
{
    this->_z = z;
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

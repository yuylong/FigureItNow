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

finFigureObjectLine::finFigureObjectLine()
{
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

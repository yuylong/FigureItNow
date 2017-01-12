/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 2nd, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170102    0  Yulong Yu    Create this file.
 */

#include "finGraphConfig.h"

#include <qmath.h>

finGraphConfig::finGraphConfig()
{
    this->_unitPixelSize = 72.0;
    this->_panelSize = QSizeF(640.0 / this->_unitPixelSize, 480.0 / this->_unitPixelSize);
    this->_bgColor = QColor(Qt::white);

    this->_originPoint = QPointF(320.0 / this->_unitPixelSize, 240.0 / this->_unitPixelSize);
    this->_axisUnitSize = 20.0;

    this->_enableAxisZ = false;
    this->_axisRadZ = (-3/4) * M_PI;
}

QSizeF finGraphConfig::getPanelSize() const
{
    return this->_panelSize;
}

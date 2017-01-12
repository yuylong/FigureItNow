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

#ifndef FINGRAPHCONFIG_H
#define FINGRAPHCONFIG_H

#include <QSizeF>
#include <QPointF>
#include <QColor>

#include "finErrorCode.h"
#include "finFigureConfig.h"
#include "finGraphTrans.h"

class finGraphConfig
{
protected:
    double _unitPixelSize;
    QSizeF _panelSize;
    QColor _bgColor;

    QPointF _originPoint;
    double _axisUnitSize;

    bool _enableAxisZ;
    double _axisRadZ;

    finGraphTrans *_transform;

public:
    finGraphConfig();

    QSizeF getPanelSize() const;


};

#endif // FINGRAPHCONFIG_H

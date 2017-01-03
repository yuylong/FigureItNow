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

finGraphConfig::finGraphConfig()
{
    this->_panelSize = QSizeF(640.0, 480.0);
    this->_startPos = QPointF(-20.0, -15.0);
    this->_endPos = QPointF(20.0, 15.0);
    this->_bgColor = QColor(Qt::white);
}


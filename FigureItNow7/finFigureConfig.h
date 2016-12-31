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

#ifndef FINFIGURECONFIG_H
#define FINFIGURECONFIG_H

#include <QString>

#include "finErrorCode.h"

class finFigureConfig
{
public:
    enum figFigureEdgeShape {
        FIN_ES_ROUND,
        FIN_ES_RECT_FULL,
        FIN_ES_RECT_CUT
    };

protected:
    double _dotSizeX;
    double _dotSizeY;
    figFigureEdgeShape _edgeShape;

    bool _isBoundTransparent;
    bool _isFillTransparent;
    double _boundColor[3];
    double _fillColor[3];
    quint32 _linePattern;

    QString _fontName;
    double _fontSize;

public:
    finFigureConfig();
};

typedef finFigureConfig::figFigureEdgeShape finFigureEdgeShape;

#endif // FINFIGURECONFIG_H

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
    enum finFigureEndShape {
        FIN_ES_ROUND,
        FIN_ES_RECT_FULL,
        FIN_ES_RECT_CUT
    };

    enum finFigureColorIdx {
        FIN_CI_RED = 0,
        FIN_CI_GREEN = 1,
        FIN_CI_BLUE = 2,
        FIN_CI_TRANSPARENCY = 3,
        FIN_CI_MAX
    };

protected:
    double _dotSizeX;
    double _dotSizeY;
    finFigureEndShape _endShape;

    double _borderColor[FIN_CI_MAX];
    double _fillColor[FIN_CI_MAX];
    quint32 _linePattern;

    QString _fontName;
    double _fontSize;

    static finFigureConfig *_defFigCfg;

public:
    finFigureConfig();

    double getDotSizeX() const;
    double getDotSizeY() const;
    finFigureEndShape getEndShape() const;

    bool isBorderTransparent() const;
    bool isFillTransparent() const;
    double getBorderColorSingle(finFigureColorIdx coloridx) const;
    double getFillColorSingle(finFigureColorIdx coloridx) const;
};

typedef finFigureConfig::finFigureEndShape finFigureEndShape;
typedef finFigureConfig::finFigureColorIdx finFigureColorIdx;

#endif // FINFIGURECONFIG_H

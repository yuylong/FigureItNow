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
 * 20170103    1  Yulong Yu    Change to use Qt data structures.
 */

#ifndef FINFIGURECONFIG_H
#define FINFIGURECONFIG_H

#include <QString>
#include <QSizeF>
#include <QColor>
#include <QFont>

#include "finErrorCode.h"

class finFigureConfig
{
public:
    enum finFigureEndShape {
        FIN_ES_ROUND,
        FIN_ES_RECT_FULL,
        FIN_ES_RECT_CUT
    };

protected:
    QSizeF _dotSize;
    finFigureEndShape _endShape;

    QColor _borderColor;
    QColor _fillColor;
    quint32 _linePattern;

    QFont _font;

    static finFigureConfig *_defFigCfg;

public:
    finFigureConfig();

    QSizeF getDotSize() const;
    double getDotSizeX() const;
    double getDotSizeY() const;
    finFigureEndShape getEndShape() const;

    QColor getBorderColor() const;
    QColor getFillColor() const;
    quint32 getLinePattern() const;

    QFont getFont() const;

    finErrorCode setDotSize(const QSizeF &size);
    finErrorCode setDotSizeX(double sizex);
    finErrorCode setDotSizeY(double sizey);
    finErrorCode setEndShape(finFigureEndShape endshape);

    finErrorCode setBorderColor(const QColor &brcolor);
    finErrorCode setFillColor(const QColor &filcolor);
    finErrorCode setLinePattern(quint32 lnpat);

    finErrorCode setFont(const QFont &font);

    finErrorCode cloneFigureConfig(finFigureConfig *outcfg) const;

    static finFigureConfig *getDefaultFigureConfig();
    static finErrorCode releaseDefaultFigureConfig();
    static finErrorCode cloneFromDefaultFigureConfig(finFigureConfig *outfig);
};

typedef finFigureConfig::finFigureEndShape finFigureEndShape;

#endif // FINFIGURECONFIG_H

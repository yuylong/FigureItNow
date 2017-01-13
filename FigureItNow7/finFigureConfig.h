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
#include <QPen>
#include <QBrush>
#include <QFont>

#include "finErrorCode.h"

class finFigureConfig
{
protected:
    QPen _borderPen;
    QBrush _fillBrush;
    QFont _font;

    static finFigureConfig *_defFigCfg;

public:
    finFigureConfig();

    finErrorCode cloneFigureConfig(finFigureConfig *outcfg) const;

    QPen getBorderPen() const;
    QBrush getFillBrush() const;
    QFont getFont() const;

    double getDotSize() const;
    QColor getBorderColor() const;

    finErrorCode setBorderPen(const QPen &pen);
    finErrorCode setFillBrush(const QBrush &brush);
    finErrorCode setFont(const QFont &font);

    finErrorCode setDotSize(double size);
    finErrorCode setBorderColor(const QColor &color);

    static finFigureConfig *getDefaultFigureConfig();
    static finErrorCode releaseDefaultFigureConfig();
    static finErrorCode cloneFromDefaultFigureConfig(finFigureConfig *outfig);
};

#endif // FINFIGURECONFIG_H

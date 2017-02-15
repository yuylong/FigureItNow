/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 */

#ifndef FINFIGURECONFIG_H
#define FINFIGURECONFIG_H

#include <QString>
#include <QSizeF>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMarginsF>

#include "finErrorCode.h"
#include "finFigureArrow.h"


class finFigureConfig
{
protected:
    QPen _borderPen;
    QBrush _fillBrush;
    finFigureArrow _startArrow;
    finFigureArrow _endArrow;

    QPen _textPen;
    QBrush _textBrush;
    QFont _font;
    QMarginsF _textMargins;

    static finFigureConfig *_defFigCfg;

public:
    finFigureConfig();

    finErrorCode cloneFigureConfig(finFigureConfig *outcfg) const;

    const QPen &getBorderPen() const;
    const QBrush &getFillBrush() const;
    const finFigureArrow &getStartArrow() const;
    const finFigureArrow &getEndArrow() const;
    const QPen &getTextPen() const;
    const QBrush &getTextBrush() const;
    const QFont &getFont() const;
    const QMarginsF &getTextMargins() const;

    double getDotSize() const;
    QColor getBorderColor() const;
    QColor getFillColor() const;
    finFigureArrowType getStartArrowType() const;
    double getStartArrowSize() const;
    double getStartArrowRadian() const;
    finFigureArrowType getEndArrowType() const;
    double getEndArrowSize() const;
    double getEndArrowRadian() const;

    QString getFontName() const;
    double getFontPointSize() const;
    bool getFontBold() const;
    bool getFontItalic() const;
    QColor getFontColor() const;

    finErrorCode setBorderPen(const QPen &pen);
    finErrorCode setFillBrush(const QBrush &brush);
    finErrorCode setStartArrow(const finFigureArrow &arrow);
    finErrorCode setEndArrow(const finFigureArrow &arrow);
    finErrorCode setTextPen(const QPen &pen);
    finErrorCode setTextBrush(const QBrush &brush);
    finErrorCode setFont(const QFont &font);
    finErrorCode setTextMargins(const QMarginsF &margins);

    finErrorCode setDotSize(double size);
    finErrorCode setBorderColor(const QColor &color);
    finErrorCode setFillColor(const QColor &color);
    finErrorCode setStartArrowType(finFigureArrowType type);
    finErrorCode setStartArrowSize(double size);
    finErrorCode setStartArrowRadian(double rad);
    finErrorCode setEndArrowType(finFigureArrowType type);
    finErrorCode setEndArrowSize(double size);
    finErrorCode setEndArrowRadian(double rad);

    finErrorCode setFontName(const QString &ftname);
    finErrorCode setFontPointSize(double size);
    finErrorCode setFontBold(bool bold);
    finErrorCode setFontItalic(bool bold);
    finErrorCode setFontColor(const QColor &color);

    static finFigureConfig *getDefaultFigureConfig();
    static finErrorCode releaseDefaultFigureConfig();
    static finErrorCode cloneFromDefaultFigureConfig(finFigureConfig *outfig);
};

#endif // FINFIGURECONFIG_H

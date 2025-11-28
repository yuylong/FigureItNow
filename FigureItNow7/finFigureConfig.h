/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 31st, 2016
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
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

    void cloneFigureConfig(finFigureConfig *outcfg) const;

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

    void setBorderPen(const QPen &pen);
    void setFillBrush(const QBrush &brush);
    void setStartArrow(const finFigureArrow &arrow);
    void setEndArrow(const finFigureArrow &arrow);
    void setTextPen(const QPen &pen);
    void setTextBrush(const QBrush &brush);
    void setFont(const QFont &font);
    void setTextMargins(const QMarginsF &margins);

    void setDotSize(double size);
    void setBorderColor(const QColor &color);
    void setFillColor(const QColor &color);
    void setStartArrowType(finFigureArrowType type);
    void setStartArrowSize(double size);
    void setStartArrowRadian(double rad);
    void setEndArrowType(finFigureArrowType type);
    void setEndArrowSize(double size);
    void setEndArrowRadian(double rad);

    void setFontName(const QString &ftname);
    void setFontPointSize(double size);
    void setFontBold(bool bold);
    void setFontItalic(bool bold);
    void setFontColor(const QColor &color);

    static finFigureConfig *getDefaultFigureConfig();
    static void cloneFromDefaultFigureConfig(finFigureConfig *outfig);
};

#endif // FINFIGURECONFIG_H

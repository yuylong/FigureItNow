/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finFigureArrow.h
 *  \brief The arrow object installed on figuring objects.
 *
 * This file declares a class referred as arrow to describe the arrow at the line end of the figuring object.
 */

#ifndef FINFIGUREARROW_H
#define FINFIGUREARROW_H

#include <QString>
#include <QList>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QComboBox>

#include "finErrorCode.h"
#include "finFigurePath.h"


class finFigureConfig;
class finGraphConfig;

class finFigureArrow
{
public:
    enum Type {
        TP_NONE,
        TP_TRIANGLE,
    };

    static QString getTypeName(finFigureArrow::Type type);
    static finFigureArrow::Type parseTypeString(const QString &str);
    static finErrorCode fillTypesInComboBox(QComboBox *cmbox);

protected:
    finFigureArrow::Type _type;
    double _rad;
    double _length;

public:
    finFigureArrow();
    finFigureArrow(const finFigureArrow &arrow);

    finFigureArrow::Type getType() const;
    double getRadian() const;
    double getLength() const;

    finErrorCode setType(finFigureArrow::Type type);
    finErrorCode setRadian(double rad);
    finErrorCode setLength(double length);

    finFigureArrow &operator = (const finFigureArrow &arrow);

    QPen getArrowPen(const finFigureConfig *cfg) const;
    QBrush getArrowBrush(const finFigureConfig *cfg) const;

    QPointF lineShrinkPoint(const QPointF &arwpt, const QPointF &prevpt, const finFigureConfig *cfg) const;
    finErrorCode getPixelPath(QList<finFigurePath> *pathlist, const QPointF &arwpt, const QPointF &prevpt,
                              const finFigureConfig *cfg) const;

private:
    double lineShrinkLenTriangle(const finFigureConfig *cfg) const;
    QPointF lineShrinkPtTriangle(const QPointF &prevpt, const QPointF &arwpt, const finFigureConfig *cfg) const;
    finErrorCode getPixelPathTriangle(QList<finFigurePath> *pathlist, const QPointF &arwpt, const QPointF &prevpt,
                                      const finFigureConfig *cfg) const;
};

typedef finFigureArrow::Type finFigureArrowType;

#endif // FINFIGUREARROW_H

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */
/*! \file finFigureAlg.h
 *  \brief The figuring algorithm library.
 *
 * This file declares a static class that contains all the figuring algorithms used in FIN-7 system.
 */

#ifndef FINFIGUREALG_H
#define FINFIGUREALG_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QTransform>


struct finFigAlgLine2D {
    // f(x) = ax + by + c = 0
    double a, b, c;
};

class finFigureAlg
{
public:
    finFigureAlg();

    static double vectorLength(const QPointF &vec);
    static double pointsDistance(const QPointF &pt1, const QPointF &pt2);
    static QPointF movePointInside(const QPointF &basept, const QPointF &dirpt, double len);
    static QPointF movePointOutside(const QPointF &basept, const QPointF &dirpt, double len);

    static double getVectorRadian(const QPointF &vec);
    static double radianDifference(double rad1, double rad2);
    static QPointF getVerticalVector(const QPointF &vec, double len);

    static finFigAlgLine2D vecLineFromXVal(double xval);
    static finFigAlgLine2D horLineFromYVal(double yval);
    static finFigAlgLine2D line2DFromPoints(const QPointF &pt1, const QPointF &pt2);

    static bool isParallelLines(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
    static QPointF lineCrossPoint(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
    static double linePointValue(const finFigAlgLine2D &line, const QPointF &pt);
    static bool arePointsSameSide(const finFigAlgLine2D &line, const QPointF &pt1, const QPointF &pt2);

    static bool isPointBetween(const QPointF &chkpt, const QPointF &pvpt1, const QPointF &pvpt2);
    static bool isPointInsidePolygon(const QPointF &chkpt, const QList<QPointF> &polygon);
    static QList<QPointF> polygonCrossPoint(const finFigAlgLine2D &line, const QList<QPointF> &polygon);
    static QList<QPointF> xMinCutPolygon(const QList<QPointF> &polygon, double xmin);
    static QList<QPointF> xMaxCutPolygon(const QList<QPointF> &polygon, double xmax);
    static QList<QPointF> yMinCutPolygon(const QList<QPointF> &polygon, double ymin);
    static QList<QPointF> yMaxCutPolygon(const QList<QPointF> &polygon, double ymax);

    static bool isPointInsideRect(const QPointF &pt, const QRectF &baserect);
    static bool isRectInsideRect(const QRectF &rect, const QRectF &baserect);
    static bool isPolygonInsideRect(const QList<QPointF> &polygon, const QRectF &baserect);

    static void dumpMatrix(const QTransform &matrix);
    static QTransform threePointMatrix(const QPointF &pt00, const QPointF &pt10, const QPointF &pt01);
    static QTransform threePointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist);
    static QTransform fourPointMatrix(const QPointF &pt00, const QPointF &pt10, const QPointF &pt11,
                                      const QPointF &pt01);
    static QTransform fourPointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist);
};

#endif // FINFIGUREALG_H

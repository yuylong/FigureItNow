#ifndef FINFIGUREALG_H
#define FINFIGUREALG_H

#include <QPointF>
#include <QList>

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
};

#endif // FINFIGUREALG_H

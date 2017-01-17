#ifndef FINFIGUREALG_H
#define FINFIGUREALG_H

#include <QPointF>

struct finFigAlgLine2D {
    // f(x) = ax + by + c = 0
    double a, b, c;
};

class finFigureAlg
{
public:
    finFigureAlg();

    static finFigAlgLine2D line2DFromPoints(const QPointF &pt1, const QPointF &pt2);
    static bool isParallelLines(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
    static QPointF lineCrossPoint(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2);
};

#endif // FINFIGUREALG_H

#include "finFigureAlg.h"

finFigureAlg::finFigureAlg()
{
    /* Do Nothing */
}

finFigAlgLine2D finFigureAlg::line2DFromPoints(const QPointF &pt1, const QPointF &pt2)
{
    finFigAlgLine2D retline;
    retline.a = pt1.y() - pt2.y();
    retline.b = pt2.x() - pt1.x();
    retline.c = pt1.x() * pt2.y() - pt2.x() * pt1.y();
    return retline;
}

bool finFigureAlg::isParallelLines(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2)
{
    double p = line1.a * line2.b - line2.a * line1.b;
    return (p > -1.0e-8 && p < 1.0e-8);
}

QPointF finFigureAlg::lineCrossPoint(const finFigAlgLine2D &line1, const finFigAlgLine2D &line2)
{
    QPointF retpt;
    double p = line1.a * line2.b - line2.a * line1.b;
    retpt.setX((line1.b * line2.c - line2.b * line1.c) / p);
    retpt.setY((line2.a * line1.c - line1.a * line2.c) / p);
    return retpt;
}

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finFigureAlg.h"

#include <qmath.h>

finFigureAlg::finFigureAlg()
{
    /* Do Nothing */
}

double finFigureAlg::vectorLength(const QPointF &vec)
{
    return sqrt(vec.x() * vec.x() + vec.y() * vec.y());
}

double finFigureAlg::pointsDistance(const QPointF &pt1, const QPointF &pt2)
{
    QPointF divpt = pt2 - pt1;
    return sqrt(divpt.x() * divpt.x() + divpt.y() * divpt.y());
}

QPointF finFigureAlg::movePointInside(const QPointF &basept, const QPointF &dirpt, double len)
{
    double linelen = finFigureAlg::pointsDistance(basept, dirpt);
    if ( linelen == 0.0 )
        return basept;

    double ratio = len / linelen;
    QPointF linevec = dirpt - basept;
    QPointF tarvec = linevec * ratio;
    return basept + tarvec;
}

QPointF finFigureAlg::movePointOutside(const QPointF &basept, const QPointF &dirpt, double len)
{
    double linelen = finFigureAlg::pointsDistance(basept, dirpt);
    if ( linelen == 0.0 )
        return basept;

    double ratio = len / linelen;
    QPointF linevec = basept - dirpt;
    QPointF tarvec = linevec * ratio;
    return basept + tarvec;
}

double finFigureAlg::getVectorRadian(const QPointF &vec)
{
    double veclen = finFigureAlg::vectorLength(vec);
    if ( veclen <= 0.0 )
        return 0.0;

    double cosval = vec.x() / veclen;
    if ( cosval >= 1.0 )
        cosval = 1.0;
    else if ( cosval <= -1.0 )
        cosval = -1.0;

    double acosrad = acos(cosval);
    if ( vec.y() >= 0.0 )
        return acosrad;
    else
        return -acosrad;
}

double finFigureAlg::radianDifference(double rad1, double rad2)
{
    double raddiff = rad1 - rad2;
    while ( raddiff > M_PI )
        raddiff -= M_PI * 2.0;
    while ( raddiff <= -M_PI )
        raddiff += M_PI * 2.0;
    return raddiff;
}

QPointF finFigureAlg::getVerticalVector(const QPointF &vec, double len)
{
    double base = len / finFigureAlg::vectorLength(vec);
    double x = vec.y() * base;
    double y = -vec.x() * base;
    return QPointF(x, y);
}

finFigAlgLine2D finFigureAlg::vecLineFromXVal(double xval)
{
    finFigAlgLine2D retline;
    retline.a = 1.0;
    retline.b = 0.0;
    retline.c = -xval;
    return retline;
}

finFigAlgLine2D finFigureAlg::horLineFromYVal(double yval)
{
    finFigAlgLine2D retline;
    retline.a = 0.0;
    retline.b = 1.0;
    retline.c = -yval;
    return retline;
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

double finFigureAlg::linePointValue(const finFigAlgLine2D &line, const QPointF &pt)
{
    return (line.a * pt.x() + line.b * pt.y() + line.c);
}

bool finFigureAlg::arePointsSameSide(const finFigAlgLine2D &line, const QPointF &pt1, const QPointF &pt2)
{
    double pt1val = finFigureAlg::linePointValue(line, pt1);
    double pt2val = finFigureAlg::linePointValue(line, pt2);
    if ( pt1val * pt2val >= 0.0 )
        return true;
    else
        return false;
}

bool finFigureAlg::isPointBetween(const QPointF &chkpt, const QPointF &pvpt1, const QPointF &pvpt2)
{
    QPointF pvvec1 = pvpt2 - pvpt1;
    QPointF chkvec1 = chkpt - pvpt1;
    double chkdot1 = pvvec1.x() * chkvec1.x() + pvvec1.y() * chkvec1.y();
    if ( chkdot1 < 0.0 )
        return false;

    QPointF pvvec2 = pvpt1 - pvpt2;
    QPointF chkvec2 = chkpt - pvpt2;
    double chkdot2 = pvvec2.x() * chkvec2.x() + pvvec2.y() * chkvec2.y();
    if ( chkdot2 < 0.0 )
        return false;

    return true;
}

bool finFigureAlg::isPointInsidePolygon(const QPointF &chkpt, const QList<QPointF> &polygon)
{
    int ptcnt = polygon.count();
    for ( int i = 0; i < ptcnt ; i++ ) {
        QPointF linept1 = polygon.at(i);
        QPointF linept2 = polygon.at((i + 1) % ptcnt);
        finFigAlgLine2D line = finFigureAlg::line2DFromPoints(linept1, linept2);

        int j = 2;
        QPointF pvpt = polygon.at((i + j) % ptcnt);
        while ( fabs(finFigureAlg::linePointValue(line, pvpt)) < 1.0e-8 ) {
            j++;
            if ( j == ptcnt )
                break;
            pvpt = polygon.at((i + j) % ptcnt);
        }

        if ( !finFigureAlg::arePointsSameSide(line, pvpt, chkpt) )
            return false;
    }
    return true;
}

QList<QPointF> finFigureAlg::polygonCrossPoint(const finFigAlgLine2D &line, const QList<QPointF> &polygon)
{
    QList<QPointF> ptlist;

    int ptcnt = polygon.count();
    for ( int i = 0; i < ptcnt ; i++ ) {
        QPointF linept1 = polygon.at(i);
        QPointF linept2 = polygon.at((i + 1) % ptcnt);
        finFigAlgLine2D pline = finFigureAlg::line2DFromPoints(linept1, linept2);

        if ( finFigureAlg::isParallelLines(pline, line) )
            continue;

        QPointF xpt = finFigureAlg::lineCrossPoint(pline, line);
        if ( !finFigureAlg::isPointBetween(xpt, linept1, linept2) )
            continue;

        bool hassame = false;
        for ( int j = 0; j < ptlist.count(); j++ ) {
            const QPointF &prevpt = ptlist.at(j);
            if ( fabs(prevpt.x() - xpt.x()) < 1.0e-8 && fabs(prevpt.y() - xpt.y()) < 1.0e-8 ) {
                hassame = true;
                break;
            }
        }
        if ( hassame )
            continue;

        ptlist.append(xpt);
    }
    return ptlist;
}

QList<QPointF> finFigureAlg::xMinCutPolygon(const QList<QPointF> &polygon, double xmin)
{
    finFigAlgLine2D line = finFigureAlg::vecLineFromXVal(xmin);
    int ptcnt = polygon.count();
    QList<QPointF> newpolygon;

    for ( int i = 0; i < ptcnt; i++ ) {
        const QPointF &pt1 = polygon.at(i);
        const QPointF &pt2 = polygon.at((i + 1) % ptcnt);

        if ( pt1.x() >= xmin )
            newpolygon.append(pt1);

        if ( (pt1.x() - xmin) * (pt2.x() - xmin) < 0.0 ) {
            finFigAlgLine2D ptline = finFigureAlg::line2DFromPoints(pt1, pt2);
            QPointF xpt = finFigureAlg::lineCrossPoint(ptline, line);
            newpolygon.append(xpt);
        }
    }
    return newpolygon;
}

QList<QPointF> finFigureAlg::xMaxCutPolygon(const QList<QPointF> &polygon, double xmax)
{
    finFigAlgLine2D line = finFigureAlg::vecLineFromXVal(xmax);
    int ptcnt = polygon.count();
    QList<QPointF> newpolygon;

    for ( int i = 0; i < ptcnt; i++ ) {
        const QPointF &pt1 = polygon.at(i);
        const QPointF &pt2 = polygon.at((i + 1) % ptcnt);

        if ( pt1.x() <= xmax )
            newpolygon.append(pt1);

        if ( (pt1.x() - xmax) * (pt2.x() - xmax) < 0.0 ) {
            finFigAlgLine2D ptline = finFigureAlg::line2DFromPoints(pt1, pt2);
            QPointF xpt = finFigureAlg::lineCrossPoint(ptline, line);
            newpolygon.append(xpt);
        }
    }
    return newpolygon;
}

QList<QPointF> finFigureAlg::yMinCutPolygon(const QList<QPointF> &polygon, double ymin)
{
    finFigAlgLine2D line = finFigureAlg::horLineFromYVal(ymin);
    int ptcnt = polygon.count();
    QList<QPointF> newpolygon;

    for ( int i = 0; i < ptcnt; i++ ) {
        const QPointF &pt1 = polygon.at(i);
        const QPointF &pt2 = polygon.at((i + 1) % ptcnt);

        if ( pt1.y() >= ymin )
            newpolygon.append(pt1);

        if ( (pt1.y() - ymin) * (pt2.y() - ymin) < 0.0 ) {
            finFigAlgLine2D ptline = finFigureAlg::line2DFromPoints(pt1, pt2);
            QPointF xpt = finFigureAlg::lineCrossPoint(ptline, line);
            newpolygon.append(xpt);
        }
    }
    return newpolygon;
}

QList<QPointF> finFigureAlg::yMaxCutPolygon(const QList<QPointF> &polygon, double ymax)
{
    finFigAlgLine2D line = finFigureAlg::horLineFromYVal(ymax);
    int ptcnt = polygon.count();
    QList<QPointF> newpolygon;

    for ( int i = 0; i < ptcnt; i++ ) {
        const QPointF &pt1 = polygon.at(i);
        const QPointF &pt2 = polygon.at((i + 1) % ptcnt);

        if ( pt1.y() <= ymax )
            newpolygon.append(pt1);

        if ( (pt1.y() - ymax) * (pt2.y() - ymax) < 0.0 ) {
            finFigAlgLine2D ptline = finFigureAlg::line2DFromPoints(pt1, pt2);
            QPointF xpt = finFigureAlg::lineCrossPoint(ptline, line);
            newpolygon.append(xpt);
        }
    }
    return newpolygon;
}

bool finFigureAlg::isPointInsideRect(const QPointF &pt, const QRectF &baserect)
{
    return baserect.contains(pt);
}

bool finFigureAlg::isRectInsideRect(const QRectF &rect, const QRectF &baserect)
{
    return baserect.contains(rect);
}

bool finFigureAlg::isPolygonInsideRect(const QList<QPointF> &polygon, const QRectF &baserect)
{
    for ( int i = 0; i < polygon.count(); i++ ) {
        if ( !finFigureAlg::isPointInsideRect(polygon.at(i), baserect) )
            return false;
    }
    return true;
}

void finFigureAlg::dumpMatrix(const QTransform &matrix)
{
    printf("Matrix = [ %lf, %lf, %lf ]\n", matrix.m11(), matrix.m21(), matrix.m31());
    printf("         [ %lf, %lf, %lf ]\n", matrix.m12(), matrix.m22(), matrix.m32());
    printf("         [ %lf, %lf, %lf ]\n", matrix.m13(), matrix.m23(), matrix.m33());
}

QTransform finFigureAlg::threePointMatrix(const QPointF &pt00, const QPointF &pt10, const QPointF &pt01)
{
    double ma, mb, mc, md, me, mf;

    mc = pt00.x();
    mf = pt00.y();
    ma = pt10.x() - mc;
    md = pt10.y() - mf;
    mb = pt01.x() - mc;
    me = pt01.x() - mf;
    return QTransform(ma, md, 0.0, mb, me, 0.0, mc, mf, 1.0);
}

QTransform finFigureAlg::threePointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist)
{
    if ( fromlist.count() < 3 || tolist.count() < 3 )
        return QTransform();

    QTransform frommatrix, tomatrix;
    frommatrix = finFigureAlg::threePointMatrix(fromlist.at(0), fromlist.at(1), fromlist.at(2));
    tomatrix = finFigureAlg::threePointMatrix(tolist.at(0), tolist.at(1), tolist.at(2));
    return frommatrix.inverted() * tomatrix;
}

QTransform finFigureAlg::fourPointMatrix(const QPointF &pt00, const QPointF &pt10,
                                         const QPointF &pt11, const QPointF &pt01)
{
    double ma, mb, mc, md, me, mf, mg, mh;

    mc = pt00.x();
    mf = pt00.y();
    finFigAlgLine2D ghln1 = { pt10.x() - pt11.x(), pt01.x() - pt11.x(),
                              pt10.x() + pt01.x() - pt11.x() - mc };
    finFigAlgLine2D ghln2 = { pt10.y() - pt11.y(), pt01.y() - pt11.y(),
                              pt10.y() + pt01.y() - pt11.y() - mf };
    QPointF ghvpt = finFigureAlg::lineCrossPoint(ghln1, ghln2);
    mg = ghvpt.x();
    mh = ghvpt.y();

    ma = pt10.x() * (mg + 1.0) - mc;
    mb = pt01.x() * (mh + 1.0) - mc;
    md = pt10.y() * (mg + 1.0) - mf;
    me = pt01.y() * (mh + 1.0) - mf;
    return QTransform(ma, md, mg, mb, me, mh, mc, mf, 1.0);
}

QTransform finFigureAlg::fourPointMatrix(const QList<QPointF> &fromlist, const QList<QPointF> &tolist)
{
    if ( fromlist.count() < 4 || tolist.count() < 4 )
        return QTransform();

    QTransform frommatrix, tomatrix;
    frommatrix = finFigureAlg::fourPointMatrix(fromlist.at(0), fromlist.at(1), fromlist.at(2), fromlist.at(3));
    tomatrix = finFigureAlg::fourPointMatrix(tolist.at(0), tolist.at(1), tolist.at(2), tolist.at(3));
    return frommatrix.inverted() * tomatrix;
}

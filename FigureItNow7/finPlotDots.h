/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINPLOTDOTS_H
#define FINPLOTDOTS_H

#include <QList>
#include <QPointF>

#include "finFigureObject.h"
#include "finFigureContainer.h"


class finPlotDots
{
protected:
    QList<QPointF> _ptList;
    finFigureContainer *_figcontainer;

public:
    finPlotDots();
    virtual ~finPlotDots();

    int getPointCount() const;
    const QPointF &getPointAt(int idx) const;
    finFigureContainer *getFigureContainer() const;

    void appendPoint(const QPointF &pt);
    void appendPoint(double ptx, double pty);
    void appendPoints(const QList<QPointF> &ptlist);
    void prependPoint(const QPointF &pt);
    void prependPoint(double ptx, double pty);
    void prependPoints(const QList<QPointF> &ptlist);
    void clearPoints();

    void setFigureContainer(finFigureContainer *figcontainer);

    virtual finErrorCode plot();
};

class finPlotDotsLine : public finPlotDots
{
public:
    finPlotDotsLine();
    virtual ~finPlotDotsLine();

    virtual finErrorCode plot() override;
};

class finPlotDotsStream : public finPlotDots
{
protected:
    QList<QPointF> _breakPtList;

public:
    finPlotDotsStream();
    virtual ~finPlotDotsStream();

    int getBreakPointCount() const;
    const QPointF &getBreakPointAt(int idx) const;
    bool checkBreakPoint(const QPointF &pt) const;

    void appendBreakPoint(const QPointF &pt);
    void appendBreakPoint(double ptx, double pty);
    void clearBreakPoints();

    virtual finErrorCode plot() override;

private:
    bool checkBreakPointFrom(const QPointF &pt, int fromidx, int *nextidx) const;
    bool checkNaNOrInfPoint(QPointF *pt);
    bool isNanPoint(const QPointF &pt);
};

class finPlotDotsScatter : public finPlotDots
{
protected:
    double _distLimit;

public:
    finPlotDotsScatter();
    virtual ~finPlotDotsScatter();

    double getDistanceLimit() const;
    void setDistanceLimit(double limit);

    virtual finErrorCode plot() override;

private:
    bool isNaNOrInfPoint(const QPointF &pt) const;

    int findNearestPoint(const QPointF &chkpt, const QList<QPointF> &ptlist,
                         int exceptcnt = 0, QPointF *outpt = nullptr);
    int findNearestPointWithRad(const QPointF &chkpt, const QPointF &prevpt, const QList<QPointF> &ptlist,
                                int exceptcnt = 0, QPointF *outpt = nullptr);
    void handleEnclosePoint(const QList<QPointF> &curptlist, const QList<QPointF> &pstptlist,
                            finPlotDotsLine *lnplot);
};

#endif // FINPLOTDOTS_H

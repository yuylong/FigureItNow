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

    finErrorCode appendPoint(const QPointF &pt);
    finErrorCode appendPoint(double ptx, double pty);
    finErrorCode prependPoint(const QPointF &pt);
    finErrorCode prependPoint(double ptx, double pty);
    finErrorCode clearPoints();
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);

    virtual finErrorCode plot();
};

class finPlotDotsLine : public finPlotDots
{
public:
    finPlotDotsLine();
    virtual ~finPlotDotsLine();

    virtual finErrorCode plot();
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

    finErrorCode appendBreakPoint(const QPointF &pt);
    finErrorCode appendBreakPoint(double ptx, double pty);
    finErrorCode clearBreakPoints();

    virtual finErrorCode plot();

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
    finErrorCode setDistanceLimit(double limit);

    virtual finErrorCode plot();

private:
    bool isNaNOrInfPoint(const QPointF &pt) const;

    int findNearestPoint(const QPointF &chkpt, const QList<QPointF> &ptlist,
                         int exceptcnt = 0, QPointF *outpt = NULL);
    int findNearestPointWithRad(const QPointF &chkpt, const QPointF &prevpt, const QList<QPointF> &ptlist,
                                int exceptcnt = 0, QPointF *outpt = NULL);
    finErrorCode handleEnclosePoint(const QList<QPointF> &curptlist, const QList<QPointF> &pstptlist,
                                    finPlotDotsLine *lnplot);
};

#endif // FINPLOTDOTS_H

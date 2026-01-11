/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#include "finPlotDots.h"

#include <qmath.h>

#include "finFigureAlg.h"


finPlotDots::finPlotDots()
    : _ptList()
{
    this->_figcontainer = nullptr;
}

finPlotDots::~finPlotDots()
{
    this->_ptList.clear();
}

int finPlotDots::getPointCount() const
{
    return this->_ptList.count();
}

const QPointF &finPlotDots::getPointAt(int idx) const
{
    static QPointF _defpt(0.0, 0.0);

    if ( idx < 0 || idx >= this->_ptList.count() )
        return _defpt;
    else
        return this->_ptList.at(idx);
}

finFigureContainer *finPlotDots::getFigureContainer() const
{
    return this->_figcontainer;
}

void finPlotDots::appendPoint(const QPointF &pt)
{
    this->_ptList.append(pt);
}

void finPlotDots::appendPoint(double ptx, double pty)
{
    this->_ptList.append(QPointF(ptx, pty));
}

void finPlotDots::appendPoints(const QList<QPointF> &ptlist)
{
    this->_ptList.append(ptlist);
}

void finPlotDots::prependPoint(const QPointF &pt)
{
    this->_ptList.prepend(pt);
}

void finPlotDots::prependPoint(double ptx, double pty)
{
    this->_ptList.prepend(QPointF(ptx, pty));
}

void finPlotDots::prependPoints(const QList<QPointF> &ptlist)
{
    QList<QPointF> tmplist = ptlist;
    tmplist.append(this->_ptList);
    this->_ptList = tmplist;
}

void finPlotDots::clearPoints()
{
    this->_ptList.clear();
}

void finPlotDots::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_figcontainer = figcontainer;
}

finErrorCode finPlotDots::plot()
{
    if ( this->_figcontainer == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    for ( int i = 0; i < this->_ptList.count(); i++ ) {
        finFigureObjectDot *fodot = new finFigureObjectDot();
        if ( fodot == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;

        fodot->setPoint(this->_ptList.at(i));
        finErrorCode errcode = this->_figcontainer->appendFigureObject(fodot);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete fodot;
            return errcode;
        }
    }
    return finErrorKits::EC_SUCCESS;
}

finPlotDotsLine::finPlotDotsLine()
{
    return;
}

finPlotDotsLine::~finPlotDotsLine()
{
    return;
}

finErrorCode finPlotDotsLine::plot()
{
    if ( this->_figcontainer == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finErrorCode errcode;
    if ( this->_ptList.count() == 0 ) {
        return finErrorKits::EC_SUCCESS;
    } else if ( this->_ptList.count() == 1 ) {
        finFigureObjectDot *fodot = new finFigureObjectDot();
        if ( fodot == nullptr )
            return finErrorKits::EC_OUT_OF_MEMORY;

        fodot->setPoint(this->_ptList.first());
        errcode = this->_figcontainer->appendFigureObject(fodot);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete fodot;
            return errcode;
        }
        return finErrorKits::EC_SUCCESS;
    }

    finFigureObjectPolyline *fopln = new finFigureObjectPolyline();
    if ( fopln == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    fopln->setIgnoreArrow(true);
    for ( int i = 0; i < this->_ptList.count(); i++ )
        fopln->appendPoint(this->_ptList.at(i));

    errcode = this->_figcontainer->appendFigureObject(fopln);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fopln;
        return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finPlotDotsStream::finPlotDotsStream()
    : _breakPtList()
{
    return;
}

finPlotDotsStream::~finPlotDotsStream()
{
    return;
}

int finPlotDotsStream::getBreakPointCount() const
{
    return this->_breakPtList.count();
}

const QPointF &finPlotDotsStream::getBreakPointAt(int idx) const
{
    static QPointF _defpt(0.0, 0.0);

    if ( idx < 0 || idx >= this->_breakPtList.count() )
        return _defpt;
    else
    return this->_breakPtList.at(idx);
}

bool finPlotDotsStream::checkBreakPointFrom(const QPointF &pt, int fromidx, int *nextidx) const
{
    if ( fromidx >= this->_breakPtList.count() ) {
        if ( nextidx != nullptr )
            *nextidx = fromidx;
        return false;
    } else if ( fromidx < 0 ) {
        fromidx = 0;
    }

    for ( int idx = fromidx; idx < this->_breakPtList.count(); idx++ ) {
        if ( this->_breakPtList.at(idx) == pt ) {
            if ( nextidx != nullptr )
                *nextidx = idx + 1;
            return true;
        }
    }
    if ( nextidx != nullptr )
        *nextidx = fromidx;
    return false;
}

bool finPlotDotsStream::checkBreakPoint(const QPointF &pt) const
{
    return this->checkBreakPointFrom(pt, 0, nullptr);
}

void finPlotDotsStream::appendBreakPoint(const QPointF &pt)
{
    this->_breakPtList.append(pt);
}

void finPlotDotsStream::appendBreakPoint(double ptx, double pty)
{
    this->_breakPtList.append(QPointF(ptx, pty));
}

void finPlotDotsStream::clearBreakPoints()
{
    this->_breakPtList.clear();
}

bool finPlotDotsStream::checkNaNOrInfPoint(QPointF *pt)
{
    if ( this->isNanPoint(*pt) )
        return true;

    bool isinf = false;
    if ( qIsInf(pt->x()) ) {
        isinf = true;
        pt->setX(pt->x() > 0 ? 65535.0 : -65535.0);
    }
    if ( qIsInf(pt->y()) ) {
        isinf = true;
        pt->setY(pt->y() > 0 ? 65535.0 : -65535.0);
    }
    return isinf;
}

bool finPlotDotsStream::isNanPoint(const QPointF &pt)
{
    return ( qIsNaN(pt.x()) || qIsNaN(pt.y()) || (qIsInf(pt.x()) && qIsInf(pt.y())) );
}

finErrorCode finPlotDotsStream::plot()
{
    if ( this->_figcontainer == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_ptList.count() == 0 )
        return finErrorKits::EC_SUCCESS;

    finPlotDotsLine lnplot;
    lnplot.setFigureContainer(this->_figcontainer);
    lnplot.clearPoints();

    double prevrad = 0.0, currad = 0.0;
    bool isnewline = true;
    int brkptidx = 0;

    for ( int i = 0; i + 1 < this->_ptList.count(); i++ ) {
        QPointF curpt = this->_ptList.at(i);
        prevrad = currad;
        currad = finFigureAlg::getVectorRadian(this->_ptList.at(i + 1) - curpt);

        if ( this->checkBreakPointFrom(curpt, brkptidx, &brkptidx) ) {
            isnewline = true;
        } else if ( this->checkNaNOrInfPoint(&curpt) ) {
            isnewline = true;
        } else if ( !isnewline ) {
            if ( fabs(finFigureAlg::radianDifference(currad, prevrad)) > M_PI * 0.4999 )
                isnewline = true;
            else
                isnewline = false;
        } else {
            isnewline = false;
        }

        if ( !this->isNanPoint(curpt) )
            lnplot.appendPoint(curpt);
        if ( isnewline ) {
            lnplot.plot();
            lnplot.clearPoints();
        }
    }
    if ( !this->isNanPoint(this->_ptList.last()) )
        lnplot.appendPoint(this->_ptList.last());
    lnplot.plot();

    return finErrorKits::EC_SUCCESS;
}

finPlotDotsScatter::finPlotDotsScatter()
{
    this->_distLimit = 0.1;
}

finPlotDotsScatter::~finPlotDotsScatter()
{
    return;
}

double finPlotDotsScatter::getDistanceLimit() const
{
    return this->_distLimit;
}

void finPlotDotsScatter::setDistanceLimit(double limit)
{
    if ( limit < 1.0e-8 )
        limit = 1.0e-8;

    this->_distLimit = limit;
}

bool finPlotDotsScatter::isNaNOrInfPoint(const QPointF &pt) const
{
    if ( qIsNaN(pt.x()) || qIsNaN(pt.y()) || qIsInf(pt.x()) || qIsInf(pt.y()) )
        return true;
    else
        return false;
}

int finPlotDotsScatter::findNearestPoint(const QPointF &chkpt, const QList<QPointF> &ptlist,
                                         int exceptcnt, QPointF *outpt)
{
    QRectF srchrange;
    srchrange.setX(chkpt.x() - this->_distLimit);
    srchrange.setY(chkpt.y() - this->_distLimit);
    srchrange.setSize(QSizeF(2 * this->_distLimit, 2 * this->_distLimit));

    double mindist = this->_distLimit * 2.0;
    int checkcnt = ptlist.count() - exceptcnt;
    int nearestidx = -1;

    for ( int i = 0; i < checkcnt; i++ ) {
        const QPointF &curpt = ptlist.at(i);
        if ( this->isNaNOrInfPoint(curpt) || !srchrange.contains(curpt) )
            continue;

        double curdist = finFigureAlg::pointsDistance(chkpt, curpt);
        if ( curdist < mindist && curpt != chkpt ) {
            mindist = curdist;
            nearestidx = i;
        }
    }
    if ( nearestidx < 0 )
        return -1;

    if ( outpt != nullptr )
        *outpt = ptlist.at(nearestidx);
    return nearestidx;
}

int finPlotDotsScatter::findNearestPointWithRad(const QPointF &chkpt, const QPointF &prevpt,
                                                const QList<QPointF> &ptlist, int exceptcnt, QPointF *outpt)
{
    QRectF srchrange;
    srchrange.setX(chkpt.x() - this->_distLimit);
    srchrange.setY(chkpt.y() - this->_distLimit);
    srchrange.setSize(QSizeF(2 * this->_distLimit, 2 * this->_distLimit));

    double mindist = this->_distLimit * 2.0;
    int checkcnt = ptlist.count() - exceptcnt;
    double chkrad = finFigureAlg::getVectorRadian(chkpt - prevpt);
    int nearestidx = -1;

    for ( int i = 0; i < checkcnt; i++ ) {
        const QPointF &curpt = ptlist.at(i);
        if ( this->isNaNOrInfPoint(curpt) || !srchrange.contains(curpt) )
            continue;

        double currad = finFigureAlg::getVectorRadian(curpt - chkpt);
        if ( fabs(finFigureAlg::radianDifference(currad, chkrad)) > M_PI * 0.4999 )
            continue;

        double curdist = finFigureAlg::pointsDistance(chkpt, curpt);
        if ( curdist < mindist && curpt != chkpt ) {
            mindist = curdist;
            nearestidx = i;
        }
    }
    if ( nearestidx < 0 )
        return -1;

    if ( outpt != nullptr )
        *outpt = ptlist.at(nearestidx);
    return nearestidx;
}

void finPlotDotsScatter::handleEnclosePoint(
        const QList<QPointF> &curptlist, const QList<QPointF> &pstptlist, finPlotDotsLine *lnplot)
{
    if ( curptlist.empty() )
        return;

    const QPointF &lastpt = curptlist.last();
    const QPointF &firstpt = curptlist.first();
    int lastnidx = -1, firstnidx = -1;

    if ( !pstptlist.empty() ) {
        lastnidx = this->findNearestPoint(lastpt, pstptlist);
        if ( lastnidx >= 0 )
            lnplot->appendPoint(pstptlist.at(lastnidx));

        if ( curptlist.count() > 1 ) {
            firstnidx = this->findNearestPoint(firstpt, pstptlist);
            if ( firstnidx >= 0 )
                lnplot->prependPoint(pstptlist.at(firstnidx));
        } else {
            firstnidx = lastnidx;
        }
    }
    if ( curptlist.count() <= 1 )
        return;

    if ( lastnidx < 0 ) {
        const QPointF &prevpt = curptlist.at(curptlist.count() - 2);
        lastnidx = this->findNearestPointWithRad(lastpt, prevpt, curptlist, 2);
        if ( lastnidx >= 0 )
            lnplot->appendPoint(curptlist.at(lastnidx));
    }
    if ( lastnidx == 0 )
        return;

    if ( firstnidx < 0 ) {
        const QPointF &prevpt = curptlist.at(1);
        firstnidx = this->findNearestPointWithRad(firstpt, prevpt, curptlist);
        if ( firstnidx >= 0 )
            lnplot->prependPoint(curptlist.at(firstnidx));
    }
}

finErrorCode finPlotDotsScatter::plot()
{
    if ( this->_figcontainer == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    QList<QPointF> pendpt = this->_ptList;
    QList<QPointF> curptlist;
    QList<QPointF> postpt;
    finPlotDotsLine lnplot;
    lnplot.setFigureContainer(this->_figcontainer);

    while ( !pendpt.empty() ) {
        QPointF curpt = pendpt.takeFirst();
        if ( this->isNaNOrInfPoint(curpt) )
            continue;

        lnplot.appendPoint(curpt);
        curptlist.append(curpt);

        while ( true ) {
            int nearestidx = this->findNearestPoint(curpt, pendpt);
            if ( nearestidx < 0 )
                break;

            curpt = pendpt.takeAt(nearestidx);
            lnplot.appendPoint(curpt);
            curptlist.append(curpt);
        }

        this->handleEnclosePoint(curptlist, postpt, &lnplot);
        lnplot.plot();

        lnplot.clearPoints();
        postpt.append(curptlist);
        curptlist.clear();
    }
    return finErrorKits::EC_SUCCESS;
}


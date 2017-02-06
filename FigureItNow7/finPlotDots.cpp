/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170204    0  Yulong Yu    Create this file.
 * 20170205    1  Yulong Yu    Add scatter plotting class.
 */

#include "finPlotDots.h"

#include <qmath.h>

#include "finFigureAlg.h"


finPlotDots::finPlotDots()
    : _ptList()
{
    this->_figcontainer = NULL;
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

finErrorCode finPlotDots::appendPoint(const QPointF &pt)
{
    this->_ptList.append(pt);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDots::appendPoint(double ptx, double pty)
{
    this->_ptList.append(QPointF(ptx, pty));
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDots::clearPoints()
{
    this->_ptList.clear();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDots::setFigureContainer(finFigureContainer *figcontainer)
{
    this->_figcontainer = figcontainer;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDots::plot()
{
    if ( this->_figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    for ( int i = 0; i < this->_ptList.count(); i++ ) {
        finFigureObjectDot *fodot = new finFigureObjectDot();
        if ( fodot == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        fodot->setPoint(this->_ptList.at(i));
        finErrorCode errcode = this->_figcontainer->appendFigureObject(fodot);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete fodot;
            return errcode;
        }
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    if ( this->_figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finErrorCode errcode;
    if ( this->_ptList.count() == 0 ) {
        return finErrorCodeKits::FIN_EC_SUCCESS;
    } else if ( this->_ptList.count() == 1 ) {
        finFigureObjectDot *fodot = new finFigureObjectDot();
        if ( fodot == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        fodot->setPoint(this->_ptList.first());
        errcode = this->_figcontainer->appendFigureObject(fodot);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete fodot;
            return errcode;
        }
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    finFigureObjectPolyline *fopln = new finFigureObjectPolyline();
    if ( fopln == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    for ( int i = 0; i < this->_ptList.count(); i++ )
        fopln->appendPoint(this->_ptList.at(i));

    errcode = this->_figcontainer->appendFigureObject(fopln);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete fopln;
        return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
        if ( nextidx != NULL )
            *nextidx = fromidx;
        return false;
    } else if ( fromidx < 0 ) {
        fromidx = 0;
    }

    for ( int idx = fromidx; idx < this->_breakPtList.count(); idx++ ) {
        if ( this->_breakPtList.at(idx) == pt ) {
            if ( nextidx != NULL )
                *nextidx = idx + 1;
            return true;
        }
    }
    if ( nextidx != NULL )
        *nextidx = fromidx;
    return false;
}

bool finPlotDotsStream::checkBreakPoint(const QPointF &pt) const
{
    return this->checkBreakPointFrom(pt, 0, NULL);
}

finErrorCode finPlotDotsStream::appendBreakPoint(const QPointF &pt)
{
    this->_breakPtList.append(pt);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDotsStream::appendBreakPoint(double ptx, double pty)
{
    this->_breakPtList.append(QPointF(ptx, pty));
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDotsStream::clearBreakPoints()
{
    this->_breakPtList.clear();
    return finErrorCodeKits::FIN_EC_SUCCESS;
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
    if ( this->_figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_ptList.count() == 0 )
        return finErrorCodeKits::FIN_EC_SUCCESS;

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
            if ( fabs(currad - prevrad) >= M_PI * 0.49998 )
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

    return finErrorCodeKits::FIN_EC_SUCCESS;
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

finErrorCode finPlotDotsScatter::setDistanceLimit(double limit)
{
    if ( limit < 1.0e-8 )
        limit = 1.0e-8;

    this->_distLimit = limit;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotDotsScatter::plot()
{
    if ( this->_figcontainer == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}


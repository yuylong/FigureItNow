#include "finPlotDots.h"

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

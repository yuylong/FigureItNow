#include "finPlotDots.h"

finPlotDots::finPlotDots()
    : _ptList()
{
    this->_figcontainer = NULL;
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

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

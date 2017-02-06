#include "finPlotEquation2D.h"

finPlotEquation2D::finPlotEquation2D()
    : _scrtPlot()
{
    this->_funcname = QString();
    this->_fromX = 0.0;
    this->_toX = 0.0;
    this->_fromY = 0.0;
    this->_toY = 0.0;
    this->_stepX = 0.1;
    this->_stepY = 0.1;
    this->_xidx = 0;
    this->_yidx = 1;
    this->_callArgList = NULL;
    this->_environment = NULL;
    this->_machine = NULL;
    this->_flowctl = NULL;
    this->_scrtPlot.setFigureContainer(NULL);
    this->_scrtPlot.clearPoints();
}

double finPlotEquation2D::getStartPositionX() const
{
    return this->_fromX;
}

double finPlotEquation2D::getEndPostionX() const
{
    return this->_toX;
}

double finPlotEquation2D::getStartPostionY() const
{
    return this->_fromY;
}

double finPlotEquation2D::getEndPostionY() const
{
    return this->_toY;
}

int finPlotEquation2D::getVariableXIndex() const
{
    return this->_xidx;
}

int finPlotEquation2D::getVariableYIndex() const
{
    return this->_yidx;
}

finErrorCode finPlotEquation2D::setFiguringRangeX(double x1, double x2)
{
    if ( x1 <= x2 ) {
        this->_fromX = x1;
        this->_toX = x2;
    } else {
        this->_fromX = x2;
        this->_toX = x1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setFiguringRangeY(double y1, double y2)
{
    if ( y1 <= y2 ) {
        this->_fromY = y1;
        this->_toY = y2;
    } else {
        this->_fromY = y2;
        this->_toY = y1;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setVariableXIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_xidx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finPlotEquation2D::setVariableYIndex(int idx)
{
    if ( idx < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_yidx = idx;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

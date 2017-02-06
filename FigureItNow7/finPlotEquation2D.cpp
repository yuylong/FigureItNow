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

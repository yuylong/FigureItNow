#ifndef FINPLOTEQUATION2D_H
#define FINPLOTEQUATION2D_H

#include <QString>
#include <QList>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecFlowControl.h"
#include "finPlotDots.h"


class finPlotEquation2D
{
protected:
    QString _funcname;
    double _fromX, _toX, _fromY, _toY;
    double _stepX, _stepY;
    int _xidx, _yidx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finPlotDotsScatter _scrtPlot;

public:
    finPlotEquation2D();

    double getStartPositionX() const;
    double getEndPostionX() const;
    double getStartPostionY() const;
    double getEndPostionY() const;
    int getVariableXIndex() const;
    int getVariableYIndex() const;

    finErrorCode setFiguringRangeX(double x1, double x2);
    finErrorCode setFiguringRangeY(double y1, double y2);
    finErrorCode setVariableXIndex(int idx);
    finErrorCode setVariableYIndex(int idx);

};

#endif // FINPLOTEQUATION2D_H

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
    QList<double> _posListX, _posListY;
    int _xidx, _yidx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finPlotDotsScatter _scrtPlot;

public:
    finPlotEquation2D();

    const QString &getFunctionName() const;
    double getStartPositionX() const;
    double getEndPostionX() const;
    double getStartPostionY() const;
    double getEndPostionY() const;
    int getVariableXIndex() const;
    int getVariableYIndex() const;

    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode setFiguringRangeX(double x1, double x2);
    finErrorCode setFiguringRangeY(double y1, double y2);
    finErrorCode setVariableXIndex(int idx);
    finErrorCode setVariableYIndex(int idx);

    QList<finExecVariable *> *getCallArgList() const;
    finExecEnvironment *getEnvironment() const;
    finExecMachine *getMachine() const;
    finExecFlowControl *getFlowControl() const;
    finFigureContainer *getFigureContainer() const;

    finErrorCode setCallArgList(QList<finExecVariable *> *arglist);
    finErrorCode setEnvironment(finExecEnvironment *env);
    finErrorCode setMachine(finExecMachine *machine);
    finErrorCode setFlowControl(finExecFlowControl *flowctl);
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);

    bool checkValid() const;
    finErrorCode plot();

private:
    double getBaseStep() const;
    finErrorCode buildFuncArgList(QList<finExecVariable *> *varlist,
                                  finExecVariable **xvar, finExecVariable **yvar);

    finErrorCode buildSearchPositions(double from, double to, double step, QList<double> *poslist);
    finErrorCode buildSearchRangeList(double step);
    finErrorCode disposeSearchRangeList();

    finErrorCode calcAPoint(double x, double y, finExecFunction *func, QList<finExecVariable *> *varlist,
                            finExecVariable *xvar, finExecVariable *yvar, double *retval, bool *goon);
    finErrorCode checkOnePosition(int xidx, int yidx, double curretval, QList<double> *prevretval,
                                   unsigned long *srchflags);
    finErrorCode updateRetValList(int xidx, int yidx, double curretval, QList<double> *prevretval);

    finErrorCode searchBinary(double x1, double y1, double retval1, double x2, double y2, double retval2,
                              finExecFunction *func, QList<finExecVariable *> *varlist,
                              finExecVariable *xvar, finExecVariable *yvar,
                              double *xret, double *yret, bool *goon, int level = 0);
};

#endif // FINPLOTEQUATION2D_H

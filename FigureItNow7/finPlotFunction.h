#ifndef FINPLOTFUNCTION_H
#define FINPLOTFUNCTION_H

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecVariable.h"
#include "finExecFlowControl.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finPlotDots.h"


class finPlotFunction
{
protected:
    QString _funcname;
    double _fromX, _toX;
    int _xidx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finPlotDotsStream _stmPlot;

public:
    finPlotFunction();

    const QString &getFunctionName() const;
    double getFigureFromPosition() const;
    double getFigureToPosition() const;
    int getIndependentVarIndex() const;

    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode setFigureRange(double x1, double x2);
    finErrorCode setIndependentVarIndex(int idx);

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
    finErrorCode buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **xvar);

    double getCurrentStepWoRad(double basestep) const;
    double getCurrentStep(double rad, double basestep) const;
    finErrorCode calcAPoint(double x, finExecFunction *func, QList<finExecVariable *> *varlist,
                            finExecVariable *xvar, QPointF *pt, bool *goon);
};

#endif // FINPLOTFUNCTION_H

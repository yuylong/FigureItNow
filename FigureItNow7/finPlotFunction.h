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


class finPlotFunction
{
protected:
    QString _funcname;
    double _fromX, _toX;
    double _stepX;
    int _xidx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finFigureContainer *_figcontainer;

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
};

#endif // FINPLOTFUNCTION_H

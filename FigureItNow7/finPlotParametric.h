#ifndef FINPLOTPARAMETRIC_H
#define FINPLOTPARAMETRIC_H

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecVariable.h"
#include "finExecFlowControl.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finPlotDots.h"


class finPlotParametric
{
protected:
    QString _funcname;
    double _fromT, _toT;
    int _tIdx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finPlotDotsStream _stmPlot;

public:
    finPlotParametric();

    const QString &getFunctionName() const;
    double getParameterFromValue() const;
    double getParameterToValue() const;
    int getParameterVarIndex() const;

    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode setParameterValueRange(double t1, double t2);
    finErrorCode setParameterVarIndex(int idx);

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
};

#endif // FINPLOTPARAMETRIC_H

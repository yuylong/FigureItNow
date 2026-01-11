/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

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

    void setFunctionName(const QString &funcname);
    void setParameterValueRange(double t1, double t2);
    void setParameterVarIndex(int idx);

    QList<finExecVariable *> *getCallArgList() const;
    finExecEnvironment *getEnvironment() const;
    finExecMachine *getMachine() const;
    finExecFlowControl *getFlowControl() const;
    finFigureContainer *getFigureContainer() const;

    void setCallArgList(QList<finExecVariable *> *arglist);
    void setEnvironment(finExecEnvironment *env);
    void setMachine(finExecMachine *machine);
    void setFlowControl(finExecFlowControl *flowctl);
    void setFigureContainer(finFigureContainer *figcontainer);

    bool checkValid() const;
    finErrorCode plot();

private:
    double getBaseStep() const;
    finErrorCode buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **tvar);

    double getCurrentStep(double basestep, double detlen, double prevstep);
    finErrorCode calcAPoint(double t, finExecFunction *func, QList<finExecVariable *> *varlist,
                            finExecVariable *tvar, QPointF *pt, bool *goon);

    bool isDuplicatedNaN(double t, const QPointF &prevpt, const QPointF &curpt);
    double calcDetLength(double t, const QPointF &prevpt, const QPointF &curpt);
};

#endif // FINPLOTPARAMETRIC_H

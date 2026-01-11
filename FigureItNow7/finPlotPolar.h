/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

#ifndef FINPLOTPOLAR_H
#define FINPLOTPOLAR_H

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecVariable.h"
#include "finExecFlowControl.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finPlotDots.h"


class finPlotPolar
{
protected:
    QString _funcname;
    double _fromRad, _toRad;
    int _radIdx;

    QList<finExecVariable *> *_callArgList;
    finExecEnvironment *_environment;
    finExecMachine *_machine;

    finExecFlowControl *_flowctl;
    finPlotDotsStream _stmPlot;

public:
    finPlotPolar();

    const QString &getFunctionName() const;
    double getFigureFromRadian() const;
    double getFigureToRadian() const;
    int getRadianVarIndex() const;

    void setFunctionName(const QString &funcname);
    void setFigureRange(double rad1, double rad2);
    void setRadianVarIndex(int idx);

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
    double refineRadianStep(double rad) const;
    double getBaseStep() const;
    finErrorCode buildFuncArgList(QList<finExecVariable *> *varlist, finExecVariable **radvar);

    double getRadianStep(double basestep, double rlen) const;
    finErrorCode calcAPoint(double rad, finExecFunction *func, QList<finExecVariable *> *varlist,
                            finExecVariable *radvar, double *retrlen, QPointF *pt, bool *goon);
};

#endif // FINPLOTPOLAR_H

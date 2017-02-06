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

    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode setFigureRange(double rad1, double rad2);
    finErrorCode setRadianVarIndex(int idx);

};

#endif // FINPLOTPOLAR_H

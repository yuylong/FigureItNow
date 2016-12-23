#ifndef FINEXECOPERARTORCLAC_H
#define FINEXECOPERARTORCLAC_H

#include <QList>

#include "finErrorCode.h"
#include "finLexNode.h"
#include "finExecVariable.h"


class finExecOperartorClac
{
public:
    finExecOperartorClac();

    static finErrorCode execOpCalc(finLexOperatorType optype,
                                   QList<finExecVariable *> *oprands, finExecVariable **retval);
};

#endif // FINEXECOPERARTORCLAC_H

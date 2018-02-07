#ifndef FINEXECALG_H
#define FINEXECALG_H

#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finExecVariable.h"

class finExecAlg
{
public:
    finExecAlg();

    static finErrorCode stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar);

    static finErrorCode csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar);
};

#endif // FINEXECALG_H

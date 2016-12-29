#ifndef FINEXECENVIRONMENT_H
#define FINEXECENVIRONMENT_H

#include <QList>
#include <QString>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finExecVariable.h"
#include "finExecOperartorClac.h"
#include "finFigureContainer.h"

class finExecFunction;

class finExecEnvironment
{
protected:
    QString _envName;

    QList<finExecVariable *> _varList;
    QList<finExecFunction *> _funcList;

    finFigureContainer *_figContainer;
    finExecEnvironment *_prevEnv;

public:
    finExecEnvironment();
    ~finExecEnvironment();

    finErrorCode buildChildEnvironment(finExecEnvironment **chdenv);

    QString getEnvironmentName() const;
    finErrorCode setEnvironmentName(const QString &envname);

    finExecVariable *findVariable(const QString &varname);
    finExecFunction *findFunction(const QString &funcname);

    finErrorCode addVariable(finExecVariable *var);
    finErrorCode addFunction(finExecFunction *func);

    finFigureContainer *getFigureContainer();
    finExecEnvironment *getParentEnvironment();

    finErrorCode getSyntaxNodeValue(finSyntaxNode *synnode, finExecVariable *retval);

private:
    finExecVariable *getVariableHere(const QString &varname);
    finExecFunction *getFunctionHere(const QString &funcname);
};

#endif // FINEXECENVIRONMENT_H

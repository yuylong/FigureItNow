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

    QList<finExecVariable *> _constVars;
    QList<finExecVariable *> _varList;

    QList<finExecFunction *> _funcList;

    finFigureContainer *_figContainer;

    finExecEnvironment *_prevEnv;

public:
    finExecEnvironment();

    finErrorCode getSyntaxNodeValue(finSyntaxNode *synnode, finExecVariable *retval);
};

#endif // FINEXECENVIRONMENT_H

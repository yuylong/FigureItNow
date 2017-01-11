/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 29th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161229    0  Yulong Yu    Create this file.
 * 20161230    1  Yulong Yu    Implement essetial methods.
 */
/*! \file finExecEnvironment.h
 *  \brief The declarations of the runtime environment.
 *
 * This file declares the runtime environmens, which is the container of the runtime current varialbes, constants,
 * functions, and the output figure container. The runtime environment can link one by one to build a parent-child
 * relationship. Such relationship guarantees the lifetime and accessing scopes of variables and functions.
 */

#ifndef FINEXECENVIRONMENT_H
#define FINEXECENVIRONMENT_H

#include <QList>
#include <QString>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finFigureContainer.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;


class finExecEnvironment
{
protected:
    QString _envName;

    QList<finExecVariable *> _varList;
    QList<finExecFunction *> _funcList;

    finExecFunction *_belongFunc;
    finFigureContainer *_figContainer;
    finExecEnvironment *_prevEnv;

    static finExecEnvironment *_rootEnv;

public:
    finExecEnvironment();
    ~finExecEnvironment();

    finErrorCode buildChildEnvironment(finExecEnvironment **chdenv);

    QString getEnvironmentName() const;
    finErrorCode setEnvironmentName(const QString &envname);

    finExecVariable *getVariableHere(const QString &varname);
    finExecFunction *getFunctionHere(const QString &funcname);
    finExecVariable *findVariable(const QString &varname);
    finExecFunction *findFunction(const QString &funcname);
    finExecVariable *findVariableUntil(const QString &varname, const QString &envname);
    finExecFunction *findFunctionUntil(const QString &funcname, const QString &envname);
    finExecVariable *findVariableUntil(const QString &varname, finExecFunction *blngfunc);
    finExecFunction *findFunctionUntil(const QString &funcname, finExecFunction *blngfunc);

    bool isVariableInEnv(finExecVariable *var);
    bool isVariableDirectInEnv(finExecVariable *var);
    bool isFunctionInEnv(finExecFunction *func);

    finErrorCode addVariable(finExecVariable *var);
    finErrorCode addFunction(finExecFunction *func);
    finErrorCode removeVariable(finExecVariable *var);
    finErrorCode removeFunction(finExecFunction *func);

    finExecFunction *getBelongFunctionHere() const;
    finExecFunction *getBelongFunction() const;
    finExecFunction *getPreviousBelongFunction() const;
    finExecFunction *getPreviousBelongFunction(int level) const;
    int getBelongFunctionEnvLevelIdx() const;
    int getPreviousBelongFunctionEnvLevelIdx() const;
    int getPreviousBelongFunctionEnvLevelIdx(int level) const;

    finErrorCode setBelongFunction(finExecFunction *func);

    finFigureContainer *getFigureContainer();
    finExecEnvironment *getParentEnvironment();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setParentEnvironment(finExecEnvironment *prevenv);

    static finErrorCode setupRootEnvironment();
    static finErrorCode disposeRootEnvironment();
    static finExecEnvironment *getRootEnvironment();
    static finErrorCode buildRootChildEnvironment(finExecEnvironment **chdenv);

private:
    int getBelongFuncEvnLevelIn(int curlevel) const;
    int getPrevBelongFuncEnvLevelIn(int level, int curlevel) const;
};

#endif // FINEXECENVIRONMENT_H

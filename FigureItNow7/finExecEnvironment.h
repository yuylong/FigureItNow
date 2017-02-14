/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 29th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
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
    finExecVariable *findVariableUntil(const QString &varname, int envlevel);
    finExecFunction *findFunctionUntil(const QString &funcname, int evnlevel);

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
    int getTotalBelongFunctionLevelCount() const;
    finErrorCode getBelongFunctionList(QStringList *funcnamelist) const;

    finErrorCode setBelongFunction(finExecFunction *func);

    finFigureContainer *getFigureContainer();
    finExecEnvironment *getParentEnvironment();
    finExecEnvironment *getParentEnvironment(int envlevel);
    int getTotalEnvironmentLevelCount();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setParentEnvironment(finExecEnvironment *prevenv);

    static finErrorCode setupRootEnvironment();
    static finErrorCode disposeRootEnvironment();
    static finExecEnvironment *getRootEnvironment();
    static finErrorCode buildRootChildEnvironment(finExecEnvironment **chdenv);

private:
    int getBelongFuncEvnLevelIn(int curlevel) const;
    int getPrevBelongFuncEnvLevelIn(int level, int curlevel) const;
    int getTotalFucnLevelCountIn(int curlevel) const;
    int getTotalEnvLevelCountIn(int curlevel);
};

#endif // FINEXECENVIRONMENT_H

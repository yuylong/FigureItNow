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

    static finExecEnvironment *_rootEnv;

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

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setParentEnvironment(finExecEnvironment *prevenv);

    static finErrorCode setupRootEnvironment();
    static finErrorCode disposeRootEnvironment();
    static finExecEnvironment *getRootEnvironment();
    static finErrorCode buildRootChildEnvironment(finExecEnvironment **chdenv);

private:
    finExecVariable *getVariableHere(const QString &varname);
    finExecFunction *getFunctionHere(const QString &funcname);
};

#endif // FINEXECENVIRONMENT_H

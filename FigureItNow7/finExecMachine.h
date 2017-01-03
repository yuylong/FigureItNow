/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 30th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161230    0  Yulong Yu    Create this file.
 */
/*! \file finExecMachine.h
 *  \brief The declarations of the runtime executor.
 *
 * This file declares the runtime executor. The execution procedure of script is taken here.
 */

#ifndef FINEXECMACHINE_H
#define FINEXECMACHINE_H

#include "finErrorCode.h"
#include "finExecEnvironment.h"
#include "finFigureContainer.h"
#include "finSyntaxReader.h"


class finExecMachine
{
private:
    QString _name;

    finExecEnvironment *_baseEnv;
    finFigureContainer *_baseFigContainer;

    QString _scriptCode;
    bool _isCompiled;
    finSyntaxReader _syntaxRdr;

public:
    finExecMachine();
    finExecMachine(const QString &name);
    ~finExecMachine();

    QString getName() const;
    finExecEnvironment *getBaseEnvironment();
    finFigureContainer *getFigureContainer();
    QString getScriptCode() const;
    QString getCompiledScriptCode() const;

    finErrorCode setName(const QString &name);

    finErrorCode initEnvironment(finExecEnvironment *prevenv);
    finErrorCode initEnvironmentFromRoot();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setScriptCode(const QString &script);

    bool isCompiled() const;
    finErrorCode compile();
    finErrorCode releaseCompile();

    finErrorCode execute();
    static finErrorCode instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar);

private:
    static finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar);
    static finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env, finExecVariable **retvar);

};

#endif // FINEXECMACHINE_H

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

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecEnvironment.h"
#include "finFigureContainer.h"
#include "finLexNode.h"
#include "finSyntaxReader.h"
#include "finSyntaxError.h"


class finExecMachine
{
public:
    enum finFlowControlType {
        FIN_FC_NEXT,
        FIN_FC_RETURN,
        FIN_FC_CONTINUE,
        FIN_FC_BREAK,
        FIN_FC_GOTO,
        FIN_FC_EXIT,
        FIN_FC_ERROR
    };

    struct finFlowControl {
        finFlowControlType _type;
        QString _label;
    };

private:
    QString _name;

    finExecEnvironment *_baseEnv;
    finFigureContainer *_baseFigContainer;

    QString _scriptCode;
    bool _isCompiled;
    finSyntaxReader _syntaxRdr;
    QList<finSyntaxError *> _errList;

public:
    finExecMachine();
    finExecMachine(const QString &name);
    ~finExecMachine();

    QString getName() const;
    finExecEnvironment *getBaseEnvironment();
    finFigureContainer *getFigureContainer();
    QString getScriptCode() const;
    QString getCompiledScriptCode() const;
    int getExecuteErrorCount() const;
    const finSyntaxError *getExecuteErrorAt(int idx) const;

    finErrorCode setName(const QString &name);

    finErrorCode initEnvironment(finExecEnvironment *prevenv);
    finErrorCode initEnvironmentFromRoot();

    finErrorCode setFigureContainer(finFigureContainer *figcontainer);
    finErrorCode setScriptCode(const QString &script);

    bool isCompiled() const;
    finErrorCode compile();
    finErrorCode releaseCompile();

    finErrorCode execute();
    void disposeExecutionError();

    finErrorCode instantExecute(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode appendExecutionError(finLexNode *lexnode, const QString &errinfo);

private:
    finErrorCode instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env,
                                   finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env,
                                  finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env,
                                finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env,
                              finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env,
                               finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecJump(finSyntaxNode *synnode, finExecEnvironment *env,
                              finExecVariable **retvar, finFlowControl *flowctl);
    finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env,
                                 finExecVariable **retvar, finFlowControl *flowctl);

    finErrorCode instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl);
    finErrorCode instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl);
    finErrorCode instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl);
    finErrorCode instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finFlowControl *flowctl);
};

#endif // FINEXECMACHINE_H

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 4th, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#ifndef FINEXECFLOWCONTROL_H
#define FINEXECFLOWCONTROL_H

#include <QString>

#include "finErrorCode.h"
#include "finExecVariable.h"
#include "finLexNode.h"

class finExecMachine;

class finExecFlowControl
{
public:
    enum Type {
        TP_NEXT,
        TP_RETURN,
        TP_CONTINUE,
        TP_BREAK,
        TP_GOTO,
        TP_EXIT,
        TP_ERROR
    };

protected:
    Type _type;
    QString _label;
    finExecVariable *_retVar;

public:
    finExecFlowControl();

    void resetFlowControl();
    finErrorCode copyFlowControl(finExecFlowControl *srcfc);

    Type getType() const;
    bool isFlowNext() const;
    bool isFlowGoto() const;
    QString getGotoLabel() const;
    bool isFlowExit() const;
    finExecVariable *getReturnVariable();
    finExecVariable *pickReturnVariable();

    bool isFlowExpressOk() const;
    bool isFlowProgramOk() const;
    finErrorCode checkFlowForExpress(bool *goon, finLexNode *lexnode, finExecMachine *machine);
    finErrorCode checkFlowForStatement(bool *goon, finLexNode *lexnode, finExecMachine *machine);
    finErrorCode checkFlowForProgram(bool *goon, finLexNode *lexnode, finExecMachine *machine);

    finErrorCode setType(Type type);
    finErrorCode setLabel(const QString &label);
    finErrorCode setFlowNext();
    finErrorCode setGotoAndLabel(const QString &label);
    finErrorCode setReturnVariable(finExecVariable *retvar);

    finErrorCode retVarSwitchEnv(finExecEnvironment *subenv);
    finErrorCode buildLinkedLeftVar();
    void releaseReturnVariable();

    finErrorCode directPass();
};

typedef finExecFlowControl::Type finExecFlowControlType;

#endif // FINEXECFLOWCONTROL_H

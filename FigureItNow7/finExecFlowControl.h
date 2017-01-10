/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yusoft. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170104    0  Yulong Yu    Create this file.
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
    enum finExecFlowControlType {
        FIN_FC_NEXT,
        FIN_FC_RETURN,
        FIN_FC_CONTINUE,
        FIN_FC_BREAK,
        FIN_FC_GOTO,
        FIN_FC_EXIT,
        FIN_FC_ERROR
    };

protected:
    finExecFlowControlType _type;
    QString _label;
    finExecVariable *_retVar;

public:
    finExecFlowControl();

    void resetFlowControl();
    finErrorCode copyFlowControl(finExecFlowControl *srcfc);

    finExecFlowControlType getType() const;
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

    finErrorCode setType(finExecFlowControlType type);
    finErrorCode setLabel(const QString &label);
    finErrorCode setFlowNext();
    finErrorCode setGotoAndLabel(const QString &label);
    finErrorCode setReturnVariable(finExecVariable *retvar);

    finErrorCode retVarSwitchEnv(finExecEnvironment *subenv);
    finErrorCode buildLinkedLeftVar();
    void releaseReturnVariable();

    finErrorCode directPass();
};

typedef finExecFlowControl::finExecFlowControlType finExecFlowControlType;

#endif // FINEXECFLOWCONTROL_H

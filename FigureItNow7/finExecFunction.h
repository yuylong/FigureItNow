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
 */
/*! \file finExecFunction.h
 *  \brief The declarations of the runtime function.
 *
 * This file declares the runtime function. All system functions and user script defined functions share this data
 * structure defined in this file. The system functions are the function predefined in FIN-7 system, which the script
 * can directly cite them without any declaration. The user script defined functions are defined in the script
 * language, and specific among each script.
 */

#ifndef FINEXECFUNCTION_H
#define FINEXECFUNCTION_H

#include <QString>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finExecVariable.h"

class finExecEnvironment;
class finExecFunction;
typedef finErrorCode (*finFunctionCall)(finExecFunction *self, finExecVariable *retval, finExecEnvironment *execenv);

class finExecFunction
{
public:
    enum finExecFunctionType {
        FIN_FN_TYPE_DUMMY,
        FIN_FN_TYPE_USER,
        FIN_FN_TYPE_SYSTEM,
        FIN_FIN_TYPE_MAX
    };

protected:
    finExecFunctionType _type;
    QString _funcName;
    QStringList _paramList;

    union {
        finSyntaxNode *_funcNode;
        finFunctionCall _funcCall;
        void *_rawPointer;
    } _u;

public:
    finExecFunction();

    finExecFunctionType getFunctionType() const;
    QString getFunctionName() const;
    int getParameterCount() const;
    QString getParameterName(int idx) const;

    finErrorCode setFunctionType(finExecFunctionType type);
    finErrorCode setFunctionName(const QString &funcname);
    finErrorCode appendParameterName(const QString &paramname);
    finErrorCode clearParameterNames();

    finErrorCode setFunctionSyntaxNode(finSyntaxNode *funcnode);
    finErrorCode setFunctionCall(finFunctionCall funccall);

    finErrorCode execFunction(finSyntaxNode *curnode, finExecVariable **retval,
                              finExecEnvironment *execenv);

    static finErrorCode installSystemFunctions (finExecEnvironment *rootenv);

private:
    finErrorCode execUserFunction(finExecVariable *retval,
                                  finExecEnvironment *execenv);
    finErrorCode execSysFunction(finExecVariable *retval,
                                 finExecEnvironment *execenv);

};

typedef finExecFunction::finExecFunctionType finExecFunctionType;

#endif // FINEXECFUNCTION_H

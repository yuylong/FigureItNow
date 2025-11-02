/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 29th, 2016
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
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
#include "finExecFlowControl.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;

/*! \typedef finFunctionCall
 *  \brief The prototype style of a system script function implementation.
 *
 * Each system script function is natively implemented in a C++ function with the unified prototype style. The four
 * arguments in the prototye identify the runtime environment, where self is the static infomation of the current
 * function, env is the current layer environment that contains the input arguments of the function, machine is the
 * current execution machine with the code and syntax tree information, and flowctl is used to pass the return value
 * and flow control information. An error code is required to return by such functions.
 */
typedef finErrorCode (*finFunctionCall)(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine,
                                        finExecFlowControl *flowctl);

/*! \struct finExecSysFuncRegItem
 *  \brief the information table when install system script function.
 */
struct finExecSysFuncRegItem {
    QString _funcName;          //!< The function name.
    QString _paramCsvList;      //!< The argument list in comma-splitted format.
    finFunctionCall _funcCall;  //!< The function pointer to the script function implementation.
    QString _category;          //!< The category of this function
    QString _prototype;         //!< The prototype of this function for GUI helper.
    QString _description;       //!< The description.
};

/*! \class finExecFunction
 *  \brief A runtime static information of a script function.
 *
 * This class holds the static information of a script function.
 */
class finExecFunction
{
public:
    /*! \enum finExecFunction::Type
     *  \brief The type of script function.
     */
    enum Type {
        TP_DUMMY,   //!< An invalid scirpt function type.
        TP_USER,    //!< A user-defined script function in a piece of FIN-script code.
        TP_SYSTEM,  //!< A system script function implemented natively in C++ code.
        TP_MAX      //!< The number of script function types.
    };

protected:
    Type _type;              //!< The type of the script function.
    QString _funcName;       //!< The script function name.
    QStringList _paramList;  //!< The argument list of a script function.

    union {
        finSyntaxNode *_funcNode;   //!< The root syntax node of the script function body of a user-defined script
                                    //!< function.
        finFunctionCall _funcCall;  //!< The natively implemented C++ function of a system script function.
        void *_rawPointer;          //!< The raw pointer of the script function body.
    } _u;                    //!< The script function implementation information.

    /*!
     * \brief The prefix of the name an extended argurment.
     *
     * In FIN-script code, the number of arguments in function invocation is allowed to be more than its declaration.
     * In this case, these extra arguments is stored in environment as extended arguments which is named as
     * __ext_arg_0, __ext_arg_1, etc, where '__ext_arg_' is referred as prefix.
     */
    static QString _extArgPrefix;

public:
    finExecFunction();

    Type getFunctionType() const;
    const QString &getFunctionName() const;
    int getParameterCount() const;
    QString getParameterName(int idx) const;
    bool isParameterExist(const QString &paramname) const;

    void setFunctionType(Type type);
    void setFunctionName(const QString &funcname);
    void appendParameterName(const QString &paramname);
    void clearParameterNames();

    void setFunctionSyntaxNode(finSyntaxNode *funcnode);
    void setFunctionCall(finFunctionCall funccall);

    finErrorCode execFunction(finSyntaxNode *argnode, finExecEnvironment *env,
                              finExecMachine *machine, finExecFlowControl *flowctl);
    finErrorCode execFunction(QList<finExecVariable *> *arglist, finExecEnvironment *env,
                              finExecMachine *machine, finExecFlowControl *flowctl);

    static QString getExtArgPrefix();
    static QString getExtArgName(int idx);
    static int getExtendArgCountHere(finExecEnvironment *env);
    static finExecVariable *getExtendArgHereAt(finExecEnvironment *env, int idx);
    static int getExtendArgCount(finExecEnvironment *env);
    static finExecVariable *getExtendArgAt(finExecEnvironment *env, int idx);
    static QList<finExecVariable *> getExtendArgList(finExecEnvironment *env);

    static int getPreviousExtendArgCount(finExecEnvironment *env);
    static finExecVariable *getPreviousExtendArgAt(finExecEnvironment *env, int idx);
    static int getPreviousExtendArgCount(finExecEnvironment *env, int level);
    static finExecVariable *getPreviousExtendArgAt(finExecEnvironment *env, int idx, int level);

    static finErrorCode installSystemFunctions (finExecEnvironment *rootenv);
    static const QList<finExecSysFuncRegItem> &getSysFuncRegList();

private:
    finErrorCode processArgsInSubEnv(finSyntaxNode *argnode, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
    finErrorCode appendArgToSubenv(int idx, finSyntaxNode *argnode, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl);
    finErrorCode processArgsInSubEnv(QList<finExecVariable *> *arglist, finExecEnvironment *env);

    finErrorCode execUserFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl);
    finErrorCode execSysFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl);

    static QList<finExecSysFuncRegItem> _sysFuncList;
    static void registSysFuncAll();

    static void registSysFuncFromArray(finExecSysFuncRegItem *sysfunclist, const QString &category = QString());
    static void registSysFuncMath();
    static void registSysFuncMatrix();
    static void registSysFuncString();
    static void registSysFuncFile();
    static void registSysFuncSystem();
    static void registSysFuncFiguring();
    static void registSysFuncPlot();
};

typedef finExecFunction::Type finExecFunctionType;

#endif // FINEXECFUNCTION_H

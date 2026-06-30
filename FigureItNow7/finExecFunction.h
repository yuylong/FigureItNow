/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 29th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finExecFunction.h
 *  \brief Declarations of runtime function metadata and dispatch helpers.
 *
 *  This header defines finExecFunction, the shared runtime representation used for both user-defined
 *  FIN-script functions and built-in system functions implemented in C++.
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
 *  \brief Unified C++ callback signature for built-in FIN-script functions.
 *
 *  System functions are implemented as native callbacks receiving the static function metadata, the
 *  sub-environment that already contains bound arguments, the active execution machine, and the flow-
 *  control carrier used to return values and propagate control-transfer state.
 */
typedef finErrorCode (*finFunctionCall)(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine,
                                        finExecFlowControl *flowctl);

/*! \struct finExecSysFuncRegItem
 *  \brief Registration record for one built-in system function.
 *
 *  Each record describes the function name, declared parameter list, native callback, and optional
 *  metadata used by GUI helpers and documentation views.
 */
struct finExecSysFuncRegItem {
    QString _funcName;          //!< Script-visible function name.
    QString _paramCsvList;      //!< Declared parameters in comma-separated form.
    finFunctionCall _funcCall;  //!< Native callback implementing the function.
    QString _category;          //!< UI-facing category label for grouping functions.
    QString _prototype;         //!< Human-readable prototype string shown by helpers.
    QString _description;       //!< Human-readable function description.
};

/*! \class finExecFunction
 *  \brief Runtime metadata and dispatcher for a FIN-script function.
 *
 *  finExecFunction stores the static identity of a function, including its name, declared parameters,
 *  and implementation payload. For user-defined functions that payload is a syntax-tree node; for
 *  system functions it is a native C++ callback.
 *
 *  The class also provides helpers to bind actual arguments into a child environment, execute either
 *  the user or system implementation, expose extra arguments beyond the declared parameter list, and
 *  register the built-in system-function catalog into the root environment.
 */
class finExecFunction
{
public:
    /*! \enum finExecFunction::Type
     *  \brief Kinds of runtime function implementation supported by FIN-7.
     */
    enum Type {
        TP_DUMMY,   //!< Invalid or uninitialized function type.
        TP_USER,    //!< User-defined FIN-script function backed by a syntax node.
        TP_SYSTEM,  //!< Built-in function implemented by a native C++ callback.
        TP_MAX      //!< Sentinel marking the end of the enum.
    };

protected:
    Type _type;              //!< Function implementation kind.
    QString _funcName;       //!< Script-visible function name.
    QStringList _paramList;  //!< Declared parameter names in call order.

    union {
        finSyntaxNode *_funcNode;   //!< User-function body node when _type == TP_USER.
        finFunctionCall _funcCall;  //!< Native callback when _type == TP_SYSTEM.
        void *_rawPointer;          //!< Raw view used to clear or switch the union payload.
    } _u;                    //!< Function implementation payload.

    /*!
     * \brief Prefix used when materializing extra positional arguments.
     *
     * FIN-script allows calls with more actual arguments than declared parameters. Those extra arguments
     * are stored in the function sub-environment under synthetic names such as \c __ext_arg_0,
     * \c __ext_arg_1, and so on.
     */
    static QString _extArgPrefix;

public:
    /*! \brief Constructs an uninitialized TP_DUMMY function record. */
    finExecFunction();

    /*! \name Metadata Accessors
     *  \brief Inspect the function's static identity and parameter list.
     */
    ///@{
    Type getFunctionType() const;
    const QString &getFunctionName() const;
    int getParameterCount() const;
    QString getParameterName(int idx) const;
    bool isParameterExist(const QString &paramname) const;
    ///@}

    /*! \name Metadata Mutation
     *  \brief Configure the function type, name, and declared parameters.
     */
    ///@{
    void setFunctionType(Type type);
    void setFunctionName(const QString &funcname);
    void appendParameterName(const QString &paramname);
    void clearParameterNames();

    void setFunctionSyntaxNode(finSyntaxNode *funcnode);
    void setFunctionCall(finFunctionCall funccall);
    ///@}

    /*! \name Execution Entry Points
     *  \brief Bind arguments into a child environment and dispatch the function implementation.
     */
    ///@{
    finErrorCode execFunction(finSyntaxNode *argnode, finExecEnvironment *env,
                              finExecMachine *machine, finExecFlowControl *flowctl);
    finErrorCode execFunction(QList<finExecVariable *> *arglist, finExecEnvironment *env,
                              finExecMachine *machine, finExecFlowControl *flowctl);
    ///@}

    /*! \name Extra-Argument Helpers
     *  \brief Access arguments passed beyond the declared parameter list.
     */
    ///@{
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
    ///@}

    /*! \name System-Function Registry
     *  \brief Install and expose the catalog of built-in system functions.
     */
    ///@{
    static finErrorCode installSystemFunctions (finExecEnvironment *rootenv);
    static const QList<finExecSysFuncRegItem> &getSysFuncRegList();
    ///@}

private:
    /*! \brief Evaluates and binds syntax-tree arguments into a sub-environment. */
    finErrorCode processArgsInSubEnv(finSyntaxNode *argnode, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
    /*! \brief Evaluates one argument expression and binds it into the sub-environment. */
    finErrorCode appendArgToSubenv(int idx, finSyntaxNode *argnode, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl);
    /*! \brief Binds an already-evaluated argument list into a sub-environment. */
    finErrorCode processArgsInSubEnv(QList<finExecVariable *> *arglist, finExecEnvironment *env);

    /*! \brief Executes the user-defined syntax-tree implementation. */
    finErrorCode execUserFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl);
    /*! \brief Executes the native callback implementation of a system function. */
    finErrorCode execSysFunction(finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl);

    static QList<finExecSysFuncRegItem> _sysFuncList;  //!< Process-wide registry of built-in functions.
    /*! \brief Populates the full system-function registry. */
    static void registSysFuncAll();

    /*! \brief Appends registration records from a null-terminated array into the registry. */
    static void registSysFuncFromArray(finExecSysFuncRegItem *sysfunclist, const QString &category = QString());
    /*! \brief Registers built-in math functions. */
    static void registSysFuncMath();
    /*! \brief Registers built-in matrix functions. */
    static void registSysFuncMatrix();
    /*! \brief Registers built-in string functions. */
    static void registSysFuncString();
    /*! \brief Registers built-in file functions. */
    static void registSysFuncFile();
    /*! \brief Registers built-in system/utility functions. */
    static void registSysFuncSystem();
    /*! \brief Registers built-in figure-construction functions. */
    static void registSysFuncFiguring();
    /*! \brief Registers built-in plotting functions. */
    static void registSysFuncPlot();
};

/*! \typedef finExecFunctionType
 *  \brief Shorthand alias for finExecFunction::Type.
 */
typedef finExecFunction::Type finExecFunctionType;

#endif // FINEXECFUNCTION_H

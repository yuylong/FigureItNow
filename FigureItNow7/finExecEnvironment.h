/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 29th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */
/*! \file finExecEnvironment.h
 *  \brief Declarations of the runtime environment and scope-chain container.
 *
 *  This header defines finExecEnvironment, the execution-layer object that stores variables, functions,
 *  figure-output state, and the parent-environment link used to model lexical and call scopes.
 */

#ifndef FINEXECENVIRONMENT_H
#define FINEXECENVIRONMENT_H

#include <QMap>
#include <QString>

#include "finErrorCode.h"
#include "finSyntaxNode.h"
#include "finFigureContainer.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;


/*! \class finExecEnvironment
 *  \brief One layer in the FIN-7 runtime environment chain.
 *
 *  Each finExecEnvironment instance owns the variables and functions defined at one runtime scope and
 *  links to a parent environment through _prevEnv. Together these linked instances form the lookup chain
 *  used by the interpreter for names, function-call ownership, and figure-output propagation.
 *
 *  The class also manages a process-wide root environment that holds predefined system variables and
 *  functions. Script-specific environments are typically created as children of that root.
 *
 *  \see finExecVariable
 *  \see finExecFunction
 */
class finExecEnvironment
{
protected:
    QString _envName;                            //!< Human-readable environment name.

    QMap<QString, finExecVariable *> _varList;   //!< Variables owned directly by this environment.
    QMap<QString, finExecFunction *> _funcList;  //!< Functions owned directly by this environment.

    finExecFunction *_belongFunc;                //!< Function-call owner of this environment, or \c nullptr.
    finFigureContainer *_figContainer;           //!< Figure container shared by the active execution chain.
    finExecEnvironment *_prevEnv;                //!< Parent environment used for scope lookup and nesting.

    static finExecEnvironment *_rootEnv;         //!< Process-wide root environment with system definitions.

public:
    /*!
     *  \brief Constructs an empty environment with no parent, owner function, or figure container.
     */
    finExecEnvironment();

    /*!
     *  \brief Destroys the environment and deletes the variables and functions it owns directly.
     */
    ~finExecEnvironment();

    /*! \name Environment Construction
     *  \brief Create derived environments and configure environment metadata.
     */
    ///@{

    /*! \brief Allocates a child environment linked to this environment.
     *
     *  The child inherits this environment's figure container, starts with no belonging function, and
     *  records this environment as its parent.
     *
     *  \param chdenv  Output pointer that receives the newly allocated child environment.
     *  \return \c finErrorKits::EC_SUCCESS on success, or an error code if allocation fails or
     *          \a chdenv is \c nullptr.
     */
    finErrorCode buildChildEnvironment(finExecEnvironment **chdenv);

    /*! \brief Returns this environment's name. */
    QString getEnvironmentName() const;

    /*! \brief Sets this environment's name. */
    void setEnvironmentName(const QString &envname);
    ///@}

    /*! \name Name Lookup
     *  \brief Read variables and functions from the current environment or its parent chain.
     */
    ///@{

    /*! \brief Returns a variable defined directly in this environment, or \c nullptr if not found. */
    finExecVariable *getVariableHere(const QString &varname);

    /*! \brief Returns a function defined directly in this environment, or \c nullptr if not found. */
    finExecFunction *getFunctionHere(const QString &funcname);

    /*! \brief Searches this environment and its parents for a variable name. */
    finExecVariable *findVariable(const QString &varname);

    /*! \brief Searches this environment and its parents for a function name. */
    finExecFunction *findFunction(const QString &funcname);

    /*! \brief Searches for a variable until an environment with the given name is reached. */
    finExecVariable *findVariableUntil(const QString &varname, const QString &envname);

    /*! \brief Searches for a function until an environment with the given name is reached. */
    finExecFunction *findFunctionUntil(const QString &funcname, const QString &envname);

    /*! \brief Searches for a variable until an environment owned by the given function is reached. */
    finExecVariable *findVariableUntil(const QString &varname, finExecFunction *blngfunc);

    /*! \brief Searches for a function until an environment owned by the given function is reached. */
    finExecFunction *findFunctionUntil(const QString &funcname, finExecFunction *blngfunc);

    /*! \brief Searches for a variable through at most \a envlevel parent hops. */
    finExecVariable *findVariableUntil(const QString &varname, int envlevel);

    /*! \brief Searches for a function through at most \a evnlevel parent hops. */
    finExecFunction *findFunctionUntil(const QString &funcname, int evnlevel);
    ///@}

    /*! \name Ownership Queries
     *  \brief Check whether a variable or function belongs to this environment.
     */
    ///@{

    /*! \brief Returns whether a variable is reachable inside any variable owned by this environment. */
    bool isVariableInEnv(finExecVariable *var);

    /*! \brief Returns whether a variable pointer is stored directly in this environment. */
    bool isVariableDirectInEnv(finExecVariable *var);

    /*! \brief Returns whether a function pointer is stored directly in this environment. */
    bool isFunctionInEnv(finExecFunction *func);
    ///@}

    /*! \name Resource Mutation
     *  \brief Add, remove, and enumerate variables and functions owned by this environment.
     */
    ///@{

    /*! \brief Adds a variable to this environment.
     *
     *  The variable must be non-null, have a non-empty name, and be a left value.
     */
    finErrorCode addVariable(finExecVariable *var);

    /*! \brief Adds a function to this environment. */
    finErrorCode addFunction(finExecFunction *func);

    /*! \brief Removes a variable from this environment without deleting it. */
    finErrorCode removeVariable(finExecVariable *var);

    /*! \brief Removes a function from this environment without deleting it. */
    finErrorCode removeFunction(finExecFunction *func);

    /*! \brief Returns the list of variables owned directly by this environment. */
    QList<finExecVariable *> getAllVariableList();

    /*! \brief Returns the list of functions owned directly by this environment. */
    QList<finExecFunction *> getAllFunctionList();
    ///@}

    /*! \name Function-Call Ownership Introspection
     *  \brief Inspect the function ownership markers attached to the environment chain.
     */
    ///@{

    /*! \brief Returns the function that directly owns this environment, or \c nullptr. */
    finExecFunction *getBelongFunctionHere() const;

    /*! \brief Returns the nearest owning function found in this environment chain. */
    finExecFunction *getBelongFunction() const;

    /*! \brief Returns the previous owning function in the parent chain. */
    finExecFunction *getPreviousBelongFunction() const;

    /*! \brief Returns the previous owning function at the specified function-depth offset. */
    finExecFunction *getPreviousBelongFunction(int level) const;

    /*! \brief Returns the environment-depth index of the nearest owning function. */
    int getBelongFunctionEnvLevelIdx() const;

    /*! \brief Returns the environment-depth index of the previous owning function. */
    int getPreviousBelongFunctionEnvLevelIdx() const;

    /*! \brief Returns the environment-depth index of the previous owning function at \a level. */
    int getPreviousBelongFunctionEnvLevelIdx(int level) const;

    /*! \brief Counts how many owning-function markers exist in this environment chain. */
    int getTotalBelongFunctionLevelCount() const;

    /*! \brief Appends the names of owning functions in this chain to \a funcnamelist. */
    void getBelongFunctionList(QStringList *funcnamelist) const;

    /*! \brief Sets the function that owns this environment. */
    void setBelongFunction(finExecFunction *func);
    ///@}

    /*! \name Figure Container And Parent Chain
     *  \brief Access the shared figure container and traverse the parent environment chain.
     */
    ///@{

    /*! \brief Returns the figure container currently attached to this environment chain. */
    finFigureContainer *getFigureContainer();

    /*! \brief Returns this environment's direct parent, or \c nullptr. */
    finExecEnvironment *getParentEnvironment();

    /*! \brief Returns the ancestor environment at the specified depth. */
    finExecEnvironment *getParentEnvironment(int envlevel);

    /*! \brief Returns the total number of environments reachable from this one, including itself. */
    int getTotalEnvironmentLevelCount();

    /*! \brief Sets the figure container attached to this environment. */
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);

    /*! \brief Sets the direct parent environment. */
    finErrorCode setParentEnvironment(finExecEnvironment *prevenv);
    ///@}

    /*! \name Root Environment Helpers
     *  \brief Manage the process-wide root environment that stores system definitions.
     */
    ///@{

    /*! \brief Allocates and initializes the root environment if it does not already exist. */
    static finErrorCode setupRootEnvironment();

    /*! \brief Destroys the root environment and releases its owned definitions. */
    static finErrorCode disposeRootEnvironment();

    /*! \brief Returns the root environment, creating it lazily if needed. */
    static finExecEnvironment *getRootEnvironment();

    /*! \brief Creates a child environment under the root environment. */
    static finErrorCode buildRootChildEnvironment(finExecEnvironment **chdenv);
    ///@}

private:
    /*! \brief Recursive helper for locating the nearest owning function's environment depth. */
    int getBelongFuncEvnLevelIn(int curlevel) const;

    /*! \brief Recursive helper for locating a previous owning function's environment depth. */
    int getPrevBelongFuncEnvLevelIn(int level, int curlevel) const;

    /*! \brief Recursive helper for counting function-ownership markers in the chain. */
    int getTotalFucnLevelCountIn(int curlevel) const;

    /*! \brief Recursive helper for counting environment levels in the chain. */
    int getTotalEnvLevelCountIn(int curlevel);
};

#endif // FINEXECENVIRONMENT_H

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 4th, 2017
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finExecFlowControl.h
 *  \brief Declarations of the execution-time flow-control state object.
 *
 *  This header defines finExecFlowControl, the small runtime object used to carry control-transfer
 *  state such as \c return, \c break, \c continue, \c goto, and \c exit while the interpreter walks
 *  the syntax tree.
 */

#ifndef FINEXECFLOWCONTROL_H
#define FINEXECFLOWCONTROL_H

#include <QString>

#include "finErrorCode.h"
#include "finExecVariable.h"
#include "finLexNode.h"

class finExecMachine;

/*! \class finExecFlowControl
 *  \brief Runtime carrier for control-transfer state and return values.
 *
 *  finExecFlowControl is passed through expression, statement, and program execution paths so the
 *  interpreter can propagate non-local flow changes without relying on C++ exceptions for ordinary
 *  script control structures. It stores the current flow type, an optional goto label, and an
 *  optional return variable owned or forwarded by the current execution step.
 *
 *  \see finExecMachine
 *  \see finExecVariable
 */
class finExecFlowControl
{
public:
    /*! \enum finExecFlowControl::Type
     *  \brief Kinds of flow state that can be propagated by the execution engine.
     */
    enum Type {
        TP_NEXT,      //!< Normal fall-through; execution may continue locally.
        TP_RETURN,    //!< Function return with an optional return variable.
        TP_CONTINUE,  //!< Loop continue request.
        TP_BREAK,     //!< Loop break request.
        TP_GOTO,      //!< Goto jump request targeting _label.
        TP_EXIT,      //!< Program exit request.
        TP_ERROR      //!< Invalid or unhandled flow state.
    };

protected:
    Type _type;                 //!< Current control-transfer kind.
    QString _label;             //!< Target label used when _type == TP_GOTO.
    finExecVariable *_retVar;   //!< Optional return value carried with TP_RETURN or expression results.

public:
    /*!
     *  \brief Constructs a flow-control object in the default TP_NEXT state.
     */
    finExecFlowControl();

    /*! \name State Reset And Copy
     *  \brief Reset or duplicate flow-control state.
     */
    ///@{

    /*! \brief Resets the flow state to TP_NEXT and releases any owned return variable. */
    void resetFlowControl();

    /*! \brief Copies the state, label, and return-variable pointer from another flow object.
     *
     *  \param srcfc  Source flow-control object to copy from.
     */
    void copyFlowControl(finExecFlowControl *srcfc);
    ///@}

    /*! \name State Inspection
     *  \brief Query the current flow kind and any associated payload.
     */
    ///@{

    /*! \brief Returns the current flow type. */
    Type getType() const;

    /*! \brief Returns whether the current state is TP_NEXT. */
    bool isFlowNext() const;

    /*! \brief Returns whether the current state is TP_GOTO. */
    bool isFlowGoto() const;

    /*! \brief Returns the goto label when the current state is TP_GOTO; otherwise an empty string. */
    QString getGotoLabel() const;

    /*! \brief Returns whether the current state is TP_EXIT. */
    bool isFlowExit() const;

    /*! \brief Returns the current return-variable pointer without transferring ownership. */
    finExecVariable *getReturnVariable();

    /*! \brief Detaches and returns the current return-variable pointer.
     *
     *  After this call, the flow-control object no longer owns the returned pointer.
     */
    finExecVariable *pickReturnVariable();
    ///@}

    /*! \name Flow Validation Helpers
     *  \brief Check whether the current flow state is valid in a specific execution context.
     */
    ///@{

    /*! \brief Returns whether the current state is valid for expression evaluation. */
    bool isFlowExpressOk() const;

    /*! \brief Returns whether the current state is valid at program scope. */
    bool isFlowProgramOk() const;

    /*! \brief Validates the current flow state for an expression-execution site.
     *
     *  \param goon     Optional output flag indicating whether local execution should continue.
     *  \param lexnode  Optional syntax location used when reporting an execution error.
     *  \param machine  Optional execution machine used to record an error for invalid flow states.
     *  \return Success, warning, or failure depending on whether the current flow is acceptable here.
     */
    finErrorCode checkFlowForExpress(bool *goon, finLexNode *lexnode, finExecMachine *machine);

    /*! \brief Validates the current flow state for a statement-execution site. */
    finErrorCode checkFlowForStatement(bool *goon, finLexNode *lexnode, finExecMachine *machine);

    /*! \brief Validates the current flow state for top-level program execution. */
    finErrorCode checkFlowForProgram(bool *goon, finLexNode *lexnode, finExecMachine *machine);
    ///@}

    /*! \name State Mutation
     *  \brief Update the flow type, label, and return variable.
     */
    ///@{

    /*! \brief Sets the current flow type directly. */
    void setType(Type type);

    /*! \brief Sets the goto label payload. */
    void setLabel(const QString &label);

    /*! \brief Resets the flow type to TP_NEXT and clears the goto label. */
    void setFlowNext();

    /*! \brief Sets the flow type to TP_GOTO and stores the target label. */
    void setGotoAndLabel(const QString &label);

    /*! \brief Attaches a return-variable pointer to this flow-control object.
     *
     *  \param retvar  Return variable to store.
     */
    void setReturnVariable(finExecVariable *retvar);
    ///@}

    /*! \name Return-Variable Ownership Helpers
     *  \brief Adjust or release the return variable carried by this flow state.
     */
    ///@{

    /*! \brief Rebuilds the stored return variable so it belongs to a target sub-environment. */
    finErrorCode retVarSwitchEnv(finExecEnvironment *subenv);

    /*! \brief Rebuilds the stored return variable as a linked left variable. */
    finErrorCode buildLinkedLeftVar();

    /*! \brief Releases any non-left return variable currently stored in this object. */
    void releaseReturnVariable();
    ///@}

    /*! \brief No-op pass-through hook used by flow-check helpers.
     *
     *  \return Always returns \c finErrorKits::EC_SUCCESS.
     */
    finErrorCode directPass();
};

/*! \typedef finExecFlowControlType
 *  \brief Shorthand alias for finExecFlowControl::Type.
 */
typedef finExecFlowControl::Type finExecFlowControlType;

#endif // FINEXECFLOWCONTROL_H

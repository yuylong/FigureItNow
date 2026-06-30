/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 30th, 2016
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */
/*! \file finExecMachine.h
 *  \brief Declarations of the FIN-7 runtime execution machine.
 *
 *  This header defines finExecMachine, the object that compiles script text into a syntax tree,
 *  prepares the base execution environment, walks the tree, and records execution diagnostics.
 */

#ifndef FINEXECMACHINE_H
#define FINEXECMACHINE_H

#include <QString>
#include <QList>

#include "finErrorCode.h"
#include "finExecFlowControl.h"
#include "finExecCompiler.h"
#include "finFigureContainer.h"
#include "finLexNode.h"
#include "finSyntaxReader.h"
#include "finSyntaxError.h"
#include "finSyntaxErrorDump.h"
#include "finSyntaxErrorList.h"
#include "finSyntaxTree.h"

class finExecVariable;
class finExecFunction;
class finExecEnvironment;
class finExecMachine;

/*! \class finExecMachine
 *  \brief Tree-walking execution engine for FIN-script programs.
 *
 *  finExecMachine coordinates the full runtime pipeline after parsing: it stores the script text via
 *  finExecCompiler, owns the compiled syntax tree, initializes the base runtime environment, executes
 *  syntax nodes recursively, and records execution-time warnings and errors in finSyntaxErrorList.
 *
 *  The machine executes on top of a base environment and a figure container supplied by the caller.
 *  During execution it uses finExecFlowControl to propagate return values and non-local control-flow
 *  states between nested execution helpers.
 *
 *  \see finExecCompiler
 *  \see finExecEnvironment
 *  \see finExecFlowControl
 */
class finExecMachine
{
protected:
    QString _name;                         //!< Human-readable machine name for diagnostics and UI.

    finExecEnvironment *_baseEnv;          //!< Base environment used as the root of one script execution.
    finFigureContainer *_baseFigContainer; //!< Figure container receiving drawing output.

    finExecCompiler _compiler;             //!< Compiler wrapper that produces the syntax tree.
    finSyntaxTree *_synTree;               //!< Last compiled syntax tree, owned by this machine.
    finSyntaxErrorList _errList;           //!< Execution diagnostics accumulated by this machine.

public:
    /*! \name Construction And Lifetime
     *  \brief Construct and destroy the execution machine.
     */
    ///@{

    /*! \brief Constructs an unnamed execution machine with no environment or syntax tree. */
    finExecMachine();

    /*! \brief Constructs an execution machine with the given name. */
    finExecMachine(const QString &name);

    /*! \brief Destroys the machine, its base environment, and any stored execution diagnostics. */
    ~finExecMachine();
    ///@}

    /*! \name Basic Accessors
     *  \brief Inspect the machine's identity, bound runtime objects, and compilation state.
     */
    ///@{

    /*! \brief Returns the machine name. */
    QString getName() const;

    /*! \brief Returns the base environment used for execution, or \c nullptr if uninitialized. */
    finExecEnvironment *getBaseEnvironment();

    /*! \brief Returns the figure container currently bound to the machine. */
    finFigureContainer *getFigureContainer();

    /*! \brief Returns the raw script text currently stored in the compiler. */
    QString getScriptCode() const;

    /*! \brief Returns the script text stored in the compiled syntax tree, or an empty string. */
    QString getCompiledScriptCode() const;

    /*! \brief Returns the currently compiled syntax tree, or \c nullptr. */
    finSyntaxTree *getSyntaxTree();

    /*! \brief Returns the diagnostic dumper currently attached to the execution error list. */
    finSyntaxErrorDump *getExecuteErrorDumper() const;

    /*! \brief Returns the number of execution diagnostics currently stored. */
    int getExecuteErrorCount() const;

    /*! \brief Returns one execution diagnostic by index. */
    finSyntaxError getExecuteErrorAt(int idx) const;
    ///@}

    /*! \brief Sets the machine name. */
    void setName(const QString &name);

    /*! \name Environment Setup
     *  \brief Initialize the base execution environment for a script run.
     */
    ///@{

    /*! \brief Creates a base child environment under \a prevenv or under root when \a prevenv is null. */
    finErrorCode initEnvironment(finExecEnvironment *prevenv);

    /*! \brief Creates a base child environment directly under the global root environment. */
    finErrorCode initEnvironmentFromRoot();
    ///@}

    /*! \name Input And Output Configuration
     *  \brief Bind the figure container, script text, and diagnostic output behavior.
     */
    ///@{

    /*! \brief Sets the figure container used for drawing output. */
    finErrorCode setFigureContainer(finFigureContainer *figcontainer);

    /*! \brief Replaces the script text stored in the internal compiler. */
    void setScriptCode(const QString &script);

    /*! \brief Attaches a dumper for real-time execution diagnostics. */
    finErrorCode setExecuteErrorDumper(finSyntaxErrorDump *dumper);
    ///@}

    /*! \name Compilation And Execution
     *  \brief Compile the current script and execute the resulting syntax tree.
     */
    ///@{

    /*! \brief Returns whether a syntax tree has already been compiled and stored. */
    bool isCompiled() const;

    /*! \brief Compiles the current script text into a syntax tree. */
    finErrorCode compile();

    /*! \brief Executes the currently compiled syntax tree in the base environment. */
    finErrorCode execute();

    /*! \brief Clears all recorded execution diagnostics. */
    void disposeExecutionError();

    /*! \brief Executes one syntax node immediately in the given environment.
     *
     *  This is the main recursive dispatch entry used internally and by helper classes such as
     *  finExecFunction.
     */
    finErrorCode instantExecute(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    ///@}

    /*! \name Diagnostic Reporting
     *  \brief Append execution-time diagnostics to the machine's error list.
     */
    ///@{

    /*! \brief Appends one execution diagnostic with the specified severity. */
    void appendExecutionOutput(finSyntaxError::Level level, finLexNode *lexnode, const QString &errinfo);

    /*! \brief Appends one execution error. */
    void appendExecutionError(finLexNode *lexnode, const QString &errinfo);

    /*! \brief Appends one execution warning. */
    void appendExecutionWarning(finLexNode *lexnode, const QString &errinfo);
    ///@}

private:
    /*! \name Top-Level Syntax Dispatch
     *  \brief Dispatch one syntax node by its high-level finSyntaxNode type.
     */
    ///@{
    finErrorCode instExecSingle(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclare(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecStatement(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExpress(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecFunction(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecBranch(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLoop(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLabel(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJump(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecProgram(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    ///@}

    /*! \name Declaration Execution Helpers
     *  \brief Execute variable declarations and their subforms.
     */
    ///@{
    finErrorCode instExecDeclareDirect(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareAssigned(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareComma(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecDeclareExpr(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    ///@}

    /*! \brief Executes the inside of a statement list or block in sequence. */
    finErrorCode instExecStatIn(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);

    /*! \name Expression Execution Helpers
     *  \brief Evaluate expressions and store their results in flow control.
     */
    ///@{
    finErrorCode instExecExprNull(finExecFlowControl *flowctl);
    finErrorCode instExecExprVar(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExprNum(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecExprStr(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecExprFunc(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecExprOper(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    ///@}

    /*! \name Function Definition Helpers
     *  \brief Build user-defined runtime functions from syntax nodes.
     */
    ///@{
    finErrorCode instExecFuncName(finSyntaxNode *synnode, finExecEnvironment *env, finExecFunction *func);
    finErrorCode instExecFuncArg(finSyntaxNode *synnode, finExecFunction *func);
    finErrorCode instExecFuncArgs(finSyntaxNode *synnode, finExecFunction *func);
    finErrorCode instExecFuncBody(finSyntaxNode *synnode, finExecFunction *func);
    ///@}

    /*! \name Branch Helpers
     *  \brief Evaluate branch conditions and execute branch bodies.
     */
    ///@{
    finErrorCode instExecBrCondVoid(bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecBrCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                bool *retblval, finExecFlowControl *flowctl);
    ///@}

    /*! \name Loop Helpers
     *  \brief Evaluate loop conditions and execute while/for loops.
     */
    ///@{
    finErrorCode instExecLoopCondVoid(bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecLoopCond(finSyntaxNode *synnode, finExecEnvironment *env,
                                  bool *retblval, finExecFlowControl *flowctl);
    finErrorCode instExecLoopWhile(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecLoopForHead(finSyntaxNode *synnode,
                                     finSyntaxNode **inithead, finSyntaxNode **condhead,
                                     finSyntaxNode **stephead, finSyntaxNode **loopbody);
    finErrorCode instExecLoopFor(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    ///@}

    /*! \name Jump Helpers
     *  \brief Execute goto, return, exit, continue, and break statements.
     */
    ///@{
    finErrorCode instExecJumpGoto(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpRetVoid(finExecFlowControl *flowctl);
    finErrorCode instExecJumpRetVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpRet(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExitVoid(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExitVal(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpExit(finSyntaxNode *synnode, finExecEnvironment *env, finExecFlowControl *flowctl);
    finErrorCode instExecJumpConti(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    finErrorCode instExecJumpBreak(finSyntaxNode *synnode, finExecFlowControl *flowctl);
    ///@}
};

#endif // FINEXECMACHINE_H

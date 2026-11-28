/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finPlotEquation2D.h
 *  \brief Declaration of the plotter that draws the implicit equation \c f(x,y)=0 as level curves.
 *
 *  Unlike the explicit plotters in finPlotFunction, this plotter samples a two-variable function on a
 *  rectangular grid, refines every sign change it finds, and lets finPlotDotsScatter reconnect the sampled
 *  points into continuous curves.
 */

#ifndef FINPLOTEQUATION2D_H
#define FINPLOTEQUATION2D_H

#include <QString>
#include <QList>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecFlowControl.h"
#include "finPlotDots.h"


/*! \class finPlotEquation2D
 *  \brief Plotter that draws the implicit equation \c f(x,y)=0 inside a rectangular figuring range.
 *
 *  The plotter never owns its inputs. The function name, the figuring ranges, the call argument list and the
 *  execution context (environment, machine and flow control) are supplied through the setters, and the target
 *  figure container is set with setFigureContainer(). checkValid() reports whether that configuration is
 *  complete, and plot() refuses to run until it is.
 *
 *  The sampling grid is built from the figuring ranges with a step taken from the graph scale (see
 *  getBaseStep()). Every grid point is evaluated, and each sign change found between neighbouring points is
 *  refined by searchBinary(). The collected points are reconnected by finPlotDotsScatter, whose distance limit
 *  is set to three times the step so that adjacent level curves visually join up.
 */
class finPlotEquation2D
{
protected:
    QString _funcname;                      //!< Name of the function to evaluate, resolved in the environment.
    double _fromX, _toX;                    //!< Figuring range of the x variable, with \c _fromX < \c _toX.
    double _fromY, _toY;                    //!< Figuring range of the y variable, with \c _fromY < \c _toY.
    QList<double> _posListX, _posListY;     //!< Grid positions sampled along X and along Y.
    int _xidx, _yidx;                       //!< Argument slots that receive the x and y variables.

    QList<finExecVariable *> *_callArgList; //!< Arguments of the plotted call; borrowed, not owned.
    finExecEnvironment *_environment;       //!< Environment that resolves and runs the function; borrowed.
    finExecMachine *_machine;               //!< Execution machine handed to the function call; borrowed.

    finExecFlowControl *_flowctl;           //!< Flow control that carries the return value; borrowed.
    finPlotDotsScatter _scrtPlot;           //!< Collected points and the reconnector that emits the curves.

public:
    /*! \brief Constructs a plotter with an empty name, zeroed ranges and no execution context. */
    finPlotEquation2D();

    /*! \brief Returns the name of the function to evaluate. */
    const QString &getFunctionName() const;
    /*! \brief Returns the lower bound of the x figuring range. */
    double getStartPositionX() const;
    /*! \brief Returns the upper bound of the x figuring range. */
    double getEndPostionX() const;
    /*! \brief Returns the lower bound of the y figuring range. */
    double getStartPostionY() const;
    /*! \brief Returns the upper bound of the y figuring range. */
    double getEndPostionY() const;
    /*! \brief Returns the argument slot that receives the x variable. */
    int getVariableXIndex() const;
    /*! \brief Returns the argument slot that receives the y variable. */
    int getVariableYIndex() const;

    /*! \brief Sets the name of the function to evaluate; it is resolved in the environment when plotting. */
    void setFunctionName(const QString &funcname);
    /*! \brief Sets the x figuring range, swapping the bounds when \a x1 is greater than \a x2. */
    void setFiguringRangeX(double x1, double x2);
    /*! \brief Sets the y figuring range, swapping the bounds when \a y1 is greater than \a y2. */
    void setFiguringRangeY(double y1, double y2);
    /*! \brief Sets the argument slot that receives the x variable.
     *  \param idx The slot index, which must differ from the y slot.
     *  \throws finException with finErrorKits::EC_INVALID_PARAM when \a idx is negative.
     */
    void setVariableXIndex(int idx);
    /*! \brief Sets the argument slot that receives the y variable.
     *  \param idx The slot index, which must differ from the x slot.
     *  \throws finException with finErrorKits::EC_INVALID_PARAM when \a idx is negative.
     */
    void setVariableYIndex(int idx);

    /*! \brief Returns the argument list of the plotted call, or \c nullptr if none has been set. */
    QList<finExecVariable *> *getCallArgList() const;
    /*! \brief Returns the environment used to run the function, or \c nullptr if none has been set. */
    finExecEnvironment *getEnvironment() const;
    /*! \brief Returns the execution machine handed to the function call, or \c nullptr if none has been set. */
    finExecMachine *getMachine() const;
    /*! \brief Returns the flow control that carries the return value, or \c nullptr if none has been set. */
    finExecFlowControl *getFlowControl() const;
    /*! \brief Returns the target figure container, or \c nullptr if none has been set. */
    finFigureContainer *getFigureContainer() const;

    /*! \brief Sets the argument list of the plotted call; the list is borrowed and must outlive plot(). */
    void setCallArgList(QList<finExecVariable *> *arglist);
    /*! \brief Sets the environment used to resolve and run the function; borrowed, not owned. */
    void setEnvironment(finExecEnvironment *env);
    /*! \brief Sets the execution machine handed to the function call; borrowed, not owned. */
    void setMachine(finExecMachine *machine);
    /*! \brief Sets the flow control that carries the return value; borrowed, not owned. */
    void setFlowControl(finExecFlowControl *flowctl);
    /*! \brief Sets the container that receives the plotted figures; borrowed, not owned. */
    void setFigureContainer(finFigureContainer *figcontainer);

    /*! \brief Returns \c true when the configuration is complete enough for plot() to run.
     *
     *  The name must be non-empty, both figuring ranges must be non-degenerate, every execution pointer must
     *  be set, and the x and y slots must differ and stay below the argument count plus two, since the two
     *  variables are inserted on top of the caller's arguments.
     */
    bool checkValid() const;
    /*! \brief Samples the equation and plots the resulting level curves.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_STATE_ERROR when checkValid() fails;
     *          finErrorKits::EC_NOT_FOUND when the function cannot be resolved in the environment; or an
     *          error code propagated from the sampling and from finPlotDotsScatter::plot().
     */
    finErrorCode plot();

private:
    /*! \brief Returns the sampling step in figure units, i.e. three pixels of the graph or \c 0.1 by default. */
    double getBaseStep() const;
    /*! \brief Creates the x and y variables and builds the argument list passed to the function.
     *  \param varlist Output that receives a copy of the caller's argument list with both variables inserted.
     *  \param xvar    Output that receives the new variable that carries the x value.
     *  \param yvar    Output that receives the new variable that carries the y value.
     *  \return finErrorKits::EC_NULL_POINTER for a null output, finErrorKits::EC_STATE_ERROR when no
     *          environment has been set, or finErrorKits::EC_OUT_OF_MEMORY when a variable cannot be created.
     */
    finErrorCode buildFuncArgList(QList<finExecVariable *> *varlist,
                                  finExecVariable **xvar, finExecVariable **yvar);

    /*! \brief Fills \a poslist with \a from, \a from + \a step, ... below \a to, and finally with \a to.
     *  \return finErrorKits::EC_NULL_POINTER for a null \a poslist; finErrorKits::EC_INVALID_PARAM when
     *          \a step is not positive.
     */
    finErrorCode buildSearchPositions(double from, double to, double step, QList<double> *poslist);
    /*! \brief Fills the X and Y position lists with the grid used by plot(). */
    finErrorCode buildSearchRangeList(double step);
    /*! \brief Empties both position lists. */
    finErrorCode disposeSearchRangeList();

    /*! \brief Evaluates the function at (\a x, \a y) and stores its numeric return value in \a retval.
     *  \param x       The x variable value.
     *  \param y       The y variable value.
     *  \param func    The function to call.
     *  \param varlist The argument list built by buildFuncArgList().
     *  \param xvar    The variable that receives \a x.
     *  \param yvar    The variable that receives \a y.
     *  \param retval  Output that receives the numeric result.
     *  \param goon    Output that reports whether execution may continue; \c false means the evaluated
     *                 expression requested a flow-control action.
     *  \return finErrorKits::EC_NULL_POINTER for a null pointer, an error propagated from the function call,
     *          or finErrorKits::EC_INVALID_PARAM when the function does not return a numeric value.
     */
    finErrorCode calcAPoint(double x, double y, finExecFunction *func, QList<finExecVariable *> *varlist,
                            finExecVariable *xvar, finExecVariable *yvar, double *retval, bool *goon);
    /*! \brief Classifies one sampled grid point and reports which neighbouring intervals must be refined.
     *  \param xidx       Column index of the sampled point.
     *  \param yidx       Row index of the sampled point.
     *  \param curretval  Function value at the sampled point.
     *  \param prevretval Values stored for the surrounding points: entry \a yidx still belongs to the previous
     *                    column, while entry \a yidx - 1 already belongs to the current one.
     *  \param srchflags  Output that receives a combination of \c FIN_PE_SRF_* flags.
     *  \return finErrorKits::EC_SUCCESS on success; finErrorKits::EC_INVALID_PARAM for a negative index or a
     *          row beyond \a prevretval; finErrorKits::EC_NULL_POINTER for a null output.
     */
    finErrorCode checkOnePosition(int xidx, int yidx, double curretval, QList<double> *prevretval,
                                   unsigned long *srchflags);
    /*! \brief Stores \a curretval for the point (\a xidx, \a yidx) in \a prevretval.
     *
     *  The first column is inserted so that the list ends up indexed by row, while any later column replaces
     *  the entry of the same row.
     */
    finErrorCode updateRetValList(int xidx, int yidx, double curretval, QList<double> *prevretval);

    /*! \brief Refines a sign change between two sampled points by bisection.
     *
     *  The interval is halved six times; afterwards the zero is estimated by linear interpolation between the
     *  two remaining ends.
     *
     *  \param x1      x coordinate of the first interval end.
     *  \param y1      y coordinate of the first interval end.
     *  \param retval1 Function value at (\a x1, \a y1).
     *  \param x2      x coordinate of the second interval end.
     *  \param y2      y coordinate of the second interval end.
     *  \param retval2 Function value at (\a x2, \a y2).
     *  \param func    The function to call.
     *  \param varlist The argument list built by buildFuncArgList().
     *  \param xvar    The variable that receives the x value.
     *  \param yvar    The variable that receives the y value.
     *  \param xret    Output that receives the x coordinate of the zero.
     *  \param yret    Output that receives the y coordinate of the zero.
     *  \param goon    Output that reports whether execution may continue.
     *  \param level   Current recursion depth; callers leave it at \c 0.
     *  \return finErrorKits::EC_SUCCESS when a zero was located; finErrorKits::EC_INVALID_PARAM when both ends
     *          share the same sign; finErrorKits::EC_NULL_POINTER for a null pointer; or an error propagated
     *          from calcAPoint().
     */
    finErrorCode searchBinary(double x1, double y1, double retval1, double x2, double y2, double retval2,
                              finExecFunction *func, QList<finExecVariable *> *varlist,
                              finExecVariable *xvar, finExecVariable *yvar,
                              double *xret, double *yret, bool *goon, int level = 0);
};

#endif // FINPLOTEQUATION2D_H

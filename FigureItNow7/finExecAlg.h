/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finExecAlg.h
 *  \brief Declarations of runtime conversion, vector, and matrix helpers.
 *
 *  This header defines finExecAlg, a namespace-like utility class that centralizes the data-conversion
 *  helpers and numeric algorithms used by the FIN-script execution engine.
 */

#ifndef FINEXECALG_H
#define FINEXECALG_H

#include <QList>
#include <QString>
#include <QStringList>

#include "finErrorCode.h"
#include "finExecVariable.h"

/*! \class finExecAlg
 *  \brief Static helper collection for FIN-script runtime algorithms.
 *
 *  finExecAlg is used as a namespace. It contains the conversion routines that translate between Qt
 *  containers and finExecVariable arrays, the string-parsing helpers used by built-in functions, and the
 *  vector / matrix operations consumed by the execution layer.
 */
class finExecAlg
{
public:
    /*!
     *  \brief Default constructor (unused).
     *
     *  finExecAlg is a namespace-like utility class and is not intended to be instantiated.
     */
    finExecAlg();

    /*! \name QStringList And Array Conversion Helpers
     *  \brief Convert between QStringList values and finExecVariable arrays.
     */
    ///@{

    /*! \brief Converts a list of strings to a numeric array variable.
     *
     *  Each item is parsed with QString::toDouble(); items that cannot be parsed become 0.0.
     *
     *  \param strlist  Source string list.
     *  \param outvar   Output variable that receives a TP_ARRAY of TP_NUMERIC items.
     */
    static void stringListToNumArrayVar(const QStringList &strlist, finExecVariable *outvar);

    /*! \brief Converts a list of strings to a string array variable.
     *
     *  \param strlist  Source string list.
     *  \param outvar   Output variable that receives a TP_ARRAY of TP_STRING items.
     */
    static void stringListToStrArrayVar(const QStringList &strlist, finExecVariable *outvar);

    /*! \brief Converts a list of strings to a mixed array variable.
     *
     *  Numeric-looking items become TP_NUMERIC, empty items become TP_NULL, and the rest become
     *  TP_STRING.
     *
     *  \param strlist  Source string list.
     *  \param outvar   Output variable that receives the converted array.
     */
    static void stringListToArrayVar(const QStringList &strlist, finExecVariable *outvar);

    /*! \brief Converts a numeric array variable to a list of strings.
     *
     *  Non-numeric items are rendered as fallback values following the runtime's existing coercion rules.
     *
     *  \param invar    Input variable to read.
     *  \param strlist  Output string list.
     */
    static void numArrayVarToStringList(finExecVariable *invar, QStringList *strlist);

    /*! \brief Converts an array variable to a list of strings.
     *
     *  \param invar    Input variable to read.
     *  \param strlist  Output string list.
     */
    static void arrayVarToStringList(finExecVariable *invar, QStringList *strlist);
    ///@}

    /*! \name Comma-Separated String Conversion Helpers
     *  \brief Convert between comma-separated strings and finExecVariable arrays.
     */
    ///@{

    /*! \brief Parses a comma-separated string into a numeric array variable.
     *
     *  \param csstr   Source comma-separated text.
     *  \param outvar  Output variable that receives the converted array.
     */
    static void csStringToNumArrayVar(const QString &csstr, finExecVariable *outvar);

    /*! \brief Parses a comma-separated string into a string array variable.
     *
     *  \param csstr   Source comma-separated text.
     *  \param outvar  Output variable that receives the converted array.
     */
    static void csStringToStrArrayVar(const QString &csstr, finExecVariable *outvar);

    /*! \brief Parses a comma-separated string into a mixed array variable.
     *
     *  \param csstr   Source comma-separated text.
     *  \param outvar  Output variable that receives the converted array.
     */
    static void csStringToArrayVar(const QString &csstr, finExecVariable *outvar);

    /*! \brief Serializes a numeric array variable as comma-separated text.
     *
     *  \param invar  Input variable to serialize.
     *  \return A comma-separated string generated from the input array.
     */
    static QString numArrayVarToCsString(finExecVariable *invar);

    /*! \brief Serializes an array variable as comma-separated text.
     *
     *  \param invar  Input variable to serialize.
     *  \return A comma-separated string generated from the input array.
     */
    static QString arrayVarToCsString(finExecVariable *invar);
    ///@}

    /*! \name Qt List And Variable Conversion Helpers
     *  \brief Convert between finExecVariable arrays / matrices and QList containers.
     */
    ///@{

    /*! \brief Converts a numeric array variable to a flat QList of doubles.
     *
     *  \param invar  Input variable to read.
     *  \param list   Output numeric list.
     */
    static void numArrayVarToList(finExecVariable *invar, QList<double> *list);

    /*! \brief Converts a numeric matrix variable to a nested QList of doubles.
     *
     *  \param invar  Input variable to read.
     *  \param list   Output matrix list.
     */
    static void numMatVarToList(finExecVariable *invar, QList<QList<double>> *list);

    /*! \brief Converts a flat numeric list to an array variable.
     *
     *  \param list    Source numeric list.
     *  \param outvar  Output variable that receives a numeric array.
     */
    static void listToNumArrayVar(const QList<double> &list, finExecVariable *outvar);

    /*! \brief Converts a nested numeric list to a matrix variable.
     *
     *  \param list    Source matrix list.
     *  \param outvar  Output variable that receives a numeric matrix.
     */
    static void listToNumMatVar(const QList<QList<double>> &list, finExecVariable *outvar);
    ///@}

    /*! \name Array Reshaping Helpers
     *  \brief Flatten, slice, and concatenate runtime array values.
     */
    ///@{

    /*! \brief Flattens a nested numeric list into a single output list.
     *
     *  \param inlist   Source nested list.
     *  \param outlist  Output flat list.
     */
    static void listMatrixToArray(const QList< QList<double> > &inlist, QList<double> *outlist);

    /*! \brief Flattens an array-or-matrix variable into a one-dimensional array variable.
     *
     *  \param invar   Input variable to flatten.
     *  \param outvar  Output variable that receives the flattened array.
     */
    static void varMatrixToArray(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Copies a subrange of an array variable into a new array variable.
     *
     *  \param invar   Source array variable.
     *  \param from    Start index; negative values are clamped to 0.
     *  \param to      End index; values outside the array are clamped to the array length.
     *  \param outvar  Output variable that receives the sliced array.
     */
    static void varArrayCut(finExecVariable *invar, int from, int to, finExecVariable *outvar);

    /*! \brief Concatenates variables or nested arrays into one output array.
     *
     *  Array inputs are expanded item by item; scalar inputs are appended as single items.
     *
     *  \param invarlist  Input variables to join.
     *  \param outvar     Output variable that receives the concatenated array.
     */
    static void varArrayJoin(const QList<finExecVariable *> &invarlist, finExecVariable *outvar);
    ///@}

    /*! \name Numeric Array Algorithms
     *  \brief Perform element-wise operations and reductions on flat numeric lists.
     */
    ///@{

    /*! \brief Negates every element in a numeric list. */
    static void listArrayNeg(const QList<double> &inlist, QList<double> *outlist);

    /*! \brief Adds two numeric lists element by element. */
    static void listArrayAdd(const QList<double> &inlist1, const QList<double> &inlist2,
                             QList<double> *outlist);

    /*! \brief Subtracts two numeric lists element by element. */
    static void listArraySub(const QList<double> &inlist1, const QList<double> &inlist2,
                             QList<double> *outlist);

    /*! \brief Computes the sum of all items in a numeric list. */
    static void listArraySum(const QList<double> &inlist, double *outval);

    /*! \brief Computes the arithmetic mean of a numeric list. */
    static void listArrayAvg(const QList<double> &inlist, double *outval);
    ///@}

    /*! \name Vector Algorithms
     *  \brief Compute vector norms, normalization, and dot products for numeric lists.
     */
    ///@{

    /*! \brief Computes the Euclidean norm of a numeric list. */
    static void listVectorNorm(const QList<double> &inlist, double *outval);

    /*! \brief Computes the L1 norm of a numeric list. */
    static void listVectorNorm1(const QList<double> &inlist, double *outval);

    /*! \brief Computes the Lp norm of a numeric list for the given exponent. */
    static void listVectorNormP(const QList<double> &inlist, double p, double *outval);

    /*! \brief Computes the infinity norm of a numeric list. */
    static void listVectorNormInf(const QList<double> &inlist, double *outval);

    /*! \brief Normalizes a numeric list to unit Euclidean length. */
    static void listVectorNormalize(const QList<double> &inlist, QList<double> *outlist);

    /*! \brief Computes the dot product of two numeric lists. */
    static void listVectorDot(const QList<double> &inlist1, const QList<double> &inlist2, double *outval);
    ///@}

    /*! \name Variable-Based Array And Vector Algorithms
     *  \brief Wrappers that validate finExecVariable inputs before calling list-based helpers.
     */
    ///@{

    /*! \brief Negates a numeric array variable element by element. */
    static void varArrayNeg(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Adds two numeric array variables element by element. */
    static void varArrayAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);

    /*! \brief Subtracts two numeric array variables element by element. */
    static void varArraySub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);

    /*! \brief Computes the sum of a numeric array variable. */
    static void varArraySum(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Computes the arithmetic mean of a numeric array variable. */
    static void varArrayAvg(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Computes the Euclidean norm of a numeric array variable. */
    static void varVectorNorm(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Computes the L1 norm of a numeric array variable. */
    static void varVectorNorm1(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Computes the Lp norm of a numeric array variable. */
    static void varVectorNormP(finExecVariable *invar, finExecVariable *pvar, finExecVariable *outvar);

    /*! \brief Computes the infinity norm of a numeric array variable. */
    static void varVectorNormInf(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Normalizes a numeric array variable to unit length. */
    static void varVectorNormalize(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Computes the dot product of two numeric array variables. */
    static void varVectorDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    ///@}

    /*! \name Matrix Algorithms
     *  \brief Perform transpose, addition, subtraction, and multiplication on numeric matrices.
     */
    ///@{

    /*! \brief Transposes a numeric matrix stored as nested lists. */
    static void listMatTranspose(const QList<QList<double>> &inlist, QList<QList<double>> *outlist);

    /*! \brief Adds two numeric matrices stored as nested lists. */
    static void listMatAdd(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                           QList<QList<double>> *outlist);

    /*! \brief Subtracts two numeric matrices stored as nested lists. */
    static void listMatSub(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                           QList<QList<double>> *outlist);

    /*! \brief Multiplies two numeric matrices stored as nested lists. */
    static void listMatDot(const QList<QList<double>> &inlist1, const QList<QList<double>> &inlist2,
                           QList<QList<double>> *outlist);

    /*! \brief Transposes a numeric matrix variable. */
    static void varMatTranspose(finExecVariable *invar, finExecVariable *outvar);

    /*! \brief Adds two numeric matrix variables. */
    static void varMatAdd(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);

    /*! \brief Subtracts two numeric matrix variables. */
    static void varMatSub(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);

    /*! \brief Multiplies two numeric matrix variables. */
    static void varMatDot(finExecVariable *invar1, finExecVariable *invar2, finExecVariable *outvar);
    ///@}
};

#endif // FINEXECALG_H

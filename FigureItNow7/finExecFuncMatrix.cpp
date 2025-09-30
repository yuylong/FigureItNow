/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2025 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>
#include <QScopedPointer>

#include "finExecAlg.h"
#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static QString _defFuncCtg("Matrix");

static finErrorCode _sysfunc_array(finExecFunction *self, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cntvar;
    int arylen = -1, aryidx = 0;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    cntvar = finExecVariable::transLinkTarget(env->findVariable("n"));
    if ( cntvar != nullptr && cntvar->getType() != finExecVariable::TP_NUMERIC &&
                           cntvar->getType() != finExecVariable::TP_NULL )
        return finErrorKits::EC_INVALID_PARAM;
    else if ( cntvar != nullptr && cntvar->getType() == finExecVariable::TP_NUMERIC )
        arylen = cntvar->getNumericValue();

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_ARRAY);
    if ( arylen == 0 )
        goto out;
    else if ( arylen > 0 )
        retvar->preallocArrayLength(arylen);

    while ( true ) {
        if ( aryidx >= finExecFunction::getExtendArgCount(env) )
            break;
        else if ( arylen > 0 && aryidx >= arylen )
            break;

        finExecVariable *itemvar = finExecVariable::transLinkTarget(finExecFunction::getExtendArgAt(env, aryidx));
        finExecVariable *retitemvar = retvar->getVariableItemAt(aryidx);
        if ( retitemvar == nullptr ) {
            delete retvar;
            return finErrorKits::EC_STATE_ERROR;
        }

        errcode = retitemvar->copyVariableValue(itemvar);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
        aryidx++;
    }

out:
    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array = {
    /*._funcName     =*/ QString("array"),
    /*._paramCsvList =*/ QString("n"),
    /*._funcCall     =*/ _sysfunc_array,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("array (n, ...)"),
    /*._description  =*/ QString("Generate an N-dimension array/vector with the given arguments."),
};

static struct finExecSysFuncRegItem _funcRegItem_vector = {
    /*._funcName     =*/ QString("vector"),
    /*._paramCsvList =*/ QString("n"),
    /*._funcCall     =*/ _sysfunc_array,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("vector (n, ...)"),
    /*._description  =*/ QString("Generate an N-dimension vector/array with the given arguments."),
};


static finErrorCode _sysfunc_matrix(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *rowvar, *colvar;
    int matrow, matcol;
    int extcnt;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    rowvar = finExecVariable::transLinkTarget(env->findVariable("row"));
    colvar = finExecVariable::transLinkTarget(env->findVariable("col"));
    if ( rowvar == nullptr || colvar == nullptr )
        return finErrorKits::EC_INVALID_PARAM;
    if ( rowvar->getType() != finExecVariable::TP_NUMERIC ||
         colvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    matrow = (int)floor(rowvar->getNumericValue());
    matcol = (int)floor(colvar->getNumericValue());
    if ( matrow < 0 || matcol < 0 )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_ARRAY);
    if ( matrow == 0 )
        goto out;

    retvar->preallocArrayLength(matrow);
    if ( matcol == 0 )
        goto out;

    extcnt = finExecFunction::getExtendArgCount(env);
    for ( int rowidx = 0; rowidx < matrow; rowidx++ ) {
        if ( rowidx * matcol >= extcnt )
            break;

        finExecVariable *rowvar = retvar->getVariableItemAt(rowidx);
        if ( rowvar == nullptr ) {
            delete retvar;
            return finErrorKits::EC_STATE_ERROR;
        }

        rowvar->preallocArrayLength(matcol);
        for ( int colidx = 0; colidx < matcol; colidx++ ) {
            int extidx = rowidx * matcol + colidx;
            if ( extidx >= extcnt )
                break;

            finExecVariable *srcvar = finExecVariable::transLinkTarget(
                        finExecFunction::getExtendArgAt(env, extidx));
            finExecVariable *itemvar = rowvar->getVariableItemAt(colidx);
            if ( itemvar == nullptr ) {
                delete retvar;
                return finErrorKits::EC_STATE_ERROR;
            }

            errcode = itemvar->copyVariableValue(srcvar);
            if ( finErrorKits::isErrorResult(errcode) ) {
                delete retvar;
                return errcode;
            }
        }
    }

out:
    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_matrix = {
    /*._funcName     =*/ QString("matrix"),
    /*._paramCsvList =*/ QString("row,col"),
    /*._funcCall     =*/ _sysfunc_matrix,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("matrix (row, col, ...)"),
    /*._description  =*/ QString("Generate a matrix whose size is (row x col). The elements are given in the argument "
                                 "list following row and col."),
};

static finErrorCode _sysfunc_matrix2array(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *matvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));
    if ( matvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finExecAlg::varMatrixToArray(matvar, retvar.data());

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_matrix2array = {
    /*._funcName     =*/ QString("matrix2array"),
    /*._paramCsvList =*/ QString("mat"),
    /*._funcCall     =*/ _sysfunc_matrix2array,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("matrix2array (mat)"),
    /*._description  =*/ QString("Convert the given matrix into an array/vector."),
};

static struct finExecSysFuncRegItem _funcRegItem_matrix2vector = {
    /*._funcName     =*/ QString("matrix2vector"),
    /*._paramCsvList =*/ QString("mat"),
    /*._funcCall     =*/ _sysfunc_matrix2array,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("matrix2vector (mat)"),
    /*._description  =*/ QString("Convert the given matrix into a vector/array."),
};

static finErrorCode _sysfunc_array_cut(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar, *fromvar, *tovar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    fromvar = finExecVariable::transLinkTarget(env->findVariable("from"));
    tovar = finExecVariable::transLinkTarget(env->findVariable("to"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fromvar != nullptr && fromvar->getType() != finExecVariable::TP_NULL &&
         fromvar->getType() != finExecVariable::TP_NUMERIC &&
         tovar != nullptr && tovar->getType() != finExecVariable::TP_NULL &&
         tovar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    int from = -1, to = -1;
    if ( fromvar != nullptr && fromvar->getType() == finExecVariable::TP_NUMERIC )
        from = fromvar->getNumericValue();
    if ( tovar != nullptr && tovar->getType() == finExecVariable::TP_NUMERIC )
        to = tovar->getNumericValue();

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finExecAlg::varArrayCut(aryvar, from, to, retvar.data());

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_cut = {
    ._funcName     = QString("array_cut"),
    ._paramCsvList = QString("ary,from,to"),
    ._funcCall     = _sysfunc_array_cut,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_cut (ary, from, to)"),
    ._description  = QString("Pick a sub-array form the given array."),
};

static struct finExecSysFuncRegItem _funcRegItem_vec_cut = {
    ._funcName     = QString("vec_cut"),
    ._paramCsvList = QString("ary,from,to"),
    ._funcCall     = _sysfunc_array_cut,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_cut (vec, from, to)"),
    ._description  = QString("Get a lower dimensioned vector from the given vector."),
};

static finErrorCode _sysfunc_array_join(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QList<finExecVariable *> invarlist;
    invarlist.append(aryvar);
    int varcnt = finExecFunction::getExtendArgCount(env);
    for ( int i = 0; i < varcnt; i++ ) {
        aryvar = finExecVariable::transLinkTarget(finExecFunction::getExtendArgAt(env, i));
        invarlist.append(aryvar);
    }

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finExecAlg::varArrayJoin(invarlist, retvar.data());

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_join = {
    ._funcName     = QString("array_join"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_join,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_join (ary, ...)"),
    ._description  = QString("Join several arrays/vectors into a unified one."),
};

static struct finExecSysFuncRegItem _funcRegItem_vec_join = {
    ._funcName     = QString("vec_join"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_join,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_join (vec, ...)"),
    ._description  = QString("Join several vectors/arrays into a unified one."),
};

static finErrorCode _sysfunc_array_size(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    double size = 0;
    if ( aryvar->getType() == finExecVariable::TP_ARRAY )
        size = aryvar->getArrayLength();

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(size);
    retvar->clearLeftValue();
    retvar->setWriteProtected();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_size = {
    ._funcName     = QString("array_size"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_size,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_size (ary)"),
    ._description  = QString("Get the size of the given array."),
};

static finErrorCode _sysfunc_array_neg(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varArrayNeg(aryvar, retvar.data());
    } catch (finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_neg = {
    ._funcName     = QString("array_neg"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_neg,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_neg (ary)"),
    ._description  = QString("Returns a new array whose elements are the negative of the corresponding ones in the "
                             "given array."),
};

static struct finExecSysFuncRegItem _funcRegItem_vec_neg = {
    ._funcName     = QString("vec_neg"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_neg,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_neg (vec)"),
    ._description  = QString("Returns a reversed copy of the given vector."),
};

static finErrorCode _sysfunc_array_add(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *ary1var, *ary2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == nullptr || ary2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varArrayAdd(ary1var, ary2var, retvar.data());
    } catch (finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_add = {
    ._funcName     = QString("array_add"),
    ._paramCsvList = QString("ary1,ary2"),
    ._funcCall     = _sysfunc_array_add,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_add (ary1, ary2)"),
    ._description  = QString("Returns a new array whose elements are the summation of the corresponding elements of "
                             "the give two arrays."),
};

static struct finExecSysFuncRegItem _funcRegItem_vec_add = {
    ._funcName     = QString("vec_add"),
    ._paramCsvList = QString("ary1,ary2"),
    ._funcCall     = _sysfunc_array_add,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_add (vec1, vec2)"),
    ._description  = QString("Returns the summation of the two given vectors."),
};

static finErrorCode _sysfunc_array_sub(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *ary1var, *ary2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == nullptr || ary2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varArraySub(ary1var, ary2var, retvar.data());
    } catch (finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_sub = {
    ._funcName     = QString("array_sub"),
    ._paramCsvList = QString("ary1,ary2"),
    ._funcCall     = _sysfunc_array_sub,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_sub (ary1, ary2)"),
    ._description  = QString("Returns a new array whose elements are the difference of the corresponding elements of "
                             "the give two arrays."),
};

static struct finExecSysFuncRegItem _funcRegItem_vec_sub = {
    ._funcName     = QString("vec_sub"),
    ._paramCsvList = QString("ary1,ary2"),
    ._funcCall     = _sysfunc_array_sub,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_sub (vec1, vec2)"),
    ._description  = QString("Returns the difference of the two given vectors."),
};

static finErrorCode _sysfunc_array_sum(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varArraySum(aryvar, retvar.data());
    } catch (const finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_sum = {
    ._funcName     = QString("array_sum"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_sum,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_sum (ary)"),
    ._description  = QString("Returns the summation of all elements in the given array."),
};

static finErrorCode _sysfunc_array_avg(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varArrayAvg(aryvar, retvar.data());
    } catch (const finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_array_avg = {
    ._funcName     = QString("array_avg"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_array_avg,
    ._category     = _defFuncCtg,
    ._prototype    = QString("array_avg (ary)"),
    ._description  = QString("Returns the average value of all elements in the given array."),
};

static finErrorCode _sysfunc_vec_dim(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    int dim = 0;
    bool isary = aryvar->isNumericArray(&dim);
    if ( !isary )
        dim = 0;

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(dim);
    retvar->clearLeftValue();
    retvar->setWriteProtected();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_vec_dim = {
    ._funcName     = QString("vec_dim"),
    ._paramCsvList = QString("ary"),
    ._funcCall     = _sysfunc_vec_dim,
    ._category     = _defFuncCtg,
    ._prototype    = QString("vec_dim (vec)"),
    ._description  = QString("Returns the dimension of the given vector."),
};

finErrorCode _sysfunc_vec_norm(finExecFunction *self, finExecEnvironment *env,
                               finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varVectorNorm(aryvar, retvar.data());
    } catch (const finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_norm_1(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    try {
        finExecAlg::varVectorNorm1(aryvar, retvar.data());
    } catch (const finException &e) {
        return e.getErrorCode();
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_norm_p(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar, *pvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    pvar = finExecVariable::transLinkTarget(env->findVariable("p"));
    if ( aryvar == nullptr || pvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormP(aryvar, pvar, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_norm_inf(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormInf(aryvar, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_normalize(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormalize(aryvar, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_dot(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *ary1var, *ary2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == nullptr || ary2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorDot(ary1var, ary2var, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_transpose(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *matvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));
    if ( matvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatTranspose(matvar, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == nullptr || mat2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatAdd(mat1var, mat2var, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_sub(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == nullptr || mat2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatSub(mat1var, mat2var, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_dot(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == nullptr || mat2var == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    QScopedPointer<finExecVariable> retvar(new finExecVariable());
    if ( retvar.isNull() )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatDot(mat1var, mat2var, retvar.data());
    if ( finErrorKits::isErrorResult(errcode) ) {
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar.take());
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _finSysFuncMatrixList[] = {
    _funcRegItem_array,
    _funcRegItem_vector,
    _funcRegItem_matrix,
    _funcRegItem_matrix2array,
    _funcRegItem_matrix2vector,
    _funcRegItem_array_cut,
    _funcRegItem_vec_cut,
    _funcRegItem_array_join,
    _funcRegItem_vec_join,

    _funcRegItem_array_size,
    _funcRegItem_array_neg,
    _funcRegItem_array_add,
    _funcRegItem_array_sub,
    _funcRegItem_array_sum,
    _funcRegItem_array_avg,

    _funcRegItem_vec_dim,
    _funcRegItem_vec_neg,
    _funcRegItem_vec_add,
    _funcRegItem_vec_sub,
    { QString("vec_norm"),         QString("ary"),         _sysfunc_vec_norm      },
    { QString("vec_norm_1"),       QString("ary"),         _sysfunc_vec_norm_1    },
    { QString("vec_norm_p"),       QString("ary,p"),       _sysfunc_vec_norm_p    },
    { QString("vec_norm_inf"),     QString("ary"),         _sysfunc_vec_norm_inf  },
    { QString("vec_normalize"),    QString("ary"),         _sysfunc_vec_normalize },
    { QString("vec_dot"),          QString("ary1,ary2"),   _sysfunc_vec_dot       },

    { QString("mat_transpose"),    QString("mat"),         _sysfunc_mat_transpose },
    { QString("mat_add"),          QString("mat1,mat2"),   _sysfunc_mat_add       },
    { QString("mat_sub"),          QString("mat1,mat2"),   _sysfunc_mat_sub       },
    { QString("mat_dot"),          QString("mat1,mat2"),   _sysfunc_mat_dot       },

    { QString(), QString(), nullptr, _defFuncCtg, QString(), QString() }
};


finErrorCode finExecFunction::registSysFuncMatrix()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMatrixList, QString("Matrix"));
}

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>

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
    finErrorCode errcode;
    finExecVariable *matvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));
    if ( matvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatrixToArray(matvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_cut(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArrayCut(aryvar, from, to, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_join(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArrayJoin(invarlist, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_size(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    double size = 0;
    if ( aryvar->getType() == finExecVariable::TP_ARRAY )
        size = aryvar->getArrayLength();

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(size);
    retvar->clearLeftValue();
    retvar->setWriteProtected();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_neg(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArrayNeg(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_add(finExecFunction *self, finExecEnvironment *env,
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArrayAdd(ary1var, ary2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_array_sub(finExecFunction *self, finExecEnvironment *env,
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArraySub(ary1var, ary2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArraySum(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varArrayAvg(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_vec_dim(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
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
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNorm(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNorm1(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormP(aryvar, pvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormInf(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorNormalize(aryvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varVectorDot(ary1var, ary2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatTranspose(matvar, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatAdd(mat1var, mat2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatSub(mat1var, mat2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
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

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = finExecAlg::varMatDot(mat1var, mat2var, retvar);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _finSysFuncMatrixList[] = {
    _funcRegItem_array,
    _funcRegItem_vector,
    _funcRegItem_matrix,
    { QString("matrix_to_array"),  QString("mat"),         _sysfunc_matrix2array  },
    { QString("matrix_to_vector"), QString("mat"),         _sysfunc_matrix2array  },
    { QString("array_cut"),        QString("ary,from,to"), _sysfunc_array_cut     },
    { QString("vector_cut"),       QString("ary,from,to"), _sysfunc_array_cut     },
    { QString("array_join"),       QString("ary"),         _sysfunc_array_join    },
    { QString("vector_join"),      QString("ary"),         _sysfunc_array_join    },

    { QString("array_size"),       QString("ary"),         _sysfunc_array_size    },
    { QString("array_neg"),        QString("ary"),         _sysfunc_array_neg     },
    { QString("array_add"),        QString("ary1,ary2"),   _sysfunc_array_add     },
    { QString("array_sub"),        QString("ary1,ary2"),   _sysfunc_array_sub     },
    { QString("array_sum"),        QString("ary"),         _sysfunc_array_sum     },
    { QString("array_avg"),        QString("ary"),         _sysfunc_array_avg     },

    { QString("vec_dim"),          QString("ary"),         _sysfunc_vec_dim       },
    { QString("vec_neg"),          QString("ary"),         _sysfunc_array_neg     },
    { QString("vec_add"),          QString("ary1,ary2"),   _sysfunc_array_add     },
    { QString("vec_sub"),          QString("ary1,ary2"),   _sysfunc_array_sub     },
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

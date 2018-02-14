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

static finErrorCode _sysfunc_array(finExecFunction *self, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_matrix(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_array_size(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_array_neg(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_array_add(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_array_sub(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_vec_dim(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_norm(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_norm_1(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_norm_p(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_norm_inf(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_normalize(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_vec_dot(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_mat_transpose(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_mat_sub(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_mat_dot(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncMatrixList[] = {
    { QString("array"),         QString("n"),         _sysfunc_array         },
    { QString("vector"),        QString("n"),         _sysfunc_array         },
    { QString("matrix"),        QString("row,col"),   _sysfunc_matrix        },

    { QString("array_size"),    QString("ary"),       _sysfunc_array_size    },
    { QString("array_neg"),     QString("ary"),       _sysfunc_array_neg     },
    { QString("array_add"),     QString("ary1,ary2"), _sysfunc_array_add     },
    { QString("array_sub"),     QString("ary1,ary2"), _sysfunc_array_sub     },

    { QString("vec_dim"),       QString("ary"),       _sysfunc_vec_dim       },
    { QString("vec_neg"),       QString("ary"),       _sysfunc_array_neg     },
    { QString("vec_add"),       QString("ary1,ary2"), _sysfunc_array_add     },
    { QString("vec_sub"),       QString("ary1,ary2"), _sysfunc_array_sub     },
    { QString("vec_norm"),      QString("ary"),       _sysfunc_vec_norm      },
    { QString("vec_norm_1"),    QString("ary"),       _sysfunc_vec_norm_1    },
    { QString("vec_norm_p"),    QString("ary,p"),     _sysfunc_vec_norm_p    },
    { QString("vec_norm_inf"),  QString("ary"),       _sysfunc_vec_norm_inf  },
    { QString("vec_normalize"), QString("ary"),       _sysfunc_vec_normalize },
    { QString("vec_dot"),       QString("ary1,ary2"), _sysfunc_vec_dot       },

    { QString("mat_transpose"), QString("mat"),       _sysfunc_mat_transpose },
    { QString("mat_add"),       QString("mat1,mat2"), _sysfunc_mat_add       },
    { QString("mat_sub"),       QString("mat1,mat2"), _sysfunc_mat_sub       },
    { QString("mat_dot"),       QString("mat1,mat2"), _sysfunc_mat_dot       },

    { QString(), QString(), NULL }
};


finErrorCode finExecFunction::registSysFuncMatrix()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMatrixList);
}

static finErrorCode _sysfunc_array(finExecFunction *self, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cntvar;
    int arylen = -1, aryidx = 0;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    cntvar = finExecVariable::transLinkTarget(env->findVariable("n"));
    if ( cntvar != NULL && cntvar->getType() != finExecVariable::TP_NUMERIC &&
                           cntvar->getType() != finExecVariable::TP_NULL )
        return finErrorKits::EC_INVALID_PARAM;
    else if ( cntvar != NULL && cntvar->getType() == finExecVariable::TP_NUMERIC )
        arylen = cntvar->getNumericValue();

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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
        if ( retitemvar == NULL ) {
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

static finErrorCode _sysfunc_matrix(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *rowvar, *colvar;
    int matrow, matcol;
    int extcnt;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    rowvar = finExecVariable::transLinkTarget(env->findVariable("row"));
    colvar = finExecVariable::transLinkTarget(env->findVariable("col"));
    if ( rowvar == NULL || colvar == NULL )
        return finErrorKits::EC_INVALID_PARAM;
    if ( rowvar->getType() != finExecVariable::TP_NUMERIC ||
         colvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    matrow = (int)floor(rowvar->getNumericValue());
    matcol = (int)floor(colvar->getNumericValue());
    if ( matrow < 0 || matcol < 0 )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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
        if ( rowvar == NULL ) {
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
            if ( itemvar == NULL ) {
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

static finErrorCode _sysfunc_array_size(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == NULL || ary2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == NULL || ary2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

static finErrorCode _sysfunc_vec_dim(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *aryvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    pvar = finExecVariable::transLinkTarget(env->findVariable("p"));
    if ( aryvar == NULL || pvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( aryvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    ary1var = finExecVariable::transLinkTarget(env->findVariable("ary1"));
    ary2var = finExecVariable::transLinkTarget(env->findVariable("ary2"));
    if ( ary1var == NULL || ary2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));
    if ( matvar == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorKits::EC_NOT_FOUND;

    int mat1row = 0, mat1col = 0, mat2row = 0, mat2col = 0;
    if ( !mat1var->isNumericMatrix(&mat1row, &mat1col) || !mat2var->isNumericMatrix(&mat2row, &mat2col) )
        return finErrorKits::EC_INVALID_PARAM;
    if ( mat1col != mat2row )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    errcode = retvar->preallocArrayLength(mat1row);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }
    for ( int rowidx = 0; rowidx < mat1row; rowidx++ ) {
        finExecVariable *rowvar1 = mat1var->getVariableItemAt(rowidx);
        finExecVariable *retrowvar = retvar->getVariableItemAt(rowidx);
        if ( rowvar1 == NULL || retrowvar == NULL ) {
            delete retvar;
            return finErrorKits::EC_OUT_OF_MEMORY;
        }

        errcode = retrowvar->preallocArrayLength(mat2col);
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
        for ( int colidx = 0; colidx < mat2col; colidx++ ) {
            double itemval = 0.0;
            for ( int i = 0; i < mat1col; i++ ) {
                finExecVariable *colvar1 = rowvar1->getVariableItemAt(i);
                finExecVariable *colvar2 = mat2var->getVariableItemAt(i)->getVariableItemAt(colidx);
                if ( colvar1 == NULL || colvar2 == NULL ) {
                    delete retvar;
                    return finErrorKits::EC_OUT_OF_MEMORY;
                }

                itemval += colvar1->getNumericValue() * colvar2->getNumericValue();
            }

            finExecVariable *retcolvar = retrowvar->getVariableItemAt(colidx);
            if ( retcolvar == NULL ) {
                delete retvar;
                return finErrorKits::EC_OUT_OF_MEMORY;
            }

            retcolvar->setType(finExecVariable::TP_NUMERIC);
            retcolvar->setNumericValue(itemval);
        }
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

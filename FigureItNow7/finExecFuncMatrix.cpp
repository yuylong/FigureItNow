/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_array(finExecFunction *self, finExecEnvironment *env,
                                   finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_matrix(finExecFunction *self, finExecEnvironment *env,
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
    { QString("matrix"),        QString("row,col"),   _sysfunc_matrix        },

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
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    cntvar = finExecVariable::transLinkTarget(env->findVariable("n"));
    if ( cntvar != NULL && cntvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC &&
                           cntvar->getType() != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    else if ( cntvar != NULL && cntvar->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        arylen = cntvar->getNumericValue();

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_ARRAY);
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
            return finErrorCodeKits::FIN_EC_STATE_ERROR;
        }

        errcode = retitemvar->copyVariableValue(itemvar);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
    }

out:
    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_matrix(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *rowvar, *colvar;
    int matrow, matcol;
    int extcnt;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    rowvar = finExecVariable::transLinkTarget(env->findVariable("row"));
    colvar = finExecVariable::transLinkTarget(env->findVariable("col"));
    if ( rowvar == NULL || colvar == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( rowvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         colvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    matrow = (int)floor(rowvar->getNumericValue());
    matcol = (int)floor(colvar->getNumericValue());
    if ( matrow < 0 || matcol < 0 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::FIN_VR_TYPE_ARRAY);
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
            return finErrorCodeKits::FIN_EC_STATE_ERROR;
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
                return finErrorCodeKits::FIN_EC_STATE_ERROR;
            }

            errcode = itemvar->copyVariableValue(srcvar);
            if ( finErrorCodeKits::isErrorResult(errcode) ) {
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
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_transpose(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *matvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    matvar = finExecVariable::transLinkTarget(env->findVariable("mat"));
    if ( matvar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    int matrow = 0, matcol = 0;
    if ( !matvar->isNumericMatrix(&matrow, &matcol) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    errcode = retvar->preallocArrayLength(matcol);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }
    for ( int rowidx = 0; rowidx < matcol; rowidx++ ) {
        finExecVariable *retrowvar = retvar->getVariableItemAt(rowidx);
        if ( retrowvar == NULL ) {
            delete retvar;
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        }

        errcode = retrowvar->preallocArrayLength(matrow);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
        for ( int colidx = 0; colidx < matrow; colidx++ ) {
            finExecVariable *srcvar = matvar->getVariableItemAt(colidx)->getVariableItemAt(rowidx);
            finExecVariable *retcolvar = retrowvar->getVariableItemAt(colidx);
            if ( srcvar == NULL || retcolvar == NULL ) {
                delete retvar;
                return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
            }

            retcolvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            retcolvar->setNumericValue(srcvar->getNumericValue());
        }
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    int mat1row = 0, mat1col = 0, mat2row = 0, mat2col = 0;
    if ( !mat1var->isNumericMatrix(&mat1row, &mat1col) || !mat2var->isNumericMatrix(&mat2row, &mat2col) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( mat1row != mat2row || mat1col != mat2col )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    errcode = retvar->preallocArrayLength(mat1row);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }
    for ( int rowidx = 0; rowidx < mat1row; rowidx++ ) {
        finExecVariable *rowvar1 = mat1var->getVariableItemAt(rowidx);
        finExecVariable *rowvar2 = mat2var->getVariableItemAt(rowidx);
        finExecVariable *retrowvar = retvar->getVariableItemAt(rowidx);
        if ( rowvar1 == NULL || rowvar2 == NULL || retrowvar == NULL ) {
            delete retvar;
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        }

        errcode = retrowvar->preallocArrayLength(mat1col);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
        for ( int colidx = 0; colidx < mat1col; colidx++ ) {
            finExecVariable *colvar1 = rowvar1->getVariableItemAt(colidx);
            finExecVariable *colvar2 = rowvar2->getVariableItemAt(colidx);
            finExecVariable *retcolvar = retrowvar->getVariableItemAt(colidx);
            if ( colvar1 == NULL || colvar2 == NULL || retcolvar == NULL ) {
                delete retvar;
                return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
            }

            retcolvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            retcolvar->setNumericValue(colvar1->getNumericValue() + colvar2->getNumericValue());
        }
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_sub(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    int mat1row = 0, mat1col = 0, mat2row = 0, mat2col = 0;
    if ( !mat1var->isNumericMatrix(&mat1row, &mat1col) || !mat2var->isNumericMatrix(&mat2row, &mat2col) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( mat1row != mat2row || mat1col != mat2col )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    errcode = retvar->preallocArrayLength(mat1row);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }
    for ( int rowidx = 0; rowidx < mat1row; rowidx++ ) {
        finExecVariable *rowvar1 = mat1var->getVariableItemAt(rowidx);
        finExecVariable *rowvar2 = mat2var->getVariableItemAt(rowidx);
        finExecVariable *retrowvar = retvar->getVariableItemAt(rowidx);
        if ( rowvar1 == NULL || rowvar2 == NULL || retrowvar == NULL ) {
            delete retvar;
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        }

        errcode = retrowvar->preallocArrayLength(mat1col);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete retvar;
            return errcode;
        }
        for ( int colidx = 0; colidx < mat1col; colidx++ ) {
            finExecVariable *colvar1 = rowvar1->getVariableItemAt(colidx);
            finExecVariable *colvar2 = rowvar2->getVariableItemAt(colidx);
            finExecVariable *retcolvar = retrowvar->getVariableItemAt(colidx);
            if ( colvar1 == NULL || colvar2 == NULL || retcolvar == NULL ) {
                delete retvar;
                return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
            }

            retcolvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            retcolvar->setNumericValue(colvar1->getNumericValue() - colvar2->getNumericValue());
        }
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_mat_dot(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = finExecVariable::transLinkTarget(env->findVariable("mat1"));
    mat2var = finExecVariable::transLinkTarget(env->findVariable("mat2"));
    if ( mat1var == NULL || mat2var == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    int mat1row = 0, mat1col = 0, mat2row = 0, mat2col = 0;
    if ( !mat1var->isNumericMatrix(&mat1row, &mat1col) || !mat2var->isNumericMatrix(&mat2row, &mat2col) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    if ( mat1col != mat2row )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    errcode = retvar->preallocArrayLength(mat1row);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return errcode;
    }
    for ( int rowidx = 0; rowidx < mat1row; rowidx++ ) {
        finExecVariable *rowvar1 = mat1var->getVariableItemAt(rowidx);
        finExecVariable *retrowvar = retvar->getVariableItemAt(rowidx);
        if ( rowvar1 == NULL || retrowvar == NULL ) {
            delete retvar;
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
        }

        errcode = retrowvar->preallocArrayLength(mat2col);
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
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
                    return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
                }

                itemval += colvar1->getNumericValue() * colvar2->getNumericValue();
            }

            finExecVariable *retcolvar = retrowvar->getVariableItemAt(colidx);
            if ( retcolvar == NULL ) {
                delete retvar;
                return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
            }

            retcolvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            retcolvar->setNumericValue(itemval);
        }
    }

    retvar->clearLeftValue();
    retvar->setWriteProtected();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

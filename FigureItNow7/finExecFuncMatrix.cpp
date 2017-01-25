
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_mat_add(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncMatrixList[] = {
    { QString("mat_add"), QString("mat1,mat2"), _sysfunc_mat_add },

    { QString(), QString(), NULL }
};


finErrorCode finExecFunction::registSysFuncMatrix()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncMatrixList);
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
            finExecVariable *retcolvar = retrowvar->getVariableItemAt(rowidx);
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

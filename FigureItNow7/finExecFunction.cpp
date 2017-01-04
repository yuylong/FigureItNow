/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2016 Yusoft. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20161226    0  Yulong Yu    Create this file.
 */

#include "finExecFunction.h"

#include "finLexNode.h"
#include "finExecVariable.h"
#include "finExecFunction.h"
#include "finExecEnvironment.h"
#include "finExecOperartorClac.h"

finExecFunction::finExecFunction()
{
    this->_type = FIN_FN_TYPE_DUMMY;
    this->_u._rawPointer = NULL;
}

finExecFunctionType finExecFunction::getFunctionType() const
{
    return this->_type;
}

QString finExecFunction::getFunctionName() const
{
    return this->_funcName;
}

int finExecFunction::getParameterCount() const
{
    return this->_paramList.count();
}

QString finExecFunction::getParameterName(int idx) const
{
    return this->_paramList.at(idx);
}

finErrorCode finExecFunction::setFunctionType(finExecFunctionType type)
{
    if ( this->_type == type )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_type = type;
    this->_u._rawPointer = NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionName(const QString &funcname)
{
    if ( QString::compare(this->_funcName, funcname) == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_funcName = funcname;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::appendParameterName(const QString &paramname)
{
    if ( paramname.isEmpty() || paramname.isNull() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    this->_paramList.append(paramname);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::clearParameterNames()
{
    if ( this->_paramList.count() == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_paramList.clear();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionSyntaxNode(finSyntaxNode *funcnode)
{
    if ( this->_type != finExecFunction::FIN_FN_TYPE_USER )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_u._funcNode = funcnode;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::setFunctionCall(finFunctionCall funccall)
{
    if ( this->_type != finExecFunction::FIN_FN_TYPE_SYSTEM )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_u._funcCall = funccall;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFunction::execFunction(finSyntaxNode *curnode, finExecVariable **retval,
                                           finExecEnvironment *execenv)
{
    if ( curnode->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finLexNode *lexnode = curnode->getCommandLexNode();
    if ( lexnode == NULL || lexnode->getType() != finLexNode::FIN_LN_TYPE_OPERATOR ||
         lexnode->getOperator() != finLexNode::FIN_LN_OPTYPE_FUNCTION )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( curnode->getSubListCount() < 2 )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finSyntaxNode *fnsyntk = curnode->getSubSyntaxNode(0);
    if ( fnsyntk->getType() != finSyntaxNode::FIN_SN_TYPE_EXPRESS )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finLexNode *fnlexnd = fnsyntk->getCommandLexNode();
    if ( fnlexnd == NULL || fnlexnd->getType() != finLexNode::FIN_LN_TYPE_VARIABLE )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;


    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

static finErrorCode
_sysfunc_mat_add (finExecFunction *self, finExecVariable *retval, finExecEnvironment *execenv);

static struct {
    QString _funcName;
    QString _paramCsvList;
    finFunctionCall _funcCall;
} _finSystemFunctionList[] = {
    { QString("mat_add"), QString("mat1,mat2"), _sysfunc_mat_add },

    { QString(), QString(), NULL }
};

finErrorCode
finExecFunction::installSystemFunctions (finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int i, succcnt = 0;

    if ( rootenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    for ( i = 0; !_finSystemFunctionList[i]._funcName.isNull(); i++ ) {

        QStringList paramlist;
        finExecFunction *curfunc = new finExecFunction();
        if ( curfunc == NULL )
            goto item_bad;

        errcode = curfunc->setFunctionType(finExecFunction::FIN_FN_TYPE_SYSTEM);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = curfunc->setFunctionName(_finSystemFunctionList[i]._funcName);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        paramlist = _finSystemFunctionList[i]._paramCsvList.split(',');
        curfunc->clearParameterNames();
        for ( int j = 0; j < paramlist.count(); j++ ) {
            errcode = curfunc->appendParameterName(paramlist.at(i));
            if ( finErrorCodeKits::isErrorResult(errcode) )
                goto item_bad;
        }

        errcode = curfunc->setFunctionCall(_finSystemFunctionList[i]._funcCall);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        errcode = rootenv->addFunction(curfunc);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        succcnt++;
        continue;

item_bad:
        if ( curfunc != NULL )
            delete curfunc;
    }

    if ( succcnt == i ) {
        if ( succcnt > 0 )
            return finErrorCodeKits::FIN_EC_SUCCESS;
        else
            return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    } else {
        if ( succcnt > 0 )
            return finErrorCodeKits::FIN_EC_NORMAL_WARN;
        else
            return finErrorCodeKits::FIN_EC_NOT_FOUND;
    }
}

static finErrorCode
_sysfunc_mat_add (finExecFunction *self, finExecVariable *retval, finExecEnvironment *execenv)
{
    finExecVariable *mat1var, *mat2var;

    if ( self == NULL || retval == NULL || execenv == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    mat1var = execenv->findVariable("mat1");
    mat2var = execenv->findVariable("mat2");

    if ( mat1var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY ||
         mat2var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

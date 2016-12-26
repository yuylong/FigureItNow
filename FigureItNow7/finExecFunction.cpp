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
#include "finExecEnvironment.h"

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

finErrorCode finExecFunction::execFunction(finSyntaxNode *curnode, finExecVariable *retval,
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

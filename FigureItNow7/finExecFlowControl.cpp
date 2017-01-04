/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yusoft. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20170104    0  Yulong Yu    Create this file.
 */

#include "finExecFlowControl.h"


finExecFlowControl::finExecFlowControl()
{
    this->_type = finExecFlowControl::FIN_FC_NEXT;
    this->_label = QString();
}

finExecFlowControlType finExecFlowControl::getType() const
{
    return this->_type;
}

bool finExecFlowControl::isFlowNext() const
{
    return (this->_type == finExecFlowControl::FIN_FC_NEXT);
}

bool finExecFlowControl::isFlowGoto() const
{
    return (this->_type == finExecFlowControl::FIN_FC_GOTO);
}

QString finExecFlowControl::getGotoLabel() const
{
    if ( this->_type == finExecFlowControl::FIN_FC_GOTO )
        return this->_label;
    else
        return QString();
}

finErrorCode finExecFlowControl::setType(finExecFlowControlType type)
{
    this->_type = type;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setLabel(const QString &label)
{
    this->_label = label;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setFlowNext()
{
    this->_type = finExecFlowControl::FIN_FC_NEXT;
    this->_label = QString();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::setGotoAndLabel(const QString &label)
{
    this->_type = finExecFlowControl::FIN_FC_GOTO;
    this->_label = label;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecFlowControl::directPass()
{
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

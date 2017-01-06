/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2016 Yulong Yu. All rights reserved.
 *
 * History:
 *
 * DATE      REV  AUTHOR       COMMENTS
 * 20151229    0  Yulong Yu    Create this file.
 */

#include <QtMath>

#include "finExecVariable.h"

#include "finExecFunction.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecOperartorClac.h"


finExecVariable::finExecVariable()
    : _varName()
{
    this->_type = FIN_VR_TYPE_NULL;
    this->_writeProtect = false;
    this->_numVal = 0.0;
}

finExecVariable::finExecVariable(const QString &name)
    : _varName(name)
{
    this->_type = FIN_VR_TYPE_NULL;
    this->_writeProtect = false;
    this->_numVal = 0.0;
}

finExecVariable::~finExecVariable()
{
    this->dispose();
}

QString finExecVariable::getName() const
{
    return this->_varName;
}

bool finExecVariable::isSameName(const QString &name) const
{
    return (QString::compare(this->_varName, name) == 0);
}

finExecVariableType finExecVariable::getType() const
{
    return this->_type;
}

bool finExecVariable::isWriteProtected() const
{
    return this->_writeProtect;
}

bool finExecVariable::isLeftValue() const
{
    return this->_leftValue;
}

finErrorCode finExecVariable::setName(const QString &name)
{
    if ( name.isNull() || name.isEmpty() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( QString::compare(this->_varName, name) == 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_varName = name;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setType(finExecVariableType type)
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_type = type;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setWriteProtected()
{
    if ( this->_writeProtect == true )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_writeProtect = true;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setWriteProtected();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::clearWriteProtected()
{
    if ( this->_writeProtect == false )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_writeProtect = false;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->clearWriteProtected();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setLeftValue()
{
    if ( this->_leftValue == true )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_leftValue = true;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setLeftValue();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::clearLeftValue()
{
    if ( this->_leftValue == false )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_leftValue = false;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->clearLeftValue();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

double finExecVariable::getNumericValue() const
{
    if ( this->_type != FIN_VR_TYPE_NUMERIC )
        return 0.0;

    return this->_numVal;
}

QString finExecVariable::getStringValue() const
{
    if ( this->_type != FIN_VR_TYPE_STRING )
        return QString();

    return this->_strVal;
}

int finExecVariable::getArrayLength() const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return 0;

    return this->_itemList.count();
}

finErrorCode finExecVariable::setNumericValue(double val)
{
    if ( this->_type != FIN_VR_TYPE_NUMERIC && this->_type != FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_NUMERIC;

    this->_numVal = val;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setStringValue(const QString &strval)
{
    if ( this->_type != FIN_VR_TYPE_STRING && this->_type != FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_STRING;

    this->_strVal = strval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::preallocArrayLength(int len)
{
    if ( this->_type != FIN_VR_TYPE_ARRAY && this->_type != FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_ARRAY;

    if ( this->_itemList.count() >= len )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    while ( this->_itemList.count() < len ) {
        finExecVariable *subvar = new finExecVariable();
        if ( subvar == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

        subvar->_writeProtect = this->_writeProtect;
        subvar->_leftValue = this->_leftValue;
        this->_itemList.append(subvar);
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecVariable *finExecVariable::getVariableItemAt(int idx) const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return NULL;

    if ( idx < 0 || idx >= this->_itemList.count() )
        return NULL;

    return this->_itemList.at(idx);
}

finExecVariable *finExecVariable::getVariableItemAt(int idx)
{
    if ( this->_type != FIN_VR_TYPE_ARRAY && this->_type != FIN_VR_TYPE_NULL )
        return NULL;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_ARRAY;
    if ( idx < 0 )
        return NULL;

    if ( idx < this->_itemList.count() )
        return this->_itemList.at(idx);

    finErrorCode errcode = this->preallocArrayLength(idx + 1);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return NULL;

    return this->_itemList.at(idx);
}

bool finExecVariable::isNumericMatrix(int *rowcnt, int *colcnt) const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return false;

    int pcolcnt = 0;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);

        if ( curitem->_type != FIN_VR_TYPE_ARRAY )
            return false;

        if ( i == 0 )
            pcolcnt = curitem->_itemList.count();
        else if ( pcolcnt != curitem->_itemList.count() )
            return false;

        for ( int j = 0; j < pcolcnt; j++) {
            finExecVariable *chditem = curitem->_itemList.at(j);
            if ( chditem->_type != FIN_VR_TYPE_NUMERIC )
                return false;
        }
    }

    if ( rowcnt != NULL )
        *rowcnt = this->_itemList.count();
    if ( colcnt != NULL )
        *colcnt = pcolcnt;
    return true;
}

bool finExecVariable::isNumericArray(int *cnt) const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return false;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);
        if ( curitem->_type != FIN_VR_TYPE_NUMERIC )
            return false;
    }

    if ( cnt != NULL )
        *cnt = this->_itemList.count();
    return true;
}

bool finExecVariable::isStringArray(int *cnt) const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return false;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);
        if ( curitem->_type != FIN_VR_TYPE_STRING )
            return false;
    }

    if ( cnt != NULL )
        *cnt = this->_itemList.count();
    return true;
}

const finExecVariable *finExecVariable::getLinkTarget() const
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_LINK )
        return this;

    finExecVariable *target = this->_linkTarget;
    while ( target != NULL && target->getType() == finExecVariable::FIN_VR_TYPE_LINK ) {
        target = target->_linkTarget;
    }
    return target;
}

finExecVariable *finExecVariable::getLinkTarget()
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_LINK )
        return this;

    finExecVariable *target = this->_linkTarget;
    while ( target != NULL && target->getType() == finExecVariable::FIN_VR_TYPE_LINK ) {
        target = target->_linkTarget;
    }
    return target;
}

finErrorCode finExecVariable::setLinkTarget(finExecVariable *target)
{
    if ( this->_type == finExecVariable::FIN_VR_TYPE_LINK && this->_linkTarget == target )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    finErrorCode errcode = this->unsetLinkTarget();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->_linkTarget = target;
    if ( target != NULL )
        target->_linkedList.append(this);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::unsetLinkTarget()
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_LINK && this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_type == finExecVariable::FIN_VR_TYPE_NULL ) {
        this->_type = finExecVariable::FIN_VR_TYPE_LINK;
        this->_linkTarget = NULL;
        return finErrorCodeKits::FIN_EC_SUCCESS;
    }

    if ( this->_linkTarget == NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    for ( int i = this->_linkTarget->_linkedList.count() - 1; i >= 0; i-- ) {
        if ( this->_linkTarget->_linkedList.at(i) == this )
            this->_linkTarget->_linkedList.removeAt(i);
    }
    this->_linkTarget = NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariableValue(finExecVariable *srcvar)
{
    if ( srcvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_type != FIN_VR_TYPE_NULL || this->_type != srcvar->getType() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finErrorCode errcode;
    this->_type = srcvar->getType();

    switch ( srcvar->_type ) {
      case FIN_VR_TYPE_NUMERIC:
        this->_numVal = srcvar->getNumericValue();
        break;

      case FIN_VR_TYPE_STRING:
        this->_strVal = srcvar->getStringValue();
        break;

      case FIN_VR_TYPE_ARRAY:
        errcode = this->copyArrayVariable(srcvar);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        break;

      case FIN_VR_TYPE_LINK:
        errcode = this->setLinkTarget(srcvar->getLinkTarget());
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        break;

      default:
        return finErrorCodeKits::FIN_EC_READ_ERROR;
        break;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariable(finExecVariable *srcvar)
{
    finErrorCode errcode = this->copyVariableValue(srcvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->_varName = srcvar->getName();
    this->_leftValue = srcvar->isLeftValue();
    this->_writeProtect = srcvar->isWriteProtected();

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::copyArrayVariable(const finExecVariable *srcvar)
{
    finErrorCode errcode;

    // Due to the private property, I omit the sanity check.

    for ( int i = 0; i < srcvar->_itemList.count(); i++ ) {
        finExecVariable *subsrcvar = srcvar->_itemList.at(i);
        finExecVariable *subdstvar = new finExecVariable();

        errcode = subdstvar->copyVariable(subsrcvar);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        this->_itemList.append(subdstvar);
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::dispose()
{
    if ( this->_type == FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_type = FIN_VR_TYPE_NULL;
    this->_strVal.clear();

    while ( !this->_itemList.empty() ) {
        finExecVariable *subvar = this->_itemList.first();
        this->_itemList.removeFirst();
        subvar->dispose();
        delete subvar;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finExecVariable::releaseNonLeftVariable(finExecVariable *var)
{
    if ( var == NULL )
        return;

    if ( !var->isLeftValue() )
        delete var;
}

static finExecVariable *_sysvar_pi();
static finExecVariable *_sysvar_e();

typedef finExecVariable *(*_finSysvarGencall)();

_finSysvarGencall _finSysvarGencallList[] = {
    _sysvar_pi,
    _sysvar_e,
    NULL
};

finErrorCode
finExecVariable::installSystemVariables (finExecEnvironment *rootenv)
{
    finErrorCode errcode;
    int success = 0, failed = 0;

    for ( int i = 0; _finSysvarGencallList[i] != NULL; i++ ) {
        finExecVariable *curvar = _finSysvarGencallList[i]();
        if ( curvar == NULL )
            goto item_bad;

        errcode = rootenv->addVariable(curvar);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto item_bad;

        success++;

item_bad:
        if ( curvar != NULL )
            delete curvar;
        failed++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorCodeKits::FIN_EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    else if ( failed != 0 )
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    else
        return finErrorCodeKits::FIN_EC_SUCCESS;
}

static inline finExecVariable *
_sysvar_gen_num_var(const QString &name, double val)
{
    finExecVariable *retvar = new finExecVariable();
    finErrorCode errcode;

    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setName(name);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setNumericValue(val);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto err;

    return retvar;

err:
    delete retvar;
    return NULL;
}

static finExecVariable *_sysvar_pi()
{
    return _sysvar_gen_num_var(QString("PI"), M_PI);
}

static finExecVariable *_sysvar_e()
{
    return _sysvar_gen_num_var(QString("E"), M_E);
}

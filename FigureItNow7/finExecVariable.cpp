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
    this->_linkTarget = NULL;
    this->_parentVar = NULL;
}

finExecVariable::finExecVariable(const QString &name)
    : _varName(name)
{
    this->_type = FIN_VR_TYPE_NULL;
    this->_writeProtect = false;
    this->_numVal = 0.0;
    this->_linkTarget = NULL;
    this->_parentVar = NULL;
}

finExecVariable::~finExecVariable()
{
    this->removeFromArray();
    this->clearLinkedVariables();
    this->dispose();
}

QString finExecVariable::getName() const
{
    return this->_varName;
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

finErrorCode finExecVariable::setupWriteProtected(bool blval)
{
    this->_writeProtect = blval;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setWriteProtected();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setWriteProtected()
{
    return this->setupWriteProtected(true);
}

finErrorCode finExecVariable::clearWriteProtected()
{
    return this->setupWriteProtected(false);
}

finErrorCode finExecVariable::setupLeftValue(bool blval)
{
    this->_leftValue = blval;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setLeftValue();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setLeftValue()
{
    return this->setupLeftValue(true);
}

finErrorCode finExecVariable::clearLeftValue()
{
    return this->setupLeftValue(false);
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
        subvar->_parentVar = this;
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

bool finExecVariable::isVariableInside(const finExecVariable *var) const
{
    if ( this == var )
        return true;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *chdvar = this->_itemList.at(i);

        if ( chdvar->isVariableInside(var) )
            return true;
    }
    return false;
}

finErrorCode finExecVariable::clearArrayItems()
{
    if ( this->_type != FIN_VR_TYPE_ARRAY && this->_type != FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_ARRAY;

    if ( this->_itemList.count() <= 0 )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    while ( !this->_itemList.empty() ) {
        finExecVariable *itemvar = this->_itemList.first();
        this->_itemList.removeFirst();
        delete itemvar;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finExecVariable::isInArray() const
{
    return (this->_parentVar != NULL);
}

finExecVariable *finExecVariable::getParentVariable() const
{
    return this->_parentVar;
}

finErrorCode finExecVariable::removeFromArray()
{
    if ( this->_parentVar == NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    for ( int i = 0; i < this->_parentVar->_itemList.count(); i++ ) {
        if ( this->_parentVar->_itemList.at(i) == this ) {
            this->_parentVar->_itemList.removeAt(i);
            break;
        }
    }
    this->_parentVar = NULL;

    this->clearLeftValue();
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

finExecVariable *finExecVariable::transLinkTarget(finExecVariable *var)
{
    if ( var == NULL )
        return NULL;
    else
        return var->getLinkTarget();
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

finErrorCode finExecVariable::readColorValue(QColor *color) const
{
    if ( color == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finExecVariable::setupColorValue(const QColor &color)
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    return finErrorCode::FIN_EC_NON_IMPLEMENT;
}

finErrorCode finExecVariable::copyVariableValueIn(finExecVariable *srcvar)
{
    if ( srcvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_type != FIN_VR_TYPE_NULL && this->_type != srcvar->getType() )
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

bool finExecVariable::isSameName(const QString &name) const
{
    return (QString::compare(this->_varName, name) == 0);
}

bool finExecVariable::isSameValue(finExecVariable *var)
{
    finExecVariable *var1 = this->getLinkTarget();
    finExecVariable *var2 = var->getLinkTarget();

    if ( var1 == NULL && var2 == NULL )
        return true;
    else if ( var1 == NULL || var2 == NULL )
        return false;
    else if ( var1 == var2 )
        return true;

    if ( var1->getType() != var2->getType() )
        return false;

    switch ( var1->getType() ) {
      case finExecVariable::FIN_VR_TYPE_NULL:
        return true;

      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        return (var1->getNumericValue() == var2->getNumericValue());
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        return (QString::compare(var1->getStringValue(), var2->getStringValue()) == 0);
        break;

      case finExecVariable::FIN_VR_TYPE_ARRAY:
        if ( var1->getArrayLength() != var2->getArrayLength() )
            return false;

        for ( int i = 0; i < var1->getArrayLength(); i++ ) {
            finExecVariable *subvar1 = var1->getVariableItemAt(i);
            finExecVariable *subvar2 = var2->getVariableItemAt(i);
            if ( !subvar1->isSameValue(subvar2) )
                return false;
        }
        return true;
        break;

      default:
        return false;
    }
    return false;
}

finErrorCode finExecVariable::copyArrayVariable(const finExecVariable *srcvar)
{
    finErrorCode errcode = this->clearArrayItems();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    for ( int i = 0; i < srcvar->_itemList.count(); i++ ) {
        finExecVariable *subsrcvar = srcvar->_itemList.at(i);
        finExecVariable *subdstvar = new finExecVariable();

        errcode = subdstvar->copyVariableValueIn(subsrcvar);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;

        this->_itemList.append(subdstvar);
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariableValue(finExecVariable *srcvar)
{
    finErrorCode errcode = this->copyVariableValueIn(srcvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    // To keep the access mode of variables in array.
    this->setupWriteProtected(this->_writeProtect);
    this->setupLeftValue(this->_leftValue);

    return finErrorCodeKits::FIN_EC_STATE_ERROR;
}

finErrorCode finExecVariable::copyVariable(finExecVariable *srcvar)
{
    finErrorCode errcode = this->copyVariableValueIn(srcvar);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    this->setName(srcvar->getName());
    this->setupWriteProtected(srcvar->isWriteProtected());
    this->setupLeftValue(srcvar->isLeftValue());

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::disposeValue()
{
    if ( this->_type == FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    switch ( this->_type ) {
      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        this->_numVal = 0.0;
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        this->_strVal.clear();
        break;

      case finExecVariable::FIN_VR_TYPE_ARRAY:
        this->clearArrayItems();
        break;

      case finExecVariable::FIN_VR_TYPE_LINK:
        this->unsetLinkTarget();
        break;

      default:
        break;
    }

    this->_type = FIN_VR_TYPE_NULL;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::dispose()
{
    this->disposeValue();

    this->setName(QString());
    this->clearWriteProtected();
    this->clearLeftValue();

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::clearLinkedVariables()
{
    while ( !this->_linkedList.empty() ) {
        int cnt = this->_linkedList.count();
        finExecVariable *lvar = this->_linkedList.first();
        lvar->unsetLinkTarget();

        if ( this->_linkedList.count() >= cnt )
            this->_linkedList.removeFirst();
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finExecVariable *finExecVariable::buildNonLeftVariable(finExecVariable *var)
{
    if ( var == NULL )
        return NULL;

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != NULL )
        var = realvar;

    if ( !var->isLeftValue() && !var->isInArray() )
        return var;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    finErrorCode errcode = retvar->copyVariableValueIn(var);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return NULL;
    }

    retvar->setWriteProtected();
    retvar->clearLeftValue();
    return retvar;
}

finExecVariable *finExecVariable::buildCopyLeftVariable(finExecVariable *var)
{
    if ( var == NULL ) {
        var = new finExecVariable();
        if ( var == NULL )
            return NULL;
        var->clearLeftValue();
    }

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != NULL )
        var = realvar;

    if ( !var->isLeftValue() && !var->isInArray() ) {
        var->setLeftValue();
        var->clearWriteProtected();
        return var;
    }

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    finErrorCode errcode = retvar->copyVariableValueIn(var);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return NULL;
    }

    retvar->setLeftValue();
    retvar->clearWriteProtected();
    return retvar;
}

finExecVariable *finExecVariable::buildLinkLeftVariable(finExecVariable *var)
{
    if ( var == NULL ) {
        var = new finExecVariable();
        if ( var == NULL )
            return NULL;
        var->clearLeftValue();
    }

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != NULL )
        var = realvar;

    if ( !var->isLeftValue() )
        return buildCopyLeftVariable(var);

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    finErrorCode errcode = retvar->setLinkTarget(var);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete retvar;
        return NULL;
    }

    retvar->setLeftValue();
    retvar->clearWriteProtected();
    return retvar;
}

finExecVariable *finExecVariable::buildFuncReturnVariable(finExecVariable *var, finExecEnvironment *env)
{
    if ( var == NULL )
        return NULL;

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != NULL )
        var = realvar;

    finErrorCode errcode;
    if ( !var->isLeftValue() ) {
        errcode = var->removeFromArray();
        if ( finErrorCodeKits::isErrorResult(errcode) )
            goto copy_var;

        return var;
    }

    if ( !env->isVariableInEnv(var) )
        return var;

    errcode = var->removeFromArray();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto copy_var;

    errcode = env->removeVariable(var);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        goto copy_var;

    var->clearLeftValue();
    return var;

copy_var:
    finExecVariable *clonevar = new finExecVariable();
    if ( clonevar == NULL )
        return NULL;

    clonevar->copyVariableValue(var);
    clonevar->setWriteProtected();
    clonevar->clearLeftValue();
    return clonevar;
}

void finExecVariable::releaseNonLeftVariable(finExecVariable *var)
{
    if ( var == NULL )
        return;

    if ( !var->isLeftValue() )
        delete var;
}

static finExecVariable *_sysvar_nil();
static finExecVariable *_sysvar_pi();
static finExecVariable *_sysvar_e();

typedef finExecVariable *(*_finSysvarGencall)();

_finSysvarGencall _finSysvarGencallList[] = {
    _sysvar_nil,
    _sysvar_pi,
    _sysvar_e,
    NULL
};

finErrorCode
finExecVariable::installSystemVariables(finExecEnvironment *rootenv)
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
        continue;

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

static finExecVariable *_sysvar_nil()
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setType(finExecVariable::FIN_VR_TYPE_NULL);
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

static inline finExecVariable *
_sysvar_gen_num_var(const QString &name, double val)
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
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

/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include <QtMath>

#include "finExecVariable.h"

#include "finExecFunction.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecOperartorClac.h"


finExecVariable::finExecVariable()
    : _varName(), _itemList()
{
    this->_type = FIN_VR_TYPE_NULL;
    this->_leftValue = false;
    this->_writeProtect = false;
    this->_numVal = 0.0;
    this->_strVal = QString();
    this->_image = QImage();
    this->_linkTarget = NULL;
    this->_parentVar = NULL;
}

finExecVariable::finExecVariable(const QString &name)
    : _varName(name), _itemList()
{
    this->_type = FIN_VR_TYPE_NULL;
    this->_leftValue = false;
    this->_writeProtect = false;
    this->_numVal = 0.0;
    this->_strVal = QString();
    this->_image = QImage();
    this->_linkTarget = NULL;
    this->_parentVar = NULL;
}

finExecVariable::~finExecVariable()
{
    this->removeFromArray();
    this->clearLinkedVariables();
    this->dispose();
}

const QString &finExecVariable::getName() const
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
        return finErrorKits::EC_INVALID_PARAM;

    if ( QString::compare(this->_varName, name) == 0 )
        return finErrorKits::EC_DUPLICATE_OP;

    this->_varName = name;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setType(finExecVariableType type)
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    this->_type = type;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setupWriteProtected(bool blval)
{
    this->_writeProtect = blval;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setupWriteProtected(blval);
    }
    return finErrorKits::EC_SUCCESS;
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
        this->_itemList.at(i)->setupLeftValue(blval);
    }
    return finErrorKits::EC_SUCCESS;
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

QImage finExecVariable::getImageValue() const
{
    if ( this->_type != FIN_VR_TYPE_IMAGE )
        return QImage();

    return this->_image;
}

finErrorCode finExecVariable::setNumericValue(double val)
{
    if ( this->_type != FIN_VR_TYPE_NUMERIC && this->_type != FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_NUMERIC;

    this->_numVal = val;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setStringValue(const QString &strval)
{
    if ( this->_type != FIN_VR_TYPE_STRING && this->_type != FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_STRING;

    this->_strVal = strval;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setImageValue(const QImage &img)
{
    if ( this->_type != FIN_VR_TYPE_IMAGE && this->_type != FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_IMAGE;

    this->_image = img;
    return finErrorKits::EC_SUCCESS;
}

int finExecVariable::getArrayLength() const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return 0;

    return this->_itemList.count();
}

finErrorCode finExecVariable::preallocArrayLength(int len)
{
    if ( this->_type != FIN_VR_TYPE_ARRAY && this->_type != FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_ARRAY;

    if ( this->_itemList.count() >= len )
        return finErrorKits::EC_DUPLICATE_OP;

    while ( this->_itemList.count() < len ) {
        finExecVariable *subvar = new finExecVariable();
        if ( subvar == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;

        subvar->_writeProtect = this->_writeProtect;
        subvar->_leftValue = this->_leftValue;
        subvar->_parentVar = this;
        this->_itemList.append(subvar);
    }
    return finErrorKits::EC_SUCCESS;
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
    if ( finErrorKits::isErrorResult(errcode) )
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
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == FIN_VR_TYPE_NULL )
        this->_type = FIN_VR_TYPE_ARRAY;

    if ( this->_itemList.count() <= 0 )
        return finErrorKits::EC_DUPLICATE_OP;

    while ( !this->_itemList.empty() ) {
        finExecVariable *itemvar = this->_itemList.first();
        this->_itemList.removeFirst();
        delete itemvar;
    }
    return finErrorKits::EC_SUCCESS;
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
        return finErrorKits::EC_DUPLICATE_OP;

    for ( int i = 0; i < this->_parentVar->_itemList.count(); i++ ) {
        if ( this->_parentVar->_itemList.at(i) == this ) {
            this->_parentVar->_itemList.removeAt(i);
            break;
        }
    }
    this->_parentVar = NULL;

    this->clearLeftValue();
    return finErrorKits::EC_SUCCESS;
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
        return finErrorKits::EC_DUPLICATE_OP;

    finErrorCode errcode = this->unsetLinkTarget();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    this->_linkTarget = target;
    if ( target != NULL )
        target->_linkedList.append(this);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::unsetLinkTarget()
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_LINK && this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    if ( this->_type == finExecVariable::FIN_VR_TYPE_NULL ) {
        this->_type = finExecVariable::FIN_VR_TYPE_LINK;
        this->_linkTarget = NULL;
        return finErrorKits::EC_SUCCESS;
    }

    if ( this->_linkTarget == NULL )
        return finErrorKits::EC_DUPLICATE_OP;

    for ( int i = this->_linkTarget->_linkedList.count() - 1; i >= 0; i-- ) {
        if ( this->_linkTarget->_linkedList.at(i) == this )
            this->_linkTarget->_linkedList.removeAt(i);
    }
    this->_linkTarget = NULL;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::readBoolValue(bool *blval) const
{
    if ( blval == NULL )
        return finErrorKits::EC_NULL_POINTER;

    switch ( this->getType() ) {
      case finExecVariable::FIN_VR_TYPE_NULL:
        *blval = false;
        break;

      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        *blval = !(this->getNumericValue() < 1.0e-8 && this->getNumericValue() > -1.0e-8);
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        *blval = (QString::compare(this->getStringValue(), "yes", Qt::CaseInsensitive) == 0 ||
                  QString::compare(this->getStringValue(), "true", Qt::CaseInsensitive) == 0);
        break;

      case finExecVariable::FIN_VR_TYPE_IMAGE:
        *blval = !this->getImageValue().isNull();
        break;

      case finExecVariable::FIN_VR_TYPE_ARRAY:
        *blval = (this->getArrayLength() > 0);
        break;

      default:
        *blval = false;
        break;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setupBoolValue(bool blval)
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_NULL &&
         this->_type != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorKits::EC_STATE_ERROR;

    this->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    if ( blval ) {
        this->setNumericValue(1.0);
    } else {
        this->setNumericValue(0.0);
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::readColorValue(QColor *color) const
{
    if ( color == NULL )
        return finErrorKits::EC_NULL_POINTER;

    int arylen = 0;
    if ( !this->isNumericArray(&arylen) )
        return finErrorKits::EC_INVALID_PARAM;
    if ( arylen != 3 && arylen != 4 )
        return finErrorKits::EC_INVALID_PARAM;

    double red, green, blue;
    red = this->getVariableItemAt(0)->getNumericValue();
    green = this->getVariableItemAt(1)->getNumericValue();
    blue = this->getVariableItemAt(2)->getNumericValue();
    red = (red < 0.0 ? 0.0 : (red > 1.0 ? 1.0 : red));
    green = (green < 0.0 ? 0.0 : (green > 1.0 ? 1.0 : green));
    blue = (blue < 0.0 ? 0.0 : (blue > 1.0 ? 1.0 : blue));

    double alpha = 1.0;
    if ( arylen == 4 ) {
        alpha = this->getVariableItemAt(3)->getNumericValue();
        alpha = (alpha < 0.0 ? 0.0 : (alpha > 1.0 ? 1.0 : alpha));
    }

    color->setRgbF(red, green, blue, alpha);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::setupColorValue(const QColor &color)
{
    if ( this->_type != finExecVariable::FIN_VR_TYPE_NULL )
        return finErrorKits::EC_STATE_ERROR;

    finErrorCode errcode = this->preallocArrayLength(4);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finExecVariable *subvar = this->getVariableItemAt(0);
    subvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    subvar->setNumericValue(color.redF());

    subvar = this->getVariableItemAt(1);
    subvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    subvar->setNumericValue(color.greenF());

    subvar = this->getVariableItemAt(2);
    subvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    subvar->setNumericValue(color.blueF());

    subvar = this->getVariableItemAt(3);
    subvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    subvar->setNumericValue(color.alphaF());

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariableValueIn(finExecVariable *srcvar)
{
    if ( srcvar == NULL )
        return finErrorKits::EC_NULL_POINTER;

    if ( this->_type != FIN_VR_TYPE_NULL && this->_type != srcvar->getType() )
        return finErrorKits::EC_STATE_ERROR;

    finErrorCode errcode;
    this->_type = srcvar->getType();

    switch ( srcvar->_type ) {
      case FIN_VR_TYPE_NUMERIC:
        this->_numVal = srcvar->getNumericValue();
        break;

      case FIN_VR_TYPE_STRING:
        this->_strVal = srcvar->getStringValue();
        break;

      case FIN_VR_TYPE_IMAGE:
        this->_image = srcvar->getImageValue();
        break;

      case FIN_VR_TYPE_ARRAY:
        errcode = this->copyArrayVariable(srcvar);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        break;

      case FIN_VR_TYPE_LINK:
        errcode = this->setLinkTarget(srcvar->getLinkTarget());
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        break;

      default:
        return finErrorKits::EC_READ_ERROR;
        break;
    }
    return finErrorKits::EC_SUCCESS;
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

      case finExecVariable::FIN_VR_TYPE_IMAGE:
        return false;
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
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    this->preallocArrayLength(srcvar->_itemList.count());
    for ( int i = 0; i < srcvar->_itemList.count(); i++ ) {
        finExecVariable *subsrcvar = srcvar->_itemList.at(i);
        finExecVariable *subdstvar = this->getVariableItemAt(i);

        errcode = subdstvar->copyVariableValueIn(subsrcvar);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariableValue(finExecVariable *srcvar)
{
    finErrorCode errcode;
    errcode = this->disposeValue();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    if ( srcvar == NULL )
        return finErrorKits::EC_NORMAL_WARN;

    errcode = this->copyVariableValueIn(srcvar);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    // To keep the access mode of variables in array.
    this->setupWriteProtected(this->_writeProtect);
    this->setupLeftValue(this->_leftValue);

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::smartCopyVariableValue(finExecVariable *srcvar)
{
    finErrorCode errcode;
    errcode = this->disposeValue();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    if ( srcvar == NULL )
        return finErrorKits::EC_NORMAL_WARN;

    if ( srcvar->getType() == finExecVariable::FIN_VR_TYPE_ARRAY &&
         !srcvar->isLeftValue() ) {
        this->_itemList = srcvar->_itemList;
        srcvar->_itemList.clear();

        finExecVariable *itemvar;
        foreach ( itemvar, this->_itemList ) {
            itemvar->_parentVar = this;
        }
        this->_type = finExecVariable::FIN_VR_TYPE_ARRAY;
    } else {
        errcode = this->copyVariableValueIn(srcvar);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    // To keep the access mode of variables in array.
    this->setupWriteProtected(this->_writeProtect);
    this->setupLeftValue(this->_leftValue);

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariable(finExecVariable *srcvar)
{
    finErrorCode errcode = this->copyVariableValueIn(srcvar);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    this->setName(srcvar->getName());
    this->setupWriteProtected(srcvar->isWriteProtected());
    this->setupLeftValue(srcvar->isLeftValue());

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::disposeValue()
{
    if ( this->_type == FIN_VR_TYPE_NULL )
        return finErrorKits::EC_DUPLICATE_OP;

    switch ( this->_type ) {
      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        this->_numVal = 0.0;
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        this->_strVal.clear();
        break;

      case finExecVariable::FIN_VR_TYPE_IMAGE:
        this->_image = QImage();
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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::dispose()
{
    this->disposeValue();

    this->setName(QString());
    this->clearWriteProtected();
    this->clearLeftValue();

    return finErrorKits::EC_SUCCESS;
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
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::transToPointListArray(finExecVariable *aryvar, QList<QPointF> *ptlist)
{
    if ( ptlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    if ( aryvar == NULL || aryvar->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        ptlist->clear();
        return finErrorKits::EC_SUCCESS;
    }

    int arylen = 0;
    if ( !aryvar->isNumericArray(&arylen) )
        return finErrorKits::EC_INVALID_PARAM;

    ptlist->clear();
    for ( int i = 0; i + 1 < arylen; i += 2 ) {
        finExecVariable *varx = aryvar->getVariableItemAt(i);
        finExecVariable *vary = aryvar->getVariableItemAt(i + 1);

        ptlist->append(QPoint(varx->getNumericValue(), vary->getNumericValue()));
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::transToPointListMatrix(finExecVariable *matvar, QList<QPointF> *ptlist)
{
    if ( ptlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    if ( matvar == NULL || matvar->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        ptlist->clear();
        return finErrorKits::EC_SUCCESS;
    }

    int row = 0, col = 0;
    if ( !matvar->isNumericMatrix(&row, &col) )
        return finErrorKits::EC_INVALID_PARAM;
    if ( col < 2 )
        return finErrorKits::EC_INVALID_PARAM;

    ptlist->clear();
    for ( int i = 0; i < row; i++ ) {
        finExecVariable *ptvar = matvar->getVariableItemAt(i);
        finExecVariable *varx = ptvar->getVariableItemAt(0);
        finExecVariable *vary = ptvar->getVariableItemAt(1);

        ptlist->append(QPointF(varx->getNumericValue(), vary->getNumericValue()));
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finExecVariable::transToPointList(finExecVariable *var, QList<QPointF> *ptlist)
{
    if ( ptlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    if ( var == NULL || var->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        ptlist->clear();
        return finErrorKits::EC_SUCCESS;
    }

    if ( var->getType() != finExecVariable::FIN_VR_TYPE_ARRAY )
        return finErrorKits::EC_INVALID_PARAM;

    if ( var->getArrayLength() > 0 && var->getVariableItemAt(0)->getType() == finExecVariable::FIN_VR_TYPE_ARRAY )
        return transToPointListMatrix(var, ptlist);
    else
        return transToPointListArray(var, ptlist);
}

finErrorCode finExecVariable::transToPointList(
        finExecVariable *xvar, finExecVariable *yvar, QList<QPointF> *ptlist)
{
    if ( ptlist == NULL )
        return finErrorKits::EC_NULL_POINTER;

    if ( yvar == NULL || yvar->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        if ( xvar == NULL || xvar->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
            ptlist->clear();
            return finErrorKits::EC_SUCCESS;
        } else {
             return transToPointList(xvar, ptlist);
        }
    }

    int arylen = 0, yarylen = 0;
    if ( !xvar->isNumericArray(&arylen) || !yvar->isNumericArray(&yarylen) )
        return finErrorKits::EC_INVALID_PARAM;
    if ( arylen > yarylen )
        arylen = yarylen;

    ptlist->clear();
    for ( int i = 0; i < arylen; i++ ) {
        finExecVariable *xitem = xvar->getVariableItemAt(i);
        finExecVariable *yitem = yvar->getVariableItemAt(i);

        ptlist->append(QPointF(xitem->getNumericValue(), yitem->getNumericValue()));
    }
    return finErrorKits::EC_SUCCESS;
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
    if ( finErrorKits::isErrorResult(errcode) ) {
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

    if ( !var->isLeftValue() ) {
        if ( var->isInArray() )
            var->removeFromArray();
        var->setLeftValue();
        var->clearWriteProtected();
        return var;
    }

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    finErrorCode errcode = retvar->copyVariableValueIn(var);
    if ( finErrorKits::isErrorResult(errcode) ) {
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
    if ( finErrorKits::isErrorResult(errcode) ) {
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
        if ( finErrorKits::isErrorResult(errcode) )
            goto copy_var;

        return var;
    }

    if ( !env->isVariableInEnv(var) )
        return var;

    errcode = var->removeFromArray();
    if ( finErrorKits::isErrorResult(errcode) )
        goto copy_var;

    errcode = env->removeVariable(var);
    if ( finErrorKits::isErrorResult(errcode) )
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
        if ( finErrorKits::isErrorResult(errcode) )
            goto item_bad;

        success++;
        continue;

item_bad:
        if ( curvar != NULL )
            delete curvar;
        failed++;
    }

    if ( success == 0 && failed == 0 )
        return finErrorKits::EC_REACH_BOTTOM;
    else if ( success == 0 )
        return finErrorKits::EC_NOT_FOUND;
    else if ( failed != 0 )
        return finErrorKits::EC_NORMAL_WARN;
    else
        return finErrorKits::EC_SUCCESS;
}

static finExecVariable *_sysvar_nil()
{
    finErrorCode errcode;
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    errcode = retvar->setType(finExecVariable::FIN_VR_TYPE_NULL);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
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
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setNumericValue(val);
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setLeftValue();
    if ( finErrorKits::isErrorResult(errcode) )
        goto err;

    errcode = retvar->setWriteProtected();
    if ( finErrorKits::isErrorResult(errcode) )
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

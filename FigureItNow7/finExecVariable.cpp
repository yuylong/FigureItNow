/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, May 26th, 2016
 * Copyright(c) 2015-2026 Yulong Yu. All rights reserved.
 */

/*! \file finExecVariable.cpp
 *  \brief Implementations of FIN-script runtime variable storage and ownership.
 *
 *  Provides typed value storage, arrays and symbolic links, value conversion, deep-copy and transfer
 *  operations, and helpers that prepare runtime values for operators, functions, and plot routines.
 */

#include <QtMath>

#include "finExecVariable.h"

#include "finExecFunction.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecOperartorCalc.h"


finExecVariable::finExecVariable()
    : _varName(), _itemList()
{
    this->_type = TP_NULL;
    this->_leftValue = false;
    this->_writeProtect = false;
    this->_numVal = 0.0;
    this->_strVal = QString();
    this->_image = QImage();
    this->_linkTarget = nullptr;
    this->_parentVar = nullptr;
}

finExecVariable::finExecVariable(const QString &name)
    : _varName(name), _itemList()
{
    this->_type = TP_NULL;
    this->_leftValue = false;
    this->_writeProtect = false;
    this->_numVal = 0.0;
    this->_strVal = QString();
    this->_image = QImage();
    this->_linkTarget = nullptr;
    this->_parentVar = nullptr;
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

void finExecVariable::setName(const QString &name)
{
    if ( name.isNull() || name.isEmpty() )
        finWarning << "Variable (" << this->_varName << ") name is set to be empty.";

    if ( QString::compare(this->_varName, name) == 0 )
        return;

    this->_varName = name;
}

void finExecVariable::setType(finExecVariableType type)
{
    if ( this->_type != finExecVariable::TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Variable type is already set.");

    this->_type = type;
}

void finExecVariable::setupWriteProtected(bool blval)
{
    this->_writeProtect = blval;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setupWriteProtected(blval);
    }
}

void finExecVariable::setWriteProtected()
{
    this->setupWriteProtected(true);
}

void finExecVariable::clearWriteProtected()
{
    this->setupWriteProtected(false);
}

void finExecVariable::setupLeftValue(bool blval)
{
    this->_leftValue = blval;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        this->_itemList.at(i)->setupLeftValue(blval);
    }
}

void finExecVariable::setLeftValue()
{
    this->setupLeftValue(true);
}

void finExecVariable::clearLeftValue()
{
    this->setupLeftValue(false);
}

double finExecVariable::getNumericValue() const
{
    if ( this->_type != TP_NUMERIC )
        return 0.0;

    return this->_numVal;
}

QString finExecVariable::getStringValue() const
{
    if ( this->_type != TP_STRING )
        return QString();

    return this->_strVal;
}

QImage finExecVariable::getImageValue() const
{
    if ( this->_type != TP_IMAGE )
        return QImage();

    return this->_image;
}

void finExecVariable::setNumericValue(double val)
{
    if ( this->_type != TP_NUMERIC && this->_type != TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot assign a numeric value to this variable type.");

    if ( this->_type == TP_NULL )
        this->_type = TP_NUMERIC;

    this->_numVal = val;
}

void finExecVariable::setStringValue(const QString &strval)
{
    if ( this->_type != TP_STRING && this->_type != TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot assign a string value to this variable type.");

    if ( this->_type == TP_NULL )
        this->_type = TP_STRING;

    this->_strVal = strval;
}

void finExecVariable::setImageValue(const QImage &img)
{
    if ( this->_type != TP_IMAGE && this->_type != TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot assign an image value to this variable type.");

    if ( this->_type == TP_NULL )
        this->_type = TP_IMAGE;

    this->_image = img;
}

int finExecVariable::getArrayLength() const
{
    if ( this->_type != TP_ARRAY )
        return 0;

    return this->_itemList.count();
}

void finExecVariable::preallocArrayLength(int len)
{
    if ( this->_type != TP_ARRAY && this->_type != TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot allocate array items for this variable type.");
    if ( len < 0 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Array length cannot be negative.");

    if ( this->_type == TP_NULL )
        this->_type = TP_ARRAY;

    if ( this->_itemList.count() >= len )
        return;

    while ( this->_itemList.count() < len ) {
        finExecVariable *subvar = new finExecVariable();
        if ( subvar == nullptr )
            finThrow(finErrorKits::EC_OUT_OF_MEMORY, "Cannot allocate an array variable item.");

        subvar->_writeProtect = this->_writeProtect;
        subvar->_leftValue = this->_leftValue;
        subvar->_parentVar = this;
        this->_itemList.append(subvar);
    }
}

finExecVariable *finExecVariable::getVariableItemAt(int idx) const
{
    if ( this->_type != TP_ARRAY )
        return nullptr;

    if ( idx < 0 || idx >= this->_itemList.count() )
        return nullptr;

    return this->_itemList.at(idx);
}

finExecVariable *finExecVariable::getVariableItemAt(int idx)
{
    if ( this->_type != TP_ARRAY && this->_type != TP_NULL )
        return nullptr;

    if ( this->_type == TP_NULL )
        this->_type = TP_ARRAY;
    if ( idx < 0 )
        return nullptr;

    if ( idx < this->_itemList.count() )
        return this->_itemList.at(idx);

    this->preallocArrayLength(idx + 1);

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

void finExecVariable::clearArrayItems()
{
    if ( this->_type != TP_ARRAY && this->_type != TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot clear items from this variable type.");

    if ( this->_type == TP_NULL )
        this->_type = TP_ARRAY;

    if ( this->_itemList.count() <= 0 )
        return;

    while ( !this->_itemList.empty() ) {
        finExecVariable *itemvar = this->_itemList.first();
        this->_itemList.removeFirst();
        delete itemvar;
    }
}

bool finExecVariable::isInArray() const
{
    return (this->_parentVar != nullptr);
}

finExecVariable *finExecVariable::getParentVariable() const
{
    return this->_parentVar;
}

void finExecVariable::removeFromArray()
{
    if ( this->_parentVar == nullptr )
        return;

    for ( int i = 0; i < this->_parentVar->_itemList.count(); i++ ) {
        if ( this->_parentVar->_itemList.at(i) == this ) {
            this->_parentVar->_itemList.removeAt(i);
            break;
        }
    }
    this->_parentVar = nullptr;

    this->clearLeftValue();
}

bool finExecVariable::isNumericMatrix(int *rowcnt, int *colcnt) const
{
    if ( this->_type != TP_ARRAY )
        return false;

    int pcolcnt = 0;
    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);

        if ( curitem->_type != TP_ARRAY )
            return false;

        if ( i == 0 )
            pcolcnt = curitem->_itemList.count();
        else if ( pcolcnt != curitem->_itemList.count() )
            return false;

        for ( int j = 0; j < pcolcnt; j++) {
            finExecVariable *chditem = curitem->_itemList.at(j);
            if ( chditem->_type != TP_NUMERIC )
                return false;
        }
    }

    if ( rowcnt != nullptr )
        *rowcnt = this->_itemList.count();
    if ( colcnt != nullptr )
        *colcnt = pcolcnt;
    return true;
}

bool finExecVariable::isNumericArray(int *cnt) const
{
    if ( this->_type != TP_ARRAY )
        return false;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);
        if ( curitem->_type != TP_NUMERIC )
            return false;
    }

    if ( cnt != nullptr )
        *cnt = this->_itemList.count();
    return true;
}

bool finExecVariable::isStringArray(int *cnt) const
{
    if ( this->_type != TP_ARRAY )
        return false;

    for ( int i = 0; i < this->_itemList.count(); i++ ) {
        finExecVariable *curitem = this->_itemList.at(i);
        if ( curitem->_type != TP_STRING )
            return false;
    }

    if ( cnt != nullptr )
        *cnt = this->_itemList.count();
    return true;
}

int finExecVariable::maxArrayLevel() const
{
    if ( this->_type != TP_ARRAY )
        return 0;

    int maxlevel = 0;
    foreach ( finExecVariable *curitem, this->_itemList ) {
        int curlevel = curitem->maxArrayLevel();
        if ( curlevel > maxlevel )
            maxlevel = curlevel;
    }
    return maxlevel + 1;
}

bool finExecVariable::hasMultiLevel() const
{
    if ( this->_type != TP_ARRAY )
        return false;

    foreach ( finExecVariable *curitem, this->_itemList ) {
        if ( curitem->getType() == TP_ARRAY )
            return true;
    }
    return false;
}

const finExecVariable *finExecVariable::getLinkTarget() const
{
    if ( this->_type != finExecVariable::TP_LINK )
        return this;

    finExecVariable *target = this->_linkTarget;
    while ( target != nullptr && target->getType() == finExecVariable::TP_LINK ) {
        target = target->_linkTarget;
    }
    return target;
}

finExecVariable *finExecVariable::getLinkTarget()
{
    if ( this->_type != finExecVariable::TP_LINK )
        return this;

    finExecVariable *target = this->_linkTarget;
    while ( target != nullptr && target->getType() == finExecVariable::TP_LINK ) {
        target = target->_linkTarget;
    }
    return target;
}

finExecVariable *finExecVariable::transLinkTarget(finExecVariable *var)
{
    if ( var == nullptr )
        return nullptr;
    else
        return var->getLinkTarget();
}

void finExecVariable::setLinkTarget(finExecVariable *target)
{
    if ( this->_type == finExecVariable::TP_LINK && this->_linkTarget == target )
        return;

    this->unsetLinkTarget();

    this->_linkTarget = target;
    if ( target != nullptr )
        target->_linkedList.append(this);
}

void finExecVariable::unsetLinkTarget()
{
    if ( this->_type != finExecVariable::TP_LINK && this->_type != finExecVariable::TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot remove a link from this variable type.");

    if ( this->_type == finExecVariable::TP_NULL ) {
        this->_type = finExecVariable::TP_LINK;
        this->_linkTarget = nullptr;
        return;
    }

    if ( this->_linkTarget == nullptr )
        return;

    for ( int i = this->_linkTarget->_linkedList.count() - 1; i >= 0; i-- ) {
        if ( this->_linkTarget->_linkedList.at(i) == this )
            this->_linkTarget->_linkedList.removeAt(i);
    }
    this->_linkTarget = nullptr;
}

void finExecVariable::readBoolValue(bool *blval) const
{
    if ( blval == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Boolean output pointer is null.");

    switch ( this->getType() ) {
      case finExecVariable::TP_NULL:
        *blval = false;
        break;

      case finExecVariable::TP_NUMERIC:
        *blval = !(this->getNumericValue() < 1.0e-8 && this->getNumericValue() > -1.0e-8);
        break;

      case finExecVariable::TP_STRING:
        *blval = (QString::compare(this->getStringValue(), "yes", Qt::CaseInsensitive) == 0 ||
                  QString::compare(this->getStringValue(), "true", Qt::CaseInsensitive) == 0);
        break;

      case finExecVariable::TP_IMAGE:
        *blval = !this->getImageValue().isNull();
        break;

      case finExecVariable::TP_ARRAY:
        *blval = (this->getArrayLength() > 0);
        break;

      default:
        *blval = false;
        break;
    }
}

void finExecVariable::setupBoolValue(bool blval)
{
    if ( this->_type != finExecVariable::TP_NULL &&
         this->_type != finExecVariable::TP_NUMERIC )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot assign a boolean value to this variable type.");

    this->setType(finExecVariable::TP_NUMERIC);
    if ( blval ) {
        this->setNumericValue(1.0);
    } else {
        this->setNumericValue(0.0);
    }
}

void finExecVariable::readColorValue(QColor *color) const
{
    if ( color == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Color output pointer is null.");

    int arylen = 0;
    if ( !this->isNumericArray(&arylen) )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Color variable must be a numeric array.");
    if ( arylen != 3 && arylen != 4 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Color array must contain three or four values.");

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
}

void finExecVariable::setupColorValue(const QColor &color)
{
    if ( this->_type != finExecVariable::TP_NULL )
        finThrow(finErrorKits::EC_STATE_ERROR, "Color can only initialize a null variable.");

    this->preallocArrayLength(4);

    finExecVariable *subvar = this->getVariableItemAt(0);
    subvar->setType(finExecVariable::TP_NUMERIC);
    subvar->setNumericValue(color.redF());

    subvar = this->getVariableItemAt(1);
    subvar->setType(finExecVariable::TP_NUMERIC);
    subvar->setNumericValue(color.greenF());

    subvar = this->getVariableItemAt(2);
    subvar->setType(finExecVariable::TP_NUMERIC);
    subvar->setNumericValue(color.blueF());

    subvar = this->getVariableItemAt(3);
    subvar->setType(finExecVariable::TP_NUMERIC);
    subvar->setNumericValue(color.alphaF());

}

void finExecVariable::copyVariableValueIn(finExecVariable *srcvar)
{
    if ( srcvar == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Cannot copy from a null variable.");

    if ( this->_type != TP_NULL && this->_type != srcvar->getType() )
        finThrow(finErrorKits::EC_STATE_ERROR, "Cannot copy a value into a different variable type.");

    this->_type = srcvar->getType();

    switch ( srcvar->_type ) {
      case TP_NUMERIC:
        this->_numVal = srcvar->getNumericValue();
        break;

      case TP_STRING:
        this->_strVal = srcvar->getStringValue();
        break;

      case TP_IMAGE:
        this->_image = srcvar->getImageValue();
        break;

      case TP_ARRAY:
                this->copyArrayVariable(srcvar);
        break;

      case TP_LINK:
                this->setLinkTarget(srcvar->getLinkTarget());
        break;

      default:
                finThrow(finErrorKits::EC_READ_ERROR, "Cannot copy an unknown variable type.");
    }
}

bool finExecVariable::isSameName(const QString &name) const
{
    return (QString::compare(this->_varName, name) == 0);
}

bool finExecVariable::isSameValue(finExecVariable *var)
{
    finExecVariable *var1 = this->getLinkTarget();
    finExecVariable *var2 = var->getLinkTarget();

    if ( var1 == nullptr && var2 == nullptr )
        return true;
    else if ( var1 == nullptr || var2 == nullptr )
        return false;
    else if ( var1 == var2 )
        return true;

    if ( var1->getType() != var2->getType() )
        return false;

    switch ( var1->getType() ) {
      case finExecVariable::TP_NULL:
        return true;

      case finExecVariable::TP_NUMERIC:
        return (var1->getNumericValue() == var2->getNumericValue());
        break;

      case finExecVariable::TP_STRING:
        return (QString::compare(var1->getStringValue(), var2->getStringValue()) == 0);
        break;

      case finExecVariable::TP_IMAGE:
        return false;
        break;

      case finExecVariable::TP_ARRAY:
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

void finExecVariable::copyArrayVariable(const finExecVariable *srcvar)
{
    this->clearArrayItems();

    this->preallocArrayLength(srcvar->_itemList.count());
    for ( int i = 0; i < srcvar->_itemList.count(); i++ ) {
        finExecVariable *subsrcvar = srcvar->_itemList.at(i);
        finExecVariable *subdstvar = this->getVariableItemAt(i);

        subdstvar->copyVariableValueIn(subsrcvar);
    }
}

void finExecVariable::copyVariableValue(finExecVariable *srcvar)
{
    this->disposeValue();

    if ( srcvar == nullptr )
        return;

    this->copyVariableValueIn(srcvar);

    // To keep the access mode of variables in array.
    this->setupWriteProtected(this->_writeProtect);
    this->setupLeftValue(this->_leftValue);

}

void finExecVariable::smartCopyVariableValue(finExecVariable *srcvar)
{
    this->disposeValue();

    if ( srcvar == nullptr )
        return;

    if ( srcvar->getType() == finExecVariable::TP_ARRAY &&
         !srcvar->isLeftValue() ) {
        this->_itemList = srcvar->_itemList;
        srcvar->_itemList.clear();

        finExecVariable *itemvar;
        foreach ( itemvar, this->_itemList ) {
            itemvar->_parentVar = this;
        }
        this->_type = finExecVariable::TP_ARRAY;
    } else {
        this->copyVariableValueIn(srcvar);
    }

    // To keep the access mode of variables in array.
    this->setupWriteProtected(this->_writeProtect);
    this->setupLeftValue(this->_leftValue);

}

void finExecVariable::copyVariable(finExecVariable *srcvar)
{
    this->copyVariableValueIn(srcvar);

    this->setName(srcvar->getName());
    this->setupWriteProtected(srcvar->isWriteProtected());
    this->setupLeftValue(srcvar->isLeftValue());

}

void finExecVariable::disposeValue()
{
    if ( this->_type == TP_NULL )
        return;

    switch ( this->_type ) {
      case finExecVariable::TP_NUMERIC:
        this->_numVal = 0.0;
        break;

      case finExecVariable::TP_STRING:
        this->_strVal.clear();
        break;

      case finExecVariable::TP_IMAGE:
        this->_image = QImage();
        break;

      case finExecVariable::TP_ARRAY:
        this->clearArrayItems();
        break;

      case finExecVariable::TP_LINK:
        this->unsetLinkTarget();
        break;

      default:
        break;
    }

    this->_type = TP_NULL;
}

void finExecVariable::dispose()
{
    this->disposeValue();

    this->_varName.clear();
    this->clearWriteProtected();
    this->clearLeftValue();
}

void finExecVariable::clearLinkedVariables()
{
    while ( !this->_linkedList.empty() ) {
        int cnt = this->_linkedList.count();
        finExecVariable *lvar = this->_linkedList.first();
        lvar->unsetLinkTarget();

        if ( this->_linkedList.count() >= cnt )
            this->_linkedList.removeFirst();
    }
}

void finExecVariable::transToPointListArray(finExecVariable *aryvar, QList<QPointF> *ptlist)
{
    if ( ptlist == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Point list output pointer is null.");

    if ( aryvar == nullptr || aryvar->getType() == finExecVariable::TP_NULL ) {
        ptlist->clear();
        return;
    }

    int arylen = 0;
    if ( !aryvar->isNumericArray(&arylen) )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Point list source must be a numeric array.");

    ptlist->clear();
    for ( int i = 0; i + 1 < arylen; i += 2 ) {
        finExecVariable *varx = aryvar->getVariableItemAt(i);
        finExecVariable *vary = aryvar->getVariableItemAt(i + 1);

        ptlist->append(QPoint(varx->getNumericValue(), vary->getNumericValue()));
    }
}

void finExecVariable::transToPointListMatrix(finExecVariable *matvar, QList<QPointF> *ptlist)
{
    if ( ptlist == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Point list output pointer is null.");

    if ( matvar == nullptr || matvar->getType() == finExecVariable::TP_NULL ) {
        ptlist->clear();
        return;
    }

    int row = 0, col = 0;
    if ( !matvar->isNumericMatrix(&row, &col) )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Point matrix source must be numeric.");
    if ( col < 2 )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Point matrix must contain at least two columns.");

    ptlist->clear();
    for ( int i = 0; i < row; i++ ) {
        finExecVariable *ptvar = matvar->getVariableItemAt(i);
        finExecVariable *varx = ptvar->getVariableItemAt(0);
        finExecVariable *vary = ptvar->getVariableItemAt(1);

        ptlist->append(QPointF(varx->getNumericValue(), vary->getNumericValue()));
    }
}

void finExecVariable::transToPointList(finExecVariable *var, QList<QPointF> *ptlist)
{
    if ( ptlist == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Point list output pointer is null.");

    if ( var == nullptr || var->getType() == finExecVariable::TP_NULL ) {
        ptlist->clear();
        return;
    }

    if ( var->getType() != finExecVariable::TP_ARRAY )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Point list source must be an array.");

    if ( var->getArrayLength() > 0 && var->getVariableItemAt(0)->getType() == finExecVariable::TP_ARRAY )
        transToPointListMatrix(var, ptlist);
    else
        transToPointListArray(var, ptlist);
}

void finExecVariable::transToPointList(
        finExecVariable *xvar, finExecVariable *yvar, QList<QPointF> *ptlist)
{
    if ( ptlist == nullptr )
        finThrow(finErrorKits::EC_NULL_POINTER, "Point list output pointer is null.");

    if ( yvar == nullptr || yvar->getType() == finExecVariable::TP_NULL ) {
        if ( xvar == nullptr || xvar->getType() == finExecVariable::TP_NULL ) {
            ptlist->clear();
              return;
        } else {
               transToPointList(xvar, ptlist);
               return;
        }
    }

    int arylen = 0, yarylen = 0;
    if ( !xvar->isNumericArray(&arylen) || !yvar->isNumericArray(&yarylen) )
        finThrow(finErrorKits::EC_INVALID_PARAM, "Point coordinate sources must be numeric arrays.");
    if ( arylen > yarylen )
        arylen = yarylen;

    ptlist->clear();
    for ( int i = 0; i < arylen; i++ ) {
        finExecVariable *xitem = xvar->getVariableItemAt(i);
        finExecVariable *yitem = yvar->getVariableItemAt(i);

        ptlist->append(QPointF(xitem->getNumericValue(), yitem->getNumericValue()));
    }
}

finExecVariable *finExecVariable::buildNonLeftVariable(finExecVariable *var)
{
    if ( var == nullptr )
        return nullptr;

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != nullptr )
        var = realvar;

    if ( !var->isLeftValue() && !var->isInArray() )
        return var;

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return nullptr;

    retvar->copyVariableValueIn(var);

    retvar->setWriteProtected();
    retvar->clearLeftValue();
    return retvar;
}

finExecVariable *finExecVariable::buildCopyLeftVariable(finExecVariable *var)
{
    if ( var == nullptr ) {
        var = new finExecVariable();
        if ( var == nullptr )
            return nullptr;
        var->clearLeftValue();
    }

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != nullptr )
        var = realvar;

    if ( !var->isLeftValue() ) {
        if ( var->isInArray() )
            var->removeFromArray();
        var->setLeftValue();
        var->clearWriteProtected();
        return var;
    }

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return nullptr;

    retvar->copyVariableValueIn(var);

    retvar->setLeftValue();
    retvar->clearWriteProtected();
    return retvar;
}

finExecVariable *finExecVariable::buildLinkLeftVariable(finExecVariable *var)
{
    if ( var == nullptr ) {
        var = new finExecVariable();
        if ( var == nullptr )
            return nullptr;
        var->clearLeftValue();
    }

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != nullptr )
        var = realvar;

    if ( !var->isLeftValue() )
        return buildCopyLeftVariable(var);

    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return nullptr;

    retvar->setLinkTarget(var);

    retvar->setLeftValue();
    retvar->clearWriteProtected();
    return retvar;
}

finExecVariable *finExecVariable::buildFuncReturnVariable(finExecVariable *var, finExecEnvironment *env)
{
    if ( var == nullptr )
        return nullptr;

    finExecVariable *realvar = var->getLinkTarget();
    if ( realvar != nullptr )
        var = realvar;

    if ( !var->isLeftValue() ) {
        var->removeFromArray();
        return var;
    }

    if ( !env->isVariableInEnv(var) )
        return var;

    var->removeFromArray();
    finErrorCode errcode = env->removeVariable(var);
    if ( finErrorKits::isErrorResult(errcode) )
        goto copy_var;

    var->clearLeftValue();
    return var;

copy_var:
    finExecVariable *clonevar = new finExecVariable();
    if ( clonevar == nullptr )
        return nullptr;

    clonevar->copyVariableValue(var);
    clonevar->setWriteProtected();
    clonevar->clearLeftValue();
    return clonevar;
}

void finExecVariable::releaseNonLeftVariable(finExecVariable *var)
{
    if ( var == nullptr )
        return;

    if ( !var->isLeftValue() )
        delete var;
}

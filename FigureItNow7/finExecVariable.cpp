#include "finExecVariable.h"

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

finExecVariable *finExecVariable::getVariableItemAt(int idx) const
{
    if ( this->_type != FIN_VR_TYPE_ARRAY )
        return NULL;

    if ( idx < 0 )
        return NULL;
    if ( idx >= this->_itemList.count() )
        return NULL;

    return this->_itemList.at(idx);
}


finExecVariable *finExecVariable::getVariableItemAt(int idx)
{
    if ( this->_type != FIN_VR_TYPE_ARRAY || this->_type != FIN_VR_TYPE_NULL )
        return NULL;

    this->_type = FIN_VR_TYPE_ARRAY;

    if ( idx < 0 )
        return NULL;

    if ( idx >= this->_itemList.count() ) {
        if ( this->_writeProtect )
            return NULL;

        while ( idx >= this->_itemList.count() ) {
            finExecVariable *subvar = new finExecVariable();
            subvar->_writeProtect = this->_writeProtect;
            subvar->_leftValue = this->_leftValue;
        }
    }

    return this->_itemList.at(idx);
}

int finExecVariable::getArrayLength() const
{
    return this->_itemList.count();
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
    this->_type = type;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setWriteProtected()
{
    if ( this->_writeProtect == true )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_writeProtect = true;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::clearWriteProtected()
{
    if ( this->_writeProtect == false )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_writeProtect = false;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setLeftValue()
{
    if ( this->_leftValue == true )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_leftValue = true;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::clearLeftValue()
{
    if ( this->_leftValue == false )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_leftValue = false;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setNumericValue(double val)
{
    if ( this->_type != FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_writeProtect )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_numVal = val;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::setStringValue(const QString &strval)
{
    if ( this->_type != FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    if ( this->_writeProtect )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_strVal = strval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::copyVariable(const finExecVariable *srcvar)
{
    if ( srcvar == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    if ( this->_writeProtect )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    bool leftval = this->_leftValue;
    finErrorCode errcode = finErrorCodeKits::FIN_EC_SUCCESS;

    if ( this->_type == FIN_VR_TYPE_ARRAY && srcvar->_type != FIN_VR_TYPE_ARRAY )
        this->dispose();

    this->_type = srcvar->_type;
    this->_leftValue = leftval;

    switch ( this->_type ) {
      case FIN_VR_TYPE_NUMERIC:
        this->_numVal = srcvar->_numVal;
        break;

      case FIN_VR_TYPE_STRING:
        this->_strVal = srcvar->_strVal;
        break;

      case FIN_VR_TYPE_ARRAY:
        errcode = copyArrayVariable(srcvar);
        break;

      default:
        break;
    }

    return errcode;
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

finErrorCode finExecVariable::appendList(const QList<finExecVariable *> &vallist)
{
    if ( this->_type != FIN_VR_TYPE_ARRAY || this->_type != FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    this->_type = FIN_VR_TYPE_ARRAY;
    this->_itemList.append(vallist);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finExecVariable::dispose()
{
    if ( this->_type == FIN_VR_TYPE_NULL )
        return finErrorCodeKits::FIN_EC_DUPLICATE_OP;

    this->_type = FIN_VR_TYPE_NULL;
    this->_strVal.clear();

    while ( this->_itemList.count() > 0 ) {
        finExecVariable *subvar = this->_itemList.first();
        this->_itemList.removeFirst();
        subvar->dispose();
        delete subvar;
    }

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

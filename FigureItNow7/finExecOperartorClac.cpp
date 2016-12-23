#include "finExecOperartorClac.h"

finExecOperartorClac::finExecOperartorClac()
{

}

struct finExecOperartorClacDatabase {
    finLexOperatorType _optype;
    int _oprandCnt;
    finErrorCode (*_opcall)(QList<finExecVariable *> *oprands, finExecVariable **retval);
};

static finErrorCode _addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);

static struct finExecOperartorClacDatabase _glOperatorClacDb[] = {
    { finLexNode::FIN_LN_OPTYPE_ADD, 2, _addOpCall },
};
static int _glOperatorCalcDbCnt = sizeof (_glOperatorClacDb) / sizeof (struct finExecOperartorClacDatabase);

finErrorCode finExecOperartorClac::execOpCalc(
        finLexOperatorType optype, QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands == NULL || retval == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    struct finExecOperartorClacDatabase *curitem = NULL;
    bool found = false;
    for ( int i = 0; i < _glOperatorCalcDbCnt; i++ ) {
        curitem = &_glOperatorClacDb[i];
        if ( curitem->_optype == optype ) {
            found = true;
            break;
        }
    }
    if ( !found )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( oprands->count() < curitem->_oprandCnt )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    return curitem->_opcall(oprands, retval);
}

static finErrorCode _addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);
    *retval = new finExecVariable();

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        switch ( oprand2->getType() ) {
          case finExecVariable::FIN_VR_TYPE_NUMERIC:
            (*retval)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            (*retval)->setNumericValue(oprand1->getNumericValue() +
                                       oprand2->getNumericValue());
            break;

          default:
            goto bad;
        }
    } else if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING ) {
        switch ( oprand2->getType() ) {
          case finExecVariable::FIN_VR_TYPE_STRING:
            (*retval)->setType(finExecVariable::FIN_VR_TYPE_STRING);
            (*retval)->setStringValue(oprand1->getStringValue().append(oprand2->getStringValue()));
            break;

          case finExecVariable::FIN_VR_TYPE_NUMERIC:
            (*retval)->setType(finExecVariable::FIN_VR_TYPE_STRING);
            (*retval)->setStringValue(oprand1->getStringValue().append(
                                          QString::number(oprand2->getNumericValue())));
            break;

          default:
            goto bad;
        }
    } else {
        goto bad;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;

bad:
    delete *retval;
    *retval = NULL;
    return finErrorCodeKits::FIN_EC_READ_ERROR;
}


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

#include "finExecOperartorClac.h"

finExecOperartorClac::finExecOperartorClac()
{
    return;
}

struct finExecOperartorClacDatabase {
    finLexOperatorType _optype;
    int _oprandCnt;
    finErrorCode (*_opcall)(QList<finExecVariable *> *oprands, finExecVariable **retval);
};

static finErrorCode
_addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode
_subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode
_mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode
_divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);

static struct finExecOperartorClacDatabase _glOperatorClacDb[] = {
    { finLexNode::FIN_LN_OPTYPE_ADD, 2, _addOpCall },
    { finLexNode::FIN_LN_OPTYPE_SUB, 2, _subOpCall },
    { finLexNode::FIN_LN_OPTYPE_MUL, 2, _mulOpCall },
    { finLexNode::FIN_LN_OPTYPE_DIV, 2, _divOpCall },
};
static int _glOperatorCalcDbCnt = sizeof (_glOperatorClacDb) / sizeof (struct finExecOperartorClacDatabase);

finErrorCode
finExecOperartorClac::execOpCalc(
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

static finErrorCode
_addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);
    finExecVariable *tmpretval = new finExecVariable();

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->clearLeftValue();
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() +
                                   oprand2->getNumericValue());
    } else if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_STRING ) {
        tmpretval->clearLeftValue();
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_STRING);
        tmpretval->setStringValue(oprand1->getStringValue().append(oprand2->getStringValue()));
    } else if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->clearLeftValue();
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_STRING);
        tmpretval->setStringValue(oprand1->getStringValue().append(
                                          QString::number(oprand2->getNumericValue())));
    } else {
        delete tmpretval;
        *retval = NULL;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);
    *retval = new finExecVariable();

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        switch ( oprand2->getType() ) {
          case finExecVariable::FIN_VR_TYPE_NUMERIC:
            (*retval)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            (*retval)->setNumericValue(oprand1->getNumericValue() -
                                       oprand2->getNumericValue());
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

static finErrorCode
_mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);
    *retval = new finExecVariable();

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        switch ( oprand2->getType() ) {
          case finExecVariable::FIN_VR_TYPE_NUMERIC:
            (*retval)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            (*retval)->setNumericValue(oprand1->getNumericValue() *
                                       oprand2->getNumericValue());
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

static finErrorCode
_divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);
    *retval = new finExecVariable();
    finErrorCode errcode = finErrorCodeKits::FIN_EC_READ_ERROR;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        switch ( oprand2->getType() ) {
          case finExecVariable::FIN_VR_TYPE_NUMERIC:
            if ( oprand2->getNumericValue() == 0.0 ) {
                errcode = finErrorCodeKits::FIN_EC_OVERFLOW;
                goto bad;
            }

            (*retval)->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            (*retval)->setNumericValue(oprand1->getNumericValue() /
                                       oprand2->getNumericValue());
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
    return errcode;
}

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

static finErrorCode _addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _letOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);

static struct finExecOperartorClacDatabase _glOperatorClacDb[] = {
    { finLexNode::FIN_LN_OPTYPE_ADD,       2, _addOpCall },
    { finLexNode::FIN_LN_OPTYPE_SUB,       2, _subOpCall },
    { finLexNode::FIN_LN_OPTYPE_POSITIVE,  1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_NEGATIVE,  1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT,   1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT_2, 1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_MUL,       2, _mulOpCall },
    { finLexNode::FIN_LN_OPTYPE_DIV,       2, _divOpCall },
    { finLexNode::FIN_LN_OPTYPE_MOD,       2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_POWER,     2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LET,       2, _letOpCall },
    { finLexNode::FIN_LN_OPTYPE_EQUAL,     2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_GRT,       2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LES,       2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_NONEQUAL,  2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_GRT_EQ,    2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LES_EQ,    2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_NOT, 1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_AND, 2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_OR,  2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_XOR, 2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_NOT,   1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_AND,   2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_OR,    2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_XOR,   2, NULL       },
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
    if ( !found || curitem->_opcall == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( oprands->count() < curitem->_oprandCnt )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    return curitem->_opcall(oprands, retval);
}

static finErrorCode
_addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() +
                                   oprand2->getNumericValue());
    } else if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_STRING ) {
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_STRING);
        tmpretval->setStringValue(oprand1->getStringValue().append(oprand2->getStringValue()));
    } else if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_STRING);
        tmpretval->setStringValue(
                    oprand1->getStringValue().append(QString::number(oprand2->getNumericValue())));
    } else {
        delete tmpretval;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
            tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            tmpretval->setNumericValue(oprand1->getNumericValue() -
                                       oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
            tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            tmpretval->setNumericValue(oprand1->getNumericValue() *
                                       oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
            tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
            tmpretval->setNumericValue(oprand1->getNumericValue() /
                                       oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _letOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
}

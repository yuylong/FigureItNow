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
 * 20170108    1  Yulong Yu    Add logic value manipulation.
 */

#include "finExecOperartorClac.h"

finExecOperartorClac::finExecOperartorClac()
{
    return;
}

bool finExecOperartorClac::varLogicValue(finExecVariable *var)
{
    if ( var == NULL )
        return false;

    switch ( var->getType() ) {
      case finExecVariable::FIN_VR_TYPE_NULL:
        return false;
        break;

      case finExecVariable::FIN_VR_TYPE_NUMERIC:
        if ( var->getNumericValue() < 1.0e-6 && var->getNumericValue() > -1.0e-6)
            return false;
        else
            return true;
        break;

      case finExecVariable::FIN_VR_TYPE_STRING:
        if ( QString::compare(var->getStringValue(), "yes", Qt::CaseInsensitive) == 0 ||
             QString::compare(var->getStringValue(), "true", Qt::CaseInsensitive) == 0 )
            return true;
        else
            return false;
        break;

      case finExecVariable::FIN_VR_TYPE_ARRAY:
        if ( var->getArrayLength() <= 0 )
            return false;
        else
            return true;
        break;

      default:
        return false;
        break;
    }
    return false;
}

finExecVariable *finExecOperartorClac::buildStdLogicVar(bool blval)
{
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == NULL )
        return NULL;

    retvar->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    if ( blval ) {
        retvar->setNumericValue(1.0);
    } else {
        retvar->setNumericValue(0.0);
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();
    return retvar;
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
static finErrorCode _logicNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);

static struct finExecOperartorClacDatabase _glOperatorCalcDb[] = {
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
    { finLexNode::FIN_LN_OPTYPE_LOGIC_NOT, 1, _logicNotOpCall },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_AND, 2, _logicAndOpCall },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_OR,  2, _logicOrOpCall  },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_XOR, 2, _logicXorOpCall },
    { finLexNode::FIN_LN_OPTYPE_BIT_NOT,   1, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_AND,   2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_OR,    2, NULL       },
    { finLexNode::FIN_LN_OPTYPE_BIT_XOR,   2, NULL       },
};
static const int _glOperatorCalcDbCnt =
        sizeof (_glOperatorCalcDb) / sizeof (struct finExecOperartorClacDatabase);

finErrorCode
finExecOperartorClac::execOpCalc(
        finLexOperatorType optype, QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands == NULL || retval == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    struct finExecOperartorClacDatabase *curitem = NULL;
    bool found = false;
    for ( int i = 0; i < _glOperatorCalcDbCnt; i++ ) {
        curitem = &_glOperatorCalcDb[i];
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
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( !oprand1->isLeftValue() || oprand1->isWriteProtected() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NULL &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        /* Nothing to do */;
    } else if ( (oprand1->getType() == finExecVariable::FIN_VR_TYPE_NULL ||
                 oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC) &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        oprand1->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        oprand1->setNumericValue(oprand2->getNumericValue());
    } else if ( (oprand1->getType() == finExecVariable::FIN_VR_TYPE_NULL ||
                 oprand1->getType() == finExecVariable::FIN_VR_TYPE_STRING) &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_STRING ) {
        oprand1->setType(finExecVariable::FIN_VR_TYPE_STRING);
        oprand1->setStringValue(oprand2->getStringValue());
    } else if ( (oprand1->getType() == finExecVariable::FIN_VR_TYPE_NULL ||
                 oprand1->getType() == finExecVariable::FIN_VR_TYPE_ARRAY) &&
                oprand2->getType() == finExecVariable::FIN_VR_TYPE_ARRAY ) {
        return finErrorCodeKits::FIN_EC_NON_IMPLEMENT;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = oprand1;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _logicNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();
    if ( oprand == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    bool blval = finExecOperartorClac::varLogicValue(oprand);
    *retval = finExecOperartorClac::buildStdLogicVar(!blval);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _logicAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    bool blval1 = finExecOperartorClac::varLogicValue(oprand1);
    bool blval2 = finExecOperartorClac::varLogicValue(oprand2);
    *retval = finExecOperartorClac::buildStdLogicVar(blval1 && blval2);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _logicOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    bool blval1 = finExecOperartorClac::varLogicValue(oprand1);
    bool blval2 = finExecOperartorClac::varLogicValue(oprand2);
    *retval = finExecOperartorClac::buildStdLogicVar(blval1 || blval2);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _logicXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    bool blval1 = finExecOperartorClac::varLogicValue(oprand1);
    bool blval2 = finExecOperartorClac::varLogicValue(oprand2);
    *retval = finExecOperartorClac::buildStdLogicVar(blval1 == blval2 ? false : true);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

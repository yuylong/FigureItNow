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
 * 20161226    0  Yulong Yu    Create this file.
 * 20170108    1  Yulong Yu    Add logic value manipulation.
 * 20170209    2  Yulong Yu    Implement the operators for logic manipulations.
 */

#include "finExecOperartorClac.h"

#include <QtMath>


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

static finErrorCode _brcktOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _pstvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _ngtvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _psAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _prAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _psDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _prDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _modOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _powOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _factoriOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _letOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _eqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _grtOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _lesOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _neqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _gteqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _lseqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _logicXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _accessOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _commaOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _bitNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _bitAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _bitOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);
static finErrorCode _bitXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval);

static struct finExecOperartorClacDatabase _glOperatorCalcDb[] = {
    { finLexNode::FIN_LN_OPTYPE_L_RND_BRCKT, 0, _brcktOpCall    },
    { finLexNode::FIN_LN_OPTYPE_ADD,         2, _addOpCall      },
    { finLexNode::FIN_LN_OPTYPE_SUB,         2, _subOpCall      },
    { finLexNode::FIN_LN_OPTYPE_MUL,         2, _mulOpCall      },
    { finLexNode::FIN_LN_OPTYPE_DIV,         2, _divOpCall      },
    { finLexNode::FIN_LN_OPTYPE_POSITIVE,    1, _pstvOpCall     },
    { finLexNode::FIN_LN_OPTYPE_NEGATIVE,    1, _ngtvOpCall     },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT,     1, _psAccumOpCall  },
    { finLexNode::FIN_LN_OPTYPE_ACCUMLT_2,   1, _prAccumOpCall  },
    { finLexNode::FIN_LN_OPTYPE_DESCEND,     1, _psDescdOpCall  },
    { finLexNode::FIN_LN_OPTYPE_DESCEND_2,   1, _psDescdOpCall  },
    { finLexNode::FIN_LN_OPTYPE_MOD,         2, _modOpCall      },
    { finLexNode::FIN_LN_OPTYPE_POWER,       2, _powOpCall      },
    { finLexNode::FIN_LN_OPTYPE_FACTORI,     1, _factoriOpCall  },
    { finLexNode::FIN_LN_OPTYPE_LET,         2, _letOpCall      },
    { finLexNode::FIN_LN_OPTYPE_EQUAL,       2, _eqOpCall       },
    { finLexNode::FIN_LN_OPTYPE_GRT,         2, _grtOpCall      },
    { finLexNode::FIN_LN_OPTYPE_LES,         2, _lesOpCall      },
    { finLexNode::FIN_LN_OPTYPE_NONEQUAL,    2, _neqOpCall      },
    { finLexNode::FIN_LN_OPTYPE_GRT_EQ,      2, _gteqOpCall     },
    { finLexNode::FIN_LN_OPTYPE_LES_EQ,      2, _lseqOpCall     },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_NOT,   1, _logicNotOpCall },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_AND,   2, _logicAndOpCall },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_OR,    2, _logicOrOpCall  },
    { finLexNode::FIN_LN_OPTYPE_LOGIC_XOR,   2, _logicXorOpCall },
    { finLexNode::FIN_LN_OPTYPE_ACCESS,      2, _accessOpCall   },
    { finLexNode::FIN_LN_OPTYPE_L_SQR_BRCKT, 0, _brcktOpCall    },
    { finLexNode::FIN_LN_OPTYPE_COMMA,       0, _commaOpCall    },
    { finLexNode::FIN_LN_OPTYPE_BIT_NOT,     1, _bitNotOpCall   },
    { finLexNode::FIN_LN_OPTYPE_BIT_AND,     2, _bitAndOpCall   },
    { finLexNode::FIN_LN_OPTYPE_BIT_OR,      2, _bitOrOpCall    },
    { finLexNode::FIN_LN_OPTYPE_BIT_XOR,     2, _bitXorOpCall   },
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
_brcktOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands->empty() ) {
        *retval = new finExecVariable;
        (*retval)->setType(finExecVariable::FIN_VR_TYPE_NULL);
        (*retval)->setWriteProtected();
        (*retval)->clearLeftValue();
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    }

    *retval = oprands->first();
    // Remove it from oprand to prevent dealloc.
    oprands->removeFirst();
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

static finErrorCode _pstvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();
    if ( oprand == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        tmpretval->setNumericValue(oprand->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _ngtvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();
    if ( oprand == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        tmpretval->setNumericValue(0.0 - oprand->getNumericValue());
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
_psAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        oprand->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        oprand->setNumericValue(0.0);
    } else if ( oprand->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *resvar = new finExecVariable();
    resvar->copyVariableValue(oprand);
    resvar->clearLeftValue();
    resvar->setLeftValue();

    oprand->setNumericValue(oprand->getNumericValue() + 1.0);
    *retval = resvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_prAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        oprand->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        oprand->setNumericValue(1.0);
    } else if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        oprand->setNumericValue(oprand->getNumericValue() + 1.0);
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = oprand;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_psDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        oprand->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        oprand->setNumericValue(0.0);
    } else if ( oprand->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *resvar = new finExecVariable();
    resvar->copyVariableValue(oprand);
    resvar->clearLeftValue();
    resvar->setLeftValue();

    oprand->setNumericValue(oprand->getNumericValue() - 1.0);
    *retval = resvar;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_prDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NULL ) {
        oprand->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        oprand->setNumericValue(-1.0);
    } else if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        oprand->setNumericValue(oprand->getNumericValue() - 1.0);
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = oprand;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_modOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double opnum1, opnum2;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        opnum1 = oprand1->getNumericValue();
        opnum2 = oprand2->getNumericValue();
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    double divint = floor(opnum1 / opnum2);
    double modres = opnum1 - (divint * opnum2);

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue(modres);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_powOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    double opnum1, opnum2;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        opnum1 = oprand1->getNumericValue();
        opnum2 = oprand2->getNumericValue();
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue(pow(opnum1, opnum2));
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_factoriOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();
    if ( oprand == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    int opnum;
    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        opnum = (int)(floor(oprand->getNumericValue()));
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    double resnum = 1.0;
    for ( int i = 2; i <= opnum; i++ )
        resnum *= (double)i;

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue(resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_letOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
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

finErrorCode
_eqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);

    bool opssame = oprand1->isSameValue(oprand2);
    *retval = finExecOperartorClac::buildStdLogicVar(opssame);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_grtOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    bool blval;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        blval = (oprand1->getNumericValue() > oprand2->getNumericValue());
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = finExecOperartorClac::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_lesOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    bool blval;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        blval = (oprand1->getNumericValue() < oprand2->getNumericValue());
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = finExecOperartorClac::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_neqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);

    bool opssame = oprand1->isSameValue(oprand2);
    *retval = finExecOperartorClac::buildStdLogicVar(!opssame);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_gteqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    bool blval;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        blval = (oprand1->getNumericValue() >= oprand2->getNumericValue());
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = finExecOperartorClac::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_lseqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    bool blval;

    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        blval = (oprand1->getNumericValue() <= oprand2->getNumericValue());
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = finExecOperartorClac::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_logicNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
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

static finErrorCode
_logicAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
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

static finErrorCode
_logicOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
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

static finErrorCode
_logicXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
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

static finErrorCode _accessOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *parent = oprands->at(0)->getLinkTarget();
    finExecVariable *aryidx = oprands->at(1)->getLinkTarget();
    finExecVariable *child = NULL;

    if ( (parent->getType() == finExecVariable::FIN_VR_TYPE_NULL ||
          parent->getType() == finExecVariable::FIN_VR_TYPE_ARRAY) &&
         aryidx->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        double dbidx = aryidx->getNumericValue();

        child = parent->getVariableItemAt((int)dbidx);
        if ( child == NULL )
            return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    *retval = child;
    if ( !child->isLeftValue() )
        child->removeFromArray();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_commaOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands->empty() ) {
        *retval = new finExecVariable;
        (*retval)->setType(finExecVariable::FIN_VR_TYPE_NULL);
        (*retval)->setWriteProtected();
        (*retval)->clearLeftValue();
        return finErrorCodeKits::FIN_EC_NORMAL_WARN;
    }

    *retval = oprands->last();
    // Remove it from oprand to prevent dealloc.
    oprands->removeLast();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static const double _max_bitable_int = (double)(0xFFFFFFFF);
static const double _min_bitable_int = -(double)(0x80000000);

static inline double _bitable_neg_to_post(double num)
{
    if ( num >= 0 )
        return num;
    else if ( num > -1 )
        return 0.0;
    else
        return (double)(0xFFFFFFFF) + 1.0 + num;
}

static finErrorCode
_bitNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = oprands->at(0)->getLinkTarget();
    if ( oprand == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    quint32 opnum;
    if ( oprand->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        double opdbnum = oprand->getNumericValue();
        if ( opdbnum > _max_bitable_int || opdbnum < _min_bitable_int )
            return finErrorCodeKits::FIN_EC_OVERFLOW;

        opdbnum = _bitable_neg_to_post(opdbnum);
        opnum = (quint32)opdbnum;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    opnum = ~opnum;

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue((double)opnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_bitAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorCodeKits::FIN_EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 & opnum2);

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_bitOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorCodeKits::FIN_EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 | opnum2);

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_bitXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0)->getLinkTarget();
    finExecVariable *oprand2 = oprands->at(1)->getLinkTarget();
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC &&
         oprand2->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorCodeKits::FIN_EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 ^ opnum2);

    tmpretval->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

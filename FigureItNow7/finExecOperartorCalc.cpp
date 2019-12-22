/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Dec 26th, 2016
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecOperartorCalc.h"

#include <QtMath>


finExecOperartorCalc::finExecOperartorCalc()
{
    return;
}

bool finExecOperartorCalc::varLogicValue(finExecVariable *var)
{
    if ( var == nullptr )
        return false;

    bool retval = false;
    finErrorCode errcode = var->readBoolValue(&retval);
    if ( finErrorKits::isErrorResult(errcode) )
        return false;

    return retval;
}

finExecVariable *finExecOperartorCalc::buildStdLogicVar(bool blval)
{
    finExecVariable *retvar = new finExecVariable();
    if ( retvar == nullptr )
        return nullptr;

    finErrorCode errcode = retvar->setupBoolValue(blval);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete retvar;
        return nullptr;
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();
    return retvar;
}

struct finExecOperartorCalcDatabase {
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

static struct finExecOperartorCalcDatabase _glOperatorCalcDb[] = {
    { finLexNode::OP_L_RND_BRCKT, 0, _brcktOpCall    },
    { finLexNode::OP_ADD,         2, _addOpCall      },
    { finLexNode::OP_SUB,         2, _subOpCall      },
    { finLexNode::OP_MUL,         2, _mulOpCall      },
    { finLexNode::OP_DIV,         2, _divOpCall      },
    { finLexNode::OP_POSITIVE,    1, _pstvOpCall     },
    { finLexNode::OP_NEGATIVE,    1, _ngtvOpCall     },
    { finLexNode::OP_ACCUMLT,     1, _psAccumOpCall  },
    { finLexNode::OP_ACCUMLT_2,   1, _prAccumOpCall  },
    { finLexNode::OP_DESCEND,     1, _psDescdOpCall  },
    { finLexNode::OP_DESCEND_2,   1, _prDescdOpCall  },
    { finLexNode::OP_MOD,         2, _modOpCall      },
    { finLexNode::OP_POWER,       2, _powOpCall      },
    { finLexNode::OP_FACTORI,     1, _factoriOpCall  },
    { finLexNode::OP_LET,         2, _letOpCall      },
    { finLexNode::OP_EQUAL,       2, _eqOpCall       },
    { finLexNode::OP_GRT,         2, _grtOpCall      },
    { finLexNode::OP_LES,         2, _lesOpCall      },
    { finLexNode::OP_NONEQUAL,    2, _neqOpCall      },
    { finLexNode::OP_GRT_EQ,      2, _gteqOpCall     },
    { finLexNode::OP_LES_EQ,      2, _lseqOpCall     },
    { finLexNode::OP_LOGIC_NOT,   1, _logicNotOpCall },
    { finLexNode::OP_LOGIC_AND,   2, _logicAndOpCall },
    { finLexNode::OP_LOGIC_OR,    2, _logicOrOpCall  },
    { finLexNode::OP_LOGIC_XOR,   2, _logicXorOpCall },
    { finLexNode::OP_ACCESS,      2, _accessOpCall   },
    { finLexNode::OP_L_SQR_BRCKT, 0, _brcktOpCall    },
    { finLexNode::OP_COMMA,       0, _commaOpCall    },
    { finLexNode::OP_BIT_NOT,     1, _bitNotOpCall   },
    { finLexNode::OP_BIT_AND,     2, _bitAndOpCall   },
    { finLexNode::OP_BIT_OR,      2, _bitOrOpCall    },
    { finLexNode::OP_BIT_XOR,     2, _bitXorOpCall   },
};
static const int _glOperatorCalcDbCnt =
        sizeof (_glOperatorCalcDb) / sizeof (struct finExecOperartorCalcDatabase);

finErrorCode
finExecOperartorCalc::execOpCalc(
        finLexOperatorType optype, QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands == nullptr || retval == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    struct finExecOperartorCalcDatabase *curitem = nullptr;
    bool found = false;
    for ( int i = 0; i < _glOperatorCalcDbCnt; i++ ) {
        curitem = &_glOperatorCalcDb[i];
        if ( curitem->_optype == optype ) {
            found = true;
            break;
        }
    }
    if ( !found || curitem->_opcall == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( oprands->count() < curitem->_oprandCnt )
        return finErrorKits::EC_INVALID_PARAM;

    return curitem->_opcall(oprands, retval);
}

static finErrorCode
_brcktOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands->empty() ) {
        *retval = nullptr;
        return finErrorKits::EC_NORMAL_WARN;
    }

    *retval = oprands->first();
    // Remove it from oprand to prevent dealloc.
    oprands->removeFirst();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_addOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == nullptr || oprand2 == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() +
                                   oprand2->getNumericValue());
    } else if ( oprand1->getType() == finExecVariable::TP_STRING &&
                oprand2->getType() == finExecVariable::TP_STRING ) {
        tmpretval->setType(finExecVariable::TP_STRING);
        QString retstr = oprand1->getStringValue();
        retstr.append(oprand2->getStringValue());
        tmpretval->setStringValue(retstr);
    } else if ( oprand1->getType() == finExecVariable::TP_STRING &&
                oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_STRING);
        QString retstr = oprand1->getStringValue();
        retstr.append(QString::number(oprand2->getNumericValue()));
        tmpretval->setStringValue(retstr);
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_subOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == nullptr || oprand2 == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() -
                                   oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_mulOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == nullptr || oprand2 == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() *
                                   oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_divOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == nullptr || oprand2 == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(oprand1->getNumericValue() /
                                   oprand2->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _pstvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(oprand->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _ngtvOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        tmpretval->setType(finExecVariable::TP_NUMERIC);
        tmpretval->setNumericValue(0.0 - oprand->getNumericValue());
    } else {
        delete tmpretval;
        return finErrorKits::EC_INVALID_PARAM;
    }

    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();
    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_psAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorKits::EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::TP_NULL ) {
        oprand->setType(finExecVariable::TP_NUMERIC);
        oprand->setNumericValue(0.0);
    } else if ( oprand->getType() != finExecVariable::TP_NUMERIC ) {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *resvar = new finExecVariable();
    resvar->copyVariableValue(oprand);
    resvar->clearLeftValue();
    resvar->setLeftValue();

    oprand->setNumericValue(oprand->getNumericValue() + 1.0);
    *retval = resvar;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_prAccumOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorKits::EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::TP_NULL ) {
        oprand->setType(finExecVariable::TP_NUMERIC);
        oprand->setNumericValue(1.0);
    } else if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        oprand->setNumericValue(oprand->getNumericValue() + 1.0);
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = oprand;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_psDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorKits::EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::TP_NULL ) {
        oprand->setType(finExecVariable::TP_NUMERIC);
        oprand->setNumericValue(0.0);
    } else if ( oprand->getType() != finExecVariable::TP_NUMERIC ) {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *resvar = new finExecVariable();
    resvar->copyVariableValue(oprand);
    resvar->clearLeftValue();
    resvar->setLeftValue();

    oprand->setNumericValue(oprand->getNumericValue() - 1.0);
    *retval = resvar;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_prDescdOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    if ( !oprand->isLeftValue() || oprand->isWriteProtected() )
        return finErrorKits::EC_INVALID_PARAM;

    if ( oprand->getType() == finExecVariable::TP_NULL ) {
        oprand->setType(finExecVariable::TP_NUMERIC);
        oprand->setNumericValue(-1.0);
    } else if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        oprand->setNumericValue(oprand->getNumericValue() - 1.0);
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = oprand;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_modOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == nullptr || oprand2 == nullptr )
        return finErrorKits::EC_INVALID_PARAM;

    double opnum1, opnum2;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        opnum1 = oprand1->getNumericValue();
        opnum2 = oprand2->getNumericValue();
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    double divint = floor(opnum1 / opnum2);
    double modres = opnum1 - (divint * opnum2);

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue(modres);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_powOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    double opnum1, opnum2;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        opnum1 = oprand1->getNumericValue();
        opnum2 = oprand2->getNumericValue();
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue(pow(opnum1, opnum2));
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_factoriOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    int opnum;
    if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        opnum = (int)(floor(oprand->getNumericValue()));
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    double resnum = 1.0;
    for ( int i = 2; i <= opnum; i++ )
        resnum *= (double)i;

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue(resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_letOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    if ( !oprand1->isLeftValue() || oprand1->isWriteProtected() )
        return finErrorKits::EC_INVALID_PARAM;

    // To make the value receiver has the same type with the source variable.
    if ( oprand1->getType() != finExecVariable::TP_NULL &&
         oprand1->getType() != oprand2->getType() )
        return finErrorKits::EC_INVALID_PARAM;

    finErrorCode errcode = oprand1->smartCopyVariableValue(oprand2);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    *retval = oprand1;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode
_eqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);

    bool opssame;
    if ( oprand1 == NULL && oprand2 == NULL )
        opssame = true;
    else if ( oprand1 == NULL || oprand2 == NULL )
        opssame = false;
    else
        opssame = oprand1->isSameValue(oprand2);
    *retval = finExecOperartorCalc::buildStdLogicVar(opssame);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_grtOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        blval = (oprand1->getNumericValue() > oprand2->getNumericValue());
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = finExecOperartorCalc::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_lesOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        blval = (oprand1->getNumericValue() < oprand2->getNumericValue());
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = finExecOperartorCalc::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_neqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = oprands->at(0);
    finExecVariable *oprand2 = oprands->at(1);

    bool opssame;
    if ( oprand1 == NULL && oprand2 == NULL )
        opssame = true;
    else if ( oprand1 == NULL || oprand2 == NULL )
        opssame = false;
    else
        opssame = oprand1->isSameValue(oprand2);
    *retval = finExecOperartorCalc::buildStdLogicVar(!opssame);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_gteqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        blval = (oprand1->getNumericValue() >= oprand2->getNumericValue());
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = finExecOperartorCalc::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_lseqOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval;

    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        blval = (oprand1->getNumericValue() <= oprand2->getNumericValue());
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = finExecOperartorCalc::buildStdLogicVar(blval);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_logicNotOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval = finExecOperartorCalc::varLogicValue(oprand);
    *retval = finExecOperartorCalc::buildStdLogicVar(!blval);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_logicAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval1 = finExecOperartorCalc::varLogicValue(oprand1);
    bool blval2 = finExecOperartorCalc::varLogicValue(oprand2);
    *retval = finExecOperartorCalc::buildStdLogicVar(blval1 && blval2);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_logicOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval1 = finExecOperartorCalc::varLogicValue(oprand1);
    bool blval2 = finExecOperartorCalc::varLogicValue(oprand2);
    *retval = finExecOperartorCalc::buildStdLogicVar(blval1 || blval2);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_logicXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    bool blval1 = finExecOperartorCalc::varLogicValue(oprand1);
    bool blval2 = finExecOperartorCalc::varLogicValue(oprand2);
    *retval = finExecOperartorCalc::buildStdLogicVar(blval1 == blval2 ? false : true);
    if ( *retval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _accessOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *parent = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *aryidx = finExecVariable::transLinkTarget(oprands->at(1));
    if ( parent == NULL || aryidx == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *child = NULL;

    if ( (parent->getType() == finExecVariable::TP_NULL ||
          parent->getType() == finExecVariable::TP_ARRAY) &&
         aryidx->getType() == finExecVariable::TP_NUMERIC ) {
        double dbidx = aryidx->getNumericValue();

        child = parent->getVariableItemAt((int)dbidx);
        if ( child == NULL )
            return finErrorKits::EC_OUT_OF_MEMORY;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    *retval = child;
    if ( !child->isLeftValue() )
        child->removeFromArray();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_commaOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    if ( oprands->empty() ) {
        *retval = NULL;
        return finErrorKits::EC_NORMAL_WARN;
    }

    *retval = oprands->last();
    // Remove it from oprand to prevent dealloc.
    oprands->removeLast();
    return finErrorKits::EC_SUCCESS;
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
    finExecVariable *oprand = finExecVariable::transLinkTarget(oprands->at(0));
    if ( oprand == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    quint32 opnum;
    if ( oprand->getType() == finExecVariable::TP_NUMERIC ) {
        double opdbnum = oprand->getNumericValue();
        if ( opdbnum > _max_bitable_int || opdbnum < _min_bitable_int )
            return finErrorKits::EC_OVERFLOW;

        opdbnum = _bitable_neg_to_post(opdbnum);
        opnum = (quint32)opdbnum;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    opnum = ~opnum;

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue((double)opnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_bitAndOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorKits::EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 & opnum2);

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_bitOrOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorKits::EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 | opnum2);

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_bitXorOpCall(QList<finExecVariable *> *oprands, finExecVariable **retval)
{
    finExecVariable *oprand1 = finExecVariable::transLinkTarget(oprands->at(0));
    finExecVariable *oprand2 = finExecVariable::transLinkTarget(oprands->at(1));
    if ( oprand1 == NULL || oprand2 == NULL )
        return finErrorKits::EC_INVALID_PARAM;

    quint32 opnum1, opnum2;
    if ( oprand1->getType() == finExecVariable::TP_NUMERIC &&
         oprand2->getType() == finExecVariable::TP_NUMERIC ) {
        double opdbnum1 = oprand1->getNumericValue();
        double opdbnum2 = oprand2->getNumericValue();
        if ( opdbnum1 > _max_bitable_int || opdbnum1 < _min_bitable_int ||
             opdbnum2 > _max_bitable_int || opdbnum2 < _min_bitable_int )
            return finErrorKits::EC_OVERFLOW;

        opdbnum1 = _bitable_neg_to_post(opdbnum1);
        opdbnum2 = _bitable_neg_to_post(opdbnum2);
        opnum1 = (quint32)opdbnum1;
        opnum2 = (quint32)opdbnum2;
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    finExecVariable *tmpretval = new finExecVariable();
    if ( tmpretval == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    quint32 resnum = (opnum1 ^ opnum2);

    tmpretval->setType(finExecVariable::TP_NUMERIC);
    tmpretval->setNumericValue((double)resnum);
    tmpretval->clearLeftValue();
    tmpretval->setWriteProtected();

    *retval = tmpretval;
    return finErrorKits::EC_SUCCESS;
}

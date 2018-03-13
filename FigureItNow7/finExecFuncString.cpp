/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <qmath.h>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_str_len(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_left(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_right(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_mid(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_find(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_bk_find(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_str_trim(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);

static finErrorCode _sysfunc_chr_unicode(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_chr_ascii(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_unicode_chr(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ascii_chr(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);

static QString _defFuncCtg("String Operations");

static finErrorCode _sysfunc_str_len(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    if ( strvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString str = strvar->getStringValue();

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue((double)str.length());
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_str_len = {
    /*._funcName     =*/ QString("str_len"),
    /*._paramCsvList =*/ QString("str"),
    /*._funcCall     =*/ _sysfunc_str_len,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_len (str)"),
    /*._description  =*/ QString("Get the length or the number of characters of a given string."),
};

static finErrorCode _sysfunc_str_left(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *lenvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    lenvar = finExecVariable::transLinkTarget(env->findVariable("len"));
    if ( strvar == NULL || lenvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING ||
         lenvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    QString str = strvar->getStringValue();
    int len = (int)floor(lenvar->getNumericValue());

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_STRING);
    if ( len >= str.length() )
        retvar->setStringValue(str);
    else
        retvar->setStringValue(str.left(len));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_str_right(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *lenvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    lenvar = finExecVariable::transLinkTarget(env->findVariable("len"));
    if ( strvar == NULL || lenvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING ||
         lenvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    QString str = strvar->getStringValue();
    int len = (int)floor(lenvar->getNumericValue());

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_STRING);
    if ( len >= str.length() )
        retvar->setStringValue(str);
    else
        retvar->setStringValue(str.right(len));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_str_mid(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *posvar, *lenvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    posvar = finExecVariable::transLinkTarget(env->findVariable("pos"));
    lenvar = finExecVariable::transLinkTarget(env->findVariable("len"));
    if ( strvar == NULL || posvar == NULL || lenvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING ||
         posvar->getType() != finExecVariable::TP_NUMERIC ||
         lenvar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    QString str = strvar->getStringValue();
    int pos = (int)floor(posvar->getNumericValue());
    int len = (int)floor(lenvar->getNumericValue());

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    retvar->setType(finExecVariable::TP_STRING);
    if ( len >= str.length() )
        retvar->setStringValue(str);
    else
        retvar->setStringValue(str.mid(pos, len));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_str_find(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *substrvar, *fromvar, *casevar, *retvar;
    finErrorCode errcode;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    substrvar = finExecVariable::transLinkTarget(env->findVariable("substr"));
    fromvar = finExecVariable::transLinkTarget(env->findVariable("from"));
    casevar = finExecVariable::transLinkTarget(env->findVariable("case"));
    if ( strvar == NULL || substrvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING ||
         substrvar->getType() != finExecVariable::TP_STRING ||
         (fromvar != NULL && fromvar->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    QString str = strvar->getStringValue();
    QString substr = substrvar->getStringValue();
    int from = 0;
    bool casecare = true;
    if ( fromvar != NULL )
        from = (int)floor(fromvar->getNumericValue());
    if ( casevar != NULL ) {
        errcode = casevar->readBoolValue(&casecare);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    int retidx = str.indexOf(substr, from, casecare ? Qt::CaseSensitive : Qt::CaseInsensitive);
    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(retidx);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_str_bk_find(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *substrvar, *fromvar, *casevar, *retvar;
    finErrorCode errcode;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    substrvar = finExecVariable::transLinkTarget(env->findVariable("substr"));
    fromvar = finExecVariable::transLinkTarget(env->findVariable("from"));
    casevar = finExecVariable::transLinkTarget(env->findVariable("case"));
    if ( strvar == NULL || substrvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING ||
         substrvar->getType() != finExecVariable::TP_STRING ||
         (fromvar != NULL && fromvar->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    QString str = strvar->getStringValue();
    QString substr = substrvar->getStringValue();
    int from = 0;
    bool casecare = true;
    if ( fromvar != NULL )
        from = (int)floor(fromvar->getNumericValue());
    if ( casevar != NULL ) {
        errcode = casevar->readBoolValue(&casecare);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
    }

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    int retidx = str.lastIndexOf(substr, from, casecare ? Qt::CaseSensitive : Qt::CaseInsensitive);
    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(retidx);
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_str_trim(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *strvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    strvar = finExecVariable::transLinkTarget(env->findVariable("str"));
    if ( strvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( strvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString str = strvar->getStringValue();

    retvar->setType(finExecVariable::TP_STRING);
    retvar->setStringValue(str.trimmed());
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_chr_unicode(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *chrvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    chrvar = finExecVariable::transLinkTarget(env->findVariable("chr"));
    if ( chrvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( chrvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString str = chrvar->getStringValue();
    QChar chr = (str.isEmpty() ? QChar(0) : str.at(0));

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(chr.unicode());
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_chr_ascii(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *chrvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    chrvar = finExecVariable::transLinkTarget(env->findVariable("chr"));
    if ( chrvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( chrvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString str = chrvar->getStringValue();
    QChar chr = (str.isEmpty() ? QChar(0) : str.at(0));

    retvar->setType(finExecVariable::TP_NUMERIC);
    retvar->setNumericValue(chr.toLatin1());
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_unicode_chr(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *codevar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    codevar = finExecVariable::transLinkTarget(env->findVariable("code"));
    if ( codevar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( codevar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    ushort code = (ushort)codevar->getNumericValue();
    QChar chr = QChar(code);

    retvar->setType(finExecVariable::TP_STRING);
    retvar->setStringValue(QString(chr));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_ascii_chr(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *codevar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    codevar = finExecVariable::transLinkTarget(env->findVariable("code"));
    if ( codevar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( codevar->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    char chr = (char)codevar->getNumericValue();

    retvar->setType(finExecVariable::TP_STRING);
    retvar->setStringValue(QString(chr));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _finSysFuncStringList[] = {
    _funcRegItem_str_len,
    { QString("str_left"),    QString("str,len"),              _sysfunc_str_left    },
    { QString("str_right"),   QString("str,len"),              _sysfunc_str_right   },
    { QString("str_mid"),     QString("str,pos,len"),          _sysfunc_str_mid     },
    { QString("str_find"),    QString("str,substr,from,case"), _sysfunc_str_find    },
    { QString("str_bk_find"), QString("str,substr,from,case"), _sysfunc_str_bk_find },
    { QString("str_trim"),    QString("str"),                  _sysfunc_str_trim    },

    { QString("chr_unicode"), QString("chr"),                  _sysfunc_chr_unicode },
    { QString("chr_ascii"),   QString("chr"),                  _sysfunc_chr_ascii   },
    { QString("unicode_chr"), QString("code"),                 _sysfunc_unicode_chr },
    { QString("ascii_chr"),   QString("code"),                 _sysfunc_ascii_chr   },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncString()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncStringList, _defFuncCtg);
}

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

static struct finExecSysFuncRegItem _funcRegItem_str_left = {
    /*._funcName     =*/ QString("str_left"),
    /*._paramCsvList =*/ QString("str,len"),
    /*._funcCall     =*/ _sysfunc_str_left,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_left (str, len)"),
    /*._description  =*/ QString("Get the left part of a given string with the given length. If the string length is "
                                 "smaller than the expected length, the whole string is returned."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_str_right = {
    /*._funcName     =*/ QString("str_right"),
    /*._paramCsvList =*/ QString("str,len"),
    /*._funcCall     =*/ _sysfunc_str_right,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_right (str, len)"),
    /*._description  =*/ QString("Get the right part of a given string with the given length. If the string length is "
                                 "smaller than the expected length, the whole string is returned."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_str_mid = {
    /*._funcName     =*/ QString("str_mid"),
    /*._paramCsvList =*/ QString("str,len"),
    /*._funcCall     =*/ _sysfunc_str_mid,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_mid (str, pos, len)"),
    /*._description  =*/ QString("Get the sub-string of a given string with the given starting position and length. "
                                 "If the left length is smaller than the expected length from the starting position, "
                                 "all the characters rangingfrom the starting position to the end will be returned."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_str_find = {
    /*._funcName     =*/ QString("str_find"),
    /*._paramCsvList =*/ QString("str,substr,from,case"),
    /*._funcCall     =*/ _sysfunc_str_find,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_find (str,substr[,from,case])"),
    /*._description  =*/ QString("Find the sub-string inside a given string, and return the very first index of the "
                                 "sub-string inside a string. There are two optional arguments, which \'from\' gives "
                                 "the starting position of searching, and the boolean value \'case\' indicates whether "
                                 "case sensitive."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_str_bk_find = {
    /*._funcName     =*/ QString("str_bk_find"),
    /*._paramCsvList =*/ QString("str,substr,from,case"),
    /*._funcCall     =*/ _sysfunc_str_bk_find,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_bk_find (str,substr[,from,case])"),
    /*._description  =*/ QString("Find the sub-string inside a given string, and return the very last index of the "
                                 "sub-string inside a string. There are two optional arguments, which \'from\' gives "
                                 "the starting position of searching, and the boolean value \'case\' indicates whether "
                                 "case sensitive."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_str_trim = {
    /*._funcName     =*/ QString("str_trim"),
    /*._paramCsvList =*/ QString("str"),
    /*._funcCall     =*/ _sysfunc_str_trim,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("str_trim (str)"),
    /*._description  =*/ QString("Return a sub-string of the input, whose white-spaces are remove at the beginning "
                                 "and the end."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_chr_unicode = {
    /*._funcName     =*/ QString("chr_unicode"),
    /*._paramCsvList =*/ QString("chr"),
    /*._funcCall     =*/ _sysfunc_chr_unicode,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("chr_unicode (chr)"),
    /*._description  =*/ QString("Return the unicode of the given character. If a string is given as argument, only "
                                 "the unicode of the first character is returned."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_chr_ascii = {
    /*._funcName     =*/ QString("chr_ascii"),
    /*._paramCsvList =*/ QString("chr"),
    /*._funcCall     =*/ _sysfunc_chr_ascii,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("chr_ascii (chr)"),
    /*._description  =*/ QString("Return the ASCII code of the given character. If a string is given as argument, only "
                                 "the ASCII code of the first character is returned."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_unicode_chr = {
    /*._funcName     =*/ QString("unicode_chr"),
    /*._paramCsvList =*/ QString("code"),
    /*._funcCall     =*/ _sysfunc_unicode_chr,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("unicode_chr (code)"),
    /*._description  =*/ QString("Return the character corresponding to the given unicode in numerical variable."),
};

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

static struct finExecSysFuncRegItem _funcRegItem_ascii_chr = {
    /*._funcName     =*/ QString("ascii_chr"),
    /*._paramCsvList =*/ QString("code"),
    /*._funcCall     =*/ _sysfunc_ascii_chr,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ascii_chr (code)"),
    /*._description  =*/ QString("Return the character corresponding to the given ASCII code in numerical variable."),
};

static struct finExecSysFuncRegItem _finSysFuncStringList[] = {
    _funcRegItem_str_len,
    _funcRegItem_str_left,
    _funcRegItem_str_right,
    _funcRegItem_str_mid,
    _funcRegItem_str_find,
    _funcRegItem_str_bk_find,
    _funcRegItem_str_trim,

    _funcRegItem_chr_unicode,
    _funcRegItem_chr_ascii,
    _funcRegItem_unicode_chr,
    _funcRegItem_ascii_chr,

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncString()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncStringList, _defFuncCtg);
}

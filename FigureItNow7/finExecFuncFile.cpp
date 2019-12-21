/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 11th, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include <QFile>
#include <QImage>
#include <QTextStream>
#include <QStringList>

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finExecAlg.h"

static QString _defFuncCtg("File Operations");

static finErrorCode _sysfunc_load_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString filename = fnvar->getStringValue();
    QImage image = QImage(filename);
    if ( image.isNull() )
        return finErrorKits::EC_FILE_NOT_OPEN;

    retvar->setType(finExecVariable::TP_IMAGE);
    retvar->setImageValue(image.convertToFormat(QImage::Format_ARGB32));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_load_image = {
    /*._funcName     =*/ QString("load_image"),
    /*._paramCsvList =*/ QString("fn"),
    /*._funcCall     =*/ _sysfunc_load_image,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("load_image (fn)"),
    /*._description  =*/ QString("Load an image from the given file path."),
};

static finErrorCode _sysfunc_load_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString filename = fnvar->getStringValue();
    QFile fp(filename);
    if ( !fp.open(QIODevice::ReadOnly | QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    retvar->setType(finExecVariable::TP_ARRAY);
    int currow = 0;
    QTextStream tsin(&fp);
    while ( !tsin.atEnd() ) {
        QString lnstr = tsin.readLine();
        finExecVariable *rowvar = retvar->getVariableItemAt(currow);

        finExecAlg::csStringToNumArrayVar(lnstr, rowvar);
        currow++;
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();
    fp.close();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_load_numerical_csv = {
    /*._funcName     =*/ QString("load_numerical_csv"),
    /*._paramCsvList =*/ QString("fn"),
    /*._funcCall     =*/ _sysfunc_load_numerical_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("load_numerical_csv (fn)"),
    /*._description  =*/ QString("Load a numerical array from a Comma-Separated Values (CSV) file. The returned array "
                                 "may be one or two dimensions based on the number of rows or columns in the file. A "
                                 "NULL item will be placed if the corresponding data item is not a number."),
};

static finErrorCode _sysfunc_load_string_csv(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString filename = fnvar->getStringValue();
    QFile fp(filename);
    if ( !fp.open(QIODevice::ReadOnly | QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    retvar->setType(finExecVariable::TP_ARRAY);
    int currow = 0;
    QTextStream tsin(&fp);
    while ( !tsin.atEnd() ) {
        QString lnstr = tsin.readLine();
        finExecVariable *rowvar = retvar->getVariableItemAt(currow);

        finExecAlg::csStringToStrArrayVar(lnstr, rowvar);
        currow++;
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();
    fp.close();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_load_string_csv = {
    /*._funcName     =*/ QString("load_string_csv"),
    /*._paramCsvList =*/ QString("fn"),
    /*._funcCall     =*/ _sysfunc_load_string_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("load_string_csv (fn)"),
    /*._description  =*/ QString("Load a text array from a Comma-Separated Values (CSV) file. The returned array may "
                                 "be one or two dimensions based on the number of rows or columns in the file."),
};

static finErrorCode _sysfunc_load_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString filename = fnvar->getStringValue();
    QFile fp(filename);
    if ( !fp.open(QIODevice::ReadOnly | QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    retvar->setType(finExecVariable::TP_ARRAY);
    int currow = 0;
    QTextStream tsin(&fp);
    while ( !tsin.atEnd() ) {
        QString lnstr = tsin.readLine();
        finExecVariable *rowvar = retvar->getVariableItemAt(currow);

        finExecAlg::csStringToArrayVar(lnstr, rowvar);
        currow++;
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();
    fp.close();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_load_auto_csv = {
    /*._funcName     =*/ QString("load_auto_csv"),
    /*._paramCsvList =*/ QString("fn"),
    /*._funcCall     =*/ _sysfunc_load_auto_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("load_auto_csv (fn)"),
    /*._description  =*/ QString("Load an array from a Comma-Separated Values (CSV) file. The returned array may be "
                                 "one or two dimensions based on the number of rows or columns in the file. The data "
                                 "type of each array item depends on the contents of the corresponding data item in "
                                 "the file. If the data is a number, the data item will be set as a numerical "
                                 "variable; otherwise, a string variable or a NULL variable will be set."),
};

static finErrorCode _sysfunc_save_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( fnvar == nullptr || aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString filename = fnvar->getStringValue();
    QFile fp(filename);
    if ( !fp.open(QIODevice::WriteOnly | QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    QTextStream tsout(&fp);
    if ( aryvar->hasMultiLevel() ) {
        int lncnt = aryvar->getArrayLength();
        for ( int i = 0; i < lncnt; i++ ) {
            finExecVariable *rowvar = aryvar->getVariableItemAt(i);
            tsout << finExecAlg::numArrayVarToCsString(rowvar) << endl;
        }
    } else {
        tsout << finExecAlg::numArrayVarToCsString(aryvar) << endl;
    }
    fp.close();

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_save_numerical_csv = {
    /*._funcName     =*/ QString("save_numerical_csv"),
    /*._paramCsvList =*/ QString("fn,ary"),
    /*._funcCall     =*/ _sysfunc_save_numerical_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("save_numerical_csv (fn, ary)"),
    /*._description  =*/ QString("Save a numerical array to a Comma-Separated Values (CSV) file. A NULL item will be "
                                 "placed if the corresponding data item is not a number."),
};

static finErrorCode _sysfunc_save_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *aryvar;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( fnvar == nullptr || aryvar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString filename = fnvar->getStringValue();
    QFile fp(filename);
    if ( !fp.open(QIODevice::WriteOnly | QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    QTextStream tsout(&fp);
    if ( aryvar->hasMultiLevel() ) {
        int lncnt = aryvar->getArrayLength();
        for ( int i = 0; i < lncnt; i++ ) {
            finExecVariable *rowvar = aryvar->getVariableItemAt(i);
            tsout << finExecAlg::arrayVarToCsString(rowvar) << endl;
        }
    } else {
        tsout << finExecAlg::arrayVarToCsString(aryvar) << endl;
    }
    fp.close();

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_save_string_csv = {
    /*._funcName     =*/ QString("save_string_csv"),
    /*._paramCsvList =*/ QString("fn,ary"),
    /*._funcCall     =*/ _sysfunc_save_auto_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("save_string_csv (fn, ary)"),
    /*._description  =*/ QString("Save a string array to a Comma-Separated Values (CSV) file. If an array item is not "
                                 "string, it will be converted to string if possible before write into the file."),
};

static struct finExecSysFuncRegItem _funcRegItem_save_auto_csv = {
    /*._funcName     =*/ QString("save_auto_csv"),
    /*._paramCsvList =*/ QString("fn,ary"),
    /*._funcCall     =*/ _sysfunc_save_auto_csv,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("save_auto_csv (fn, ary)"),
    /*._description  =*/ QString("Save an array to a Comma-Separated Values (CSV) file. The data type of an array "
                                 "item will be recognized automatically."),
};

static struct finExecSysFuncRegItem _finSysFuncFileList[] = {
    _funcRegItem_load_image,

    _funcRegItem_load_numerical_csv,
    _funcRegItem_load_string_csv,
    _funcRegItem_load_auto_csv,
    _funcRegItem_save_numerical_csv,
    _funcRegItem_save_string_csv,
    _funcRegItem_save_auto_csv,

    { QString(), QString(), nullptr, _defFuncCtg, QString(), QString() }
};

finErrorCode finExecFunction::registSysFuncFile()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFileList, _defFuncCtg);
}

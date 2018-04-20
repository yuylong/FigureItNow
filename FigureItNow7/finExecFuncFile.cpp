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

static finErrorCode _sysfunc_load_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_load_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_load_string_csv(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_load_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_save_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_save_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl);

static QString _defFuncCtg("File Operations");

static finErrorCode _sysfunc_load_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
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

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
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

static finErrorCode _sysfunc_load_string_csv(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
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

static finErrorCode _sysfunc_load_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *retvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    if ( fnvar == NULL )
        return finErrorKits::EC_NOT_FOUND;
    if ( fnvar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
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

static finErrorCode _sysfunc_save_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *aryvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( fnvar == NULL || aryvar == NULL )
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

static finErrorCode _sysfunc_save_auto_csv(finExecFunction *self, finExecEnvironment *env,
                                           finExecMachine *machine, finExecFlowControl *flowctl)
{
    finExecVariable *fnvar, *aryvar;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorKits::EC_NULL_POINTER;

    fnvar = finExecVariable::transLinkTarget(env->findVariable("fn"));
    aryvar = finExecVariable::transLinkTarget(env->findVariable("ary"));
    if ( fnvar == NULL || aryvar == NULL )
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

static struct finExecSysFuncRegItem _finSysFuncFileList[] = {
    _funcRegItem_load_image,

    { QString("load_numerical_csv"), QString("fn"),     _sysfunc_load_numerical_csv },
    { QString("load_string_csv"),    QString("fn"),     _sysfunc_load_string_csv    },
    { QString("load_auto_csv"),      QString("fn"),     _sysfunc_load_auto_csv      },
    { QString("save_numerical_csv"), QString("fn,ary"), _sysfunc_save_numerical_csv },
    { QString("save_string_csv"),    QString("fn,ary"), _sysfunc_save_auto_csv      },
    { QString("save_auto_csv"),      QString("fn,ary"), _sysfunc_save_auto_csv      },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncFile()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFileList, _defFuncCtg);
}

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

static finErrorCode _sysfunc_load_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_load_numerical_csv(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncFileList[] = {
    { QString("load_image"),         QString("fn"), _sysfunc_load_image         },
    { QString("load_numerical_csv"), QString("fn"), _sysfunc_load_numerical_csv },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncFile()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFileList);
}

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
        QStringList lnstrlist = lnstr.split(',');

        finExecVariable *rowvar = retvar->getVariableItemAt(currow);
        rowvar->setType(finExecVariable::TP_ARRAY);
        if ( lnstr.isEmpty() )
            continue;

        rowvar->preallocArrayLength(lnstrlist.length());
        int curcol = 0;
        foreach (QString stritem, lnstrlist) {
            finExecVariable *colvar = rowvar->getVariableItemAt(curcol);
            bool numok = false;
            double valitem = stritem.toDouble(&numok);
            if ( !numok )
                valitem = 0.0;

            colvar->setType(finExecVariable::TP_NUMERIC);
            colvar->setNumericValue(valitem);
            curcol++;
        }
        currow++;
    }
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

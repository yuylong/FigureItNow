/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu, Jan 11th, 2017
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"

static finErrorCode _sysfunc_load_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);

static struct finExecSysFuncRegItem _finSysFuncFileList[] = {
    { QString("load_image"), QString("fn"), _sysfunc_load_image },

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
    if ( fnvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    retvar = new finExecVariable();
    if ( retvar == NULL )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString filename = fnvar->getStringValue();
    QImage image = QImage(filename);
    if ( image.isNull() )
        return finErrorKits::EC_FILE_NOT_OPEN;

    retvar->setType(finExecVariable::FIN_VR_TYPE_IMAGE);
    retvar->setImageValue(image.convertToFormat(QImage::Format_ARGB32));
    retvar->setWriteProtected();
    retvar->clearLeftValue();

    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

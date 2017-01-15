
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureObject.h"
#include "finFigureContainer.h"


static finErrorCode _sysfunc_clear_fig(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_line(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_line3d(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_named_color(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_read_fig_config(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_write_fig_config(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl);


static finExecSysFuncRegItem _finSysFuncFigureList[] = {
    { QString("clear_fig"),        QString(""),                  _sysfunc_clear_fig        },
    { QString("line"),             QString("x1,y1,x2,y2"),       _sysfunc_line             },
    { QString("line3d"),           QString("x1,y1,z1,x2,y2,z2"), _sysfunc_line3d           },
    { QString("named_color"),      QString("colorname"),         _sysfunc_named_color      },
    { QString("read_fig_config"),  QString("cfgname"),           _sysfunc_read_fig_config  },
    { QString("write_fig_config"), QString("cfgname,value"),     _sysfunc_write_fig_config },

    { QString(), QString(), NULL }
};

finErrorCode finExecFunction::registSysFuncFiguring()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFigureList);
}

static finErrorCode _sysfunc_clear_fig(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    env->getFigureContainer()->clearFigureObjects();

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_sysfunc_line(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *x1, *y1, *x2, *y2;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    x1 = finExecVariable::transLinkTarget(env->findVariable("x1"));
    y1 = finExecVariable::transLinkTarget(env->findVariable("y1"));
    x2 = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y2 = finExecVariable::transLinkTarget(env->findVariable("y2"));

    if ( x1 == NULL || y1 == NULL || x2 == NULL || y2 == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( x1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         x2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectLine *foline = new finFigureObjectLine();
    if ( foline == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    foline->setPoint1(x1->getNumericValue(), y1->getNumericValue());
    foline->setPoint2(x2->getNumericValue(), y2->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(foline);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foline;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode
_sysfunc_line3d(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *x1, *y1, *z1, *x2, *y2, *z2;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    x1 = finExecVariable::transLinkTarget(env->findVariable("x1"));
    y1 = finExecVariable::transLinkTarget(env->findVariable("y1"));
    z1 = finExecVariable::transLinkTarget(env->findVariable("z1"));
    x2 = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y2 = finExecVariable::transLinkTarget(env->findVariable("y2"));
    z2 = finExecVariable::transLinkTarget(env->findVariable("z2"));

    if ( x1 == NULL || y1 == NULL || z1 == NULL || x2 == NULL || y2 == NULL || z2 == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( x1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         z1->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         x2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         y2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         z2->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectLine3D *foline3d = new finFigureObjectLine3D();
    if ( foline3d == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    foline3d->setPoint1(x1->getNumericValue(), y1->getNumericValue(), z1->getNumericValue());
    foline3d->setPoint2(x2->getNumericValue(), y2->getNumericValue(), z2->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(foline3d);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foline3d;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_named_color(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;

    finExecVariable *colornamevar = finExecVariable::transLinkTarget(env->findVariable("colorname"));
    if ( colornamevar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( colornamevar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString colorname = colornamevar->getStringValue();
    if ( !QColor::isValidColor(colorname) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QColor color = QColor(colorname);
    //if ( !color.isValid() )
    //    return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if (retvar == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finErrorCode errcode = retvar->setupColorValue(color);
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    retvar->setWriteProtected();
    retvar->clearLeftValue();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_read_fig_config(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finFigureConfig *figconfig;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    figconfig = env->getFigureContainer()->getFigureConfig();
    if ( figconfig == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    if ( cfgnamevar == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    cfgvalue = new finExecVariable();
    if ( cfgvalue == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "dot_size") == 0 ) {
        cfgvalue->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getDotSize());
    } else if ( QString::compare(cfgname, "border_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getBorderColor());
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "fill_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getFillColor());
        if ( finErrorCodeKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else {
        delete cfgvalue;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    cfgvalue->setWriteProtected();
    cfgvalue->clearLeftValue();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(cfgvalue);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_write_fig_config(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finFigureConfig *figconfig;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    figconfig = env->getFigureContainer()->getFigureConfig();
    if ( figconfig == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    cfgvalue = finExecVariable::transLinkTarget(env->findVariable("value"));
    if ( cfgnamevar == NULL || cfgvalue == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "dot_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        figconfig->setDotSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "border_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setBorderColor(color);
    } else if ( QString::compare(cfgname, "fill_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFillColor(color);
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}


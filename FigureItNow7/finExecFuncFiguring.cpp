
#include "finExecFunction.h"

#include "finExecVariable.h"
#include "finExecEnvironment.h"
#include "finExecMachine.h"
#include "finFigureObject.h"
#include "finFigureArrow.h"
#include "finFigureContainer.h"


static finErrorCode _sysfunc_clear_fig(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_line(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polyline(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_rect(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polygon(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_circle(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ellipse(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_text(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_axis(finExecFunction *self, finExecEnvironment *env,
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
    { QString("clear_fig"),        QString(""),                            _sysfunc_clear_fig        },
    { QString("line"),             QString("x1,y1,x2,y2"),                 _sysfunc_line             },
    { QString("polyline"),         QString("x1,y1,x2,y2"),                 _sysfunc_polyline         },
    { QString("rect"),             QString("cx,cy,w,h,rad"),               _sysfunc_rect             },
    { QString("polygon"),          QString("x1,y1,x2,y2"),                 _sysfunc_polygon          },
    { QString("circle"),           QString("cx,cy,r"),                     _sysfunc_circle           },
    { QString("ellipse"),          QString("cx,cy,lr,sr,rad"),             _sysfunc_ellipse          },
    { QString("draw_text"),        QString("text,cx,cy,rad,sc"),           _sysfunc_draw_text        },
    { QString("draw_image"),       QString("image,cx,cy,rad,sc"),          _sysfunc_draw_text        },
    { QString("axis"),             QString("sx,sy,tx,ty,rx1,rx2,ry1,ry2"), _sysfunc_axis             },
    { QString("line3d"),           QString("x1,y1,z1,x2,y2,z2"),           _sysfunc_line3d           },
    { QString("named_color"),      QString("colorname"),                   _sysfunc_named_color      },
    { QString("read_fig_config"),  QString("cfgname"),                     _sysfunc_read_fig_config  },
    { QString("write_fig_config"), QString("cfgname,value"),               _sysfunc_write_fig_config },

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

static finErrorCode _sysfunc_polyline(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *ptx, *pty;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finFigureObjectPolyline *fopolyline = new finFigureObjectPolyline();
    if ( fopolyline == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    ptx = finExecVariable::transLinkTarget(env->findVariable("x1"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y1"));
    if ( ptx == NULL || pty == NULL ||
         ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
        delete fopolyline;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }
    fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    ptx = finExecVariable::transLinkTarget(env->findVariable("x2"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( ptx == NULL || pty == NULL ||
         ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
        delete fopolyline;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }
    fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    int idx = 0;
    while ( true ) {
        ptx = finExecFunction::getExtendArgAt(env, idx);
        pty = finExecFunction::getExtendArgAt(env, idx + 1);
        if ( ptx == NULL || pty == NULL )
            break;
        if ( ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
             pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
            delete fopolyline;
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        }

        fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());
        idx += 2;
    }

    errcode = env->getFigureContainer()->appendFigureObject(fopolyline);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete fopolyline;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_rect(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *w, *h, *rad;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    w = finExecVariable::transLinkTarget(env->findVariable("w"));
    h = finExecVariable::transLinkTarget(env->findVariable("h"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));

    if ( cx == NULL || cy == NULL || w == NULL || h == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cy->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         w->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         h->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (rad != NULL && rad->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                         rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectRect *forect = new finFigureObjectRect();
    if ( forect == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    forect->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    forect->setSize(w->getNumericValue(), h->getNumericValue());
    if ( rad != NULL && rad->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        forect->setRadian(rad->getNumericValue());
    else
        forect->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(forect);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete forect;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_polygon(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *ptx, *pty;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finFigureObjectPolygon *fopolygon = new finFigureObjectPolygon();
    if ( fopolygon == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    ptx = finExecVariable::transLinkTarget(env->findVariable("x1"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y1"));
    if ( ptx == NULL || pty == NULL ||
         ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
        delete fopolygon;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }
    fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    ptx = finExecVariable::transLinkTarget(env->findVariable("x2"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( ptx == NULL || pty == NULL ||
         ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
        delete fopolygon;
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }
    fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    int idx = 0;
    while ( true ) {
        ptx = finExecFunction::getExtendArgAt(env, idx);
        pty = finExecFunction::getExtendArgAt(env, idx + 1);
        if ( ptx == NULL || pty == NULL )
            break;
        if ( ptx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
             pty->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) {
            delete fopolygon;
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        }

        fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());
        idx += 2;
    }

    errcode = env->getFigureContainer()->appendFigureObject(fopolygon);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete fopolygon;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_circle(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *r;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    r = finExecVariable::transLinkTarget(env->findVariable("r"));

    if ( cx == NULL || cy == NULL || r == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cy->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         r->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectEllipse *circle = new finFigureObjectEllipse();
    if ( circle == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    circle->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    circle->setLongRadius(r->getNumericValue());
    circle->setShortRadius(r->getNumericValue());
    circle->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(circle);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete circle;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_ellipse(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *lr, *sr, *rad;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    lr = finExecVariable::transLinkTarget(env->findVariable("lr"));
    sr = finExecVariable::transLinkTarget(env->findVariable("sr"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));

    if ( cx == NULL || cy == NULL || lr == NULL || sr == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cy->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         lr->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         sr->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (rad != NULL && rad->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                         rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectEllipse *foellipse = new finFigureObjectEllipse();
    if ( foellipse == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    foellipse->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    foellipse->setLongRadius(lr->getNumericValue());
    foellipse->setShortRadius(sr->getNumericValue());
    if ( rad != NULL && rad->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        foellipse->setRadian(rad->getNumericValue());
    else
        foellipse->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(foellipse);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foellipse;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_draw_text(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *text, *cx, *cy, *rad, *scale;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    text = finExecVariable::transLinkTarget(env->findVariable("text"));
    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    scale = finExecVariable::transLinkTarget(env->findVariable("sc"));

    if ( text == NULL || cx == NULL || cy == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( text->getType() != finExecVariable::FIN_VR_TYPE_STRING ||
         cx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cy->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (rad != NULL && rad->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                         rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) ||
         (scale != NULL && scale->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                           scale->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC))
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectText *fotext = new finFigureObjectText();
    if ( fotext == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    fotext->setText(text->getStringValue());
    fotext->setBasePoint(cx->getNumericValue(), cy->getNumericValue());
    if ( rad != NULL && rad->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        fotext->setRadian(rad->getNumericValue());
    else
        fotext->setRadian(0.0);
    if ( scale != NULL && scale->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        fotext->setScale(scale->getNumericValue());
    else
        fotext->setScale(1.0);

    errcode = env->getFigureContainer()->appendFigureObject(fotext);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete fotext;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_draw_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *image, *cx, *cy, *rad, *scale;

    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    image = finExecVariable::transLinkTarget(env->findVariable("image"));
    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    scale = finExecVariable::transLinkTarget(env->findVariable("sc"));

    if ( image == NULL || cx == NULL || cy == NULL )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    if ( (image->getType() != finExecVariable::FIN_VR_TYPE_STRING &&
          image->getType() != finExecVariable::FIN_VR_TYPE_IMAGE) ||
         cx->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         cy->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         (rad != NULL && rad->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                         rad->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC) ||
         (scale != NULL && scale->getType() != finExecVariable::FIN_VR_TYPE_NULL &&
                           scale->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC))
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

    finFigureObjectImage *foimg = new finFigureObjectImage();
    if ( foimg == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    if ( image->getType() == finExecVariable::FIN_VR_TYPE_STRING ) {
        QImage imginst = QImage(image->getStringValue());
        foimg->setImage(imginst);
    } else {
        foimg->setImage(image->getImageValue());
    }
    foimg->setBasePoint(cx->getNumericValue(), cy->getNumericValue());
    if ( rad != NULL && rad->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        foimg->setRadian(rad->getNumericValue());
    else
        foimg->setRadian(0.0);
    if ( scale != NULL && scale->getType() == finExecVariable::FIN_VR_TYPE_NUMERIC )
        foimg->setScale(scale->getNumericValue());
    else
        foimg->setScale(1.0);

    errcode = env->getFigureContainer()->appendFigureObject(foimg);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foimg;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

static finErrorCode _sysfunc_axis(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == NULL || env == NULL || machine == NULL || flowctl == NULL )
        return finErrorCodeKits::FIN_EC_NULL_POINTER;
    if ( env->getFigureContainer() == NULL )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    finFigureObjectAxis *foaxis = new finFigureObjectAxis();
    if ( foaxis == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finExecVariable *stepxvar = finExecVariable::transLinkTarget(env->findVariable("sx"));
    if ( stepxvar == NULL || stepxvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        foaxis->setAutoStepX();
    else
        foaxis->setStepX(stepxvar->getNumericValue());

    finExecVariable *stepyvar = finExecVariable::transLinkTarget(env->findVariable("sy"));
    if ( stepyvar == NULL || stepyvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        foaxis->setAutoStepY();
    else
        foaxis->setStepY(stepyvar->getNumericValue());

    finExecVariable *titlexvar = finExecVariable::transLinkTarget(env->findVariable("tx"));
    if ( titlexvar == NULL || titlexvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        foaxis->setTitleX(QString("x"));
    else
        foaxis->setTitleX(titlexvar->getStringValue());

    finExecVariable *titleyvar = finExecVariable::transLinkTarget(env->findVariable("ty"));
    if ( titleyvar == NULL || titleyvar->getType() != finExecVariable::FIN_VR_TYPE_STRING )
        foaxis->setTitleY(QString("y"));
    else
        foaxis->setTitleY(titleyvar->getStringValue());

    finExecVariable *minxvar = finExecVariable::transLinkTarget(env->findVariable("rx1"));
    finExecVariable *maxxvar = finExecVariable::transLinkTarget(env->findVariable("rx2"));
    if ( minxvar == NULL || minxvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         maxxvar == NULL || maxxvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        foaxis->setAutoRangeX();
    else
        foaxis->setRangeX(minxvar->getNumericValue(), maxxvar->getNumericValue());

    finExecVariable *minyvar = finExecVariable::transLinkTarget(env->findVariable("ry1"));
    finExecVariable *maxyvar = finExecVariable::transLinkTarget(env->findVariable("ry2"));
    if ( minyvar == NULL || minyvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC ||
         maxyvar == NULL || maxyvar->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
        foaxis->setAutoRangeY();
    else
        foaxis->setRangeY(minyvar->getNumericValue(), maxyvar->getNumericValue());

    finErrorCode errcode = env->getFigureContainer()->appendFigureObject(foaxis);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete foaxis;
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
    if ( !color.isValid() )
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;

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
    } else if ( QString::compare(cfgname, "start_arrow_type") == 0 ) {
        finFigureArrowType arwtype = figconfig->getStartArrowType();
        cfgvalue->setType(finExecVariable::FIN_VR_TYPE_STRING);
        cfgvalue->setStringValue(finFigureArrow::getTypeName(arwtype));
    } else if ( QString::compare(cfgname, "end_arrow_type") == 0 ) {
        finFigureArrowType arwtype = figconfig->getEndArrowType();
        cfgvalue->setType(finExecVariable::FIN_VR_TYPE_STRING);
        cfgvalue->setStringValue(finFigureArrow::getTypeName(arwtype));
    } else if ( QString::compare(cfgname, "font_name") == 0 ) {
        cfgvalue->setType(finExecVariable::FIN_VR_TYPE_STRING);
        cfgvalue->setStringValue(figconfig->getFontName());
    } else if ( QString::compare(cfgname, "font_size") == 0 ) {
        cfgvalue->setType(finExecVariable::FIN_VR_TYPE_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getFontPointSize());
    } else if ( QString::compare(cfgname, "font_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getFontColor());
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
    } else if ( QString::compare(cfgname, "start_arrow_type") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::FIN_VR_TYPE_STRING )
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        finFigureArrowType arwtype = finFigureArrow::parseTypeString(cfgvalue->getStringValue());
        figconfig->setStartArrowType(arwtype);
    } else if ( QString::compare(cfgname, "end_arrow_type") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::FIN_VR_TYPE_STRING )
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        finFigureArrowType arwtype = finFigureArrow::parseTypeString(cfgvalue->getStringValue());
        figconfig->setEndArrowType(arwtype);
    } else if ( QString::compare(cfgname, "font_name") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::FIN_VR_TYPE_STRING )
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        figconfig->setFontName(cfgvalue->getStringValue());
    } else if ( QString::compare(cfgname, "font_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::FIN_VR_TYPE_NUMERIC )
            return finErrorCodeKits::FIN_EC_INVALID_PARAM;
        figconfig->setFontPointSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "font_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorCodeKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFontColor(color);
    } else {
        return finErrorCodeKits::FIN_EC_INVALID_PARAM;
    }

    flowctl->setFlowNext();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

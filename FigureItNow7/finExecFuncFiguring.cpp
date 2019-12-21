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
#include "finFigureObject.h"
#include "finFigureConfig.h"
#include "finFigureArrow.h"
#include "finFigureContainer.h"
#include "finGraphConfig.h"
#include "finGraphTrans.h"
#include "finPlotFunction.h"


static finErrorCode _sysfunc_clear_fig(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_dot(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_line(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polyline(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polyline_mat(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_rect(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polygon(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_polygon_mat(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_circle(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_ellipse(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_text(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_pinned_text(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_draw_pinned_image(finExecFunction *self, finExecEnvironment *env,
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
static finErrorCode _sysfunc_read_graph_config(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl);
static finErrorCode _sysfunc_write_graph_config(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl);

static QString _defFuncCtg("Figuring");

static finErrorCode _sysfunc_clear_fig(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    env->getFigureContainer()->clearFigureObjects();

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_clear_fig = {
    /*._funcName     =*/ QString("clear_fig"),
    /*._paramCsvList =*/ QString(""),
    /*._funcCall     =*/ _sysfunc_clear_fig,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("clear_fig ()"),
    /*._description  =*/ QString("Clear all the figure objects on the panel."),
};

static finErrorCode _sysfunc_draw_dot(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    if ( cx == nullptr || cy == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectDot *fodot = new finFigureObjectDot();
    if ( fodot == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    fodot->setPoint(cx->getNumericValue(), cy->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(fodot);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fodot;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_draw_dot = {
    /*._funcName     =*/ QString("draw_dot"),
    /*._paramCsvList =*/ QString("cx,cy"),
    /*._funcCall     =*/ _sysfunc_draw_dot,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("draw_dot (cx, cy)"),
    /*._description  =*/ QString("Draw a dot at given position on the panel."),
};

static finErrorCode
_sysfunc_line(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *x1, *y1, *x2, *y2;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    x1 = finExecVariable::transLinkTarget(env->findVariable("x1"));
    y1 = finExecVariable::transLinkTarget(env->findVariable("y1"));
    x2 = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y2 = finExecVariable::transLinkTarget(env->findVariable("y2"));

    if ( x1 == nullptr || y1 == nullptr || x2 == nullptr || y2 == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( x1->getType() != finExecVariable::TP_NUMERIC ||
         y1->getType() != finExecVariable::TP_NUMERIC ||
         x2->getType() != finExecVariable::TP_NUMERIC ||
         y2->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectLine *foline = new finFigureObjectLine();
    if ( foline == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    foline->setPoint1(x1->getNumericValue(), y1->getNumericValue());
    foline->setPoint2(x2->getNumericValue(), y2->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(foline);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete foline;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_line = {
    /*._funcName     =*/ QString("line"),
    /*._paramCsvList =*/ QString("x1,y1,x2,y2"),
    /*._funcCall     =*/ _sysfunc_line,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("line (x1, y1, x2, y2)"),
    /*._description  =*/ QString("Draw a line at given start and end positions on the panel."),
};

static finErrorCode _sysfunc_polyline(finExecFunction *self, finExecEnvironment *env,
                                      finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *ptx, *pty;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finFigureObjectPolyline *fopolyline = new finFigureObjectPolyline();
    if ( fopolyline == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    ptx = finExecVariable::transLinkTarget(env->findVariable("x1"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y1"));
    if ( ptx == nullptr || pty == nullptr ||
         ptx->getType() != finExecVariable::TP_NUMERIC ||
         pty->getType() != finExecVariable::TP_NUMERIC) {
        delete fopolyline;
        return finErrorKits::EC_INVALID_PARAM;
    }
    fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    ptx = finExecVariable::transLinkTarget(env->findVariable("x2"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( ptx == nullptr || pty == nullptr ||
         ptx->getType() != finExecVariable::TP_NUMERIC ||
         pty->getType() != finExecVariable::TP_NUMERIC) {
        delete fopolyline;
        return finErrorKits::EC_INVALID_PARAM;
    }
    fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    int idx = 0;
    while ( true ) {
        ptx = finExecFunction::getExtendArgAt(env, idx);
        pty = finExecFunction::getExtendArgAt(env, idx + 1);
        if ( ptx == nullptr || pty == nullptr )
            break;
        if ( ptx->getType() != finExecVariable::TP_NUMERIC ||
             pty->getType() != finExecVariable::TP_NUMERIC) {
            delete fopolyline;
            return finErrorKits::EC_INVALID_PARAM;
        }

        fopolyline->appendPoint(ptx->getNumericValue(), pty->getNumericValue());
        idx += 2;
    }

    errcode = env->getFigureContainer()->appendFigureObject(fopolyline);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fopolyline;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_polyline = {
    /*._funcName     =*/ QString("polyline"),
    /*._paramCsvList =*/ QString("x1,y1,x2,y2"),
    /*._funcCall     =*/ _sysfunc_polyline,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("polyline (x1, y1, x2, y2, ...)"),
    /*._description  =*/ QString("Draw multiple continuous lines at given positions on the panel."),
};

static finErrorCode _sysfunc_polyline_mat(finExecFunction *self, finExecEnvironment *env,
                                          finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));
    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finFigureObjectPolyline *fopolyline = new finFigureObjectPolyline();
    if ( fopolyline == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    fopolyline->appendPoints(ptlist);
    errcode = env->getFigureContainer()->appendFigureObject(fopolyline);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fopolyline;
        return errcode;
    }

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_polyline_mat = {
    /*._funcName     =*/ QString("polyline_mat"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_polyline_mat,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("polyline_mat (xary, yary)"),
    /*._description  =*/ QString("Draw multiple continuous lines at given positions on the panel. The points are "
                                 "assigned with two arrays of x and y coordinates or a matrix with two columns."),
};

static finErrorCode _sysfunc_rect(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *w, *h, *rad;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    w = finExecVariable::transLinkTarget(env->findVariable("w"));
    h = finExecVariable::transLinkTarget(env->findVariable("h"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));

    if ( cx == nullptr || cy == nullptr || w == nullptr || h == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC ||
         w->getType() != finExecVariable::TP_NUMERIC ||
         h->getType() != finExecVariable::TP_NUMERIC ||
         (rad != nullptr && rad->getType() != finExecVariable::TP_NULL &&
                            rad->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectRect *forect = new finFigureObjectRect();
    if ( forect == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    forect->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    forect->setSize(w->getNumericValue(), h->getNumericValue());
    if ( rad != nullptr && rad->getType() == finExecVariable::TP_NUMERIC )
        forect->setRadian(rad->getNumericValue());
    else
        forect->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(forect);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete forect;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_rect = {
    /*._funcName     =*/ QString("rect"),
    /*._paramCsvList =*/ QString("cx,cy,w,h,rad"),
    /*._funcCall     =*/ _sysfunc_rect,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("rect (cx, cy, w, h, [rad])"),
    /*._description  =*/ QString("Draw a rectangle with the central position, size and optional sloping radian."),
};

static finErrorCode _sysfunc_polygon(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *ptx, *pty;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finFigureObjectPolygon *fopolygon = new finFigureObjectPolygon();
    if ( fopolygon == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    ptx = finExecVariable::transLinkTarget(env->findVariable("x1"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y1"));
    if ( ptx == nullptr || pty == nullptr ||
         ptx->getType() != finExecVariable::TP_NUMERIC ||
         pty->getType() != finExecVariable::TP_NUMERIC) {
        delete fopolygon;
        return finErrorKits::EC_INVALID_PARAM;
    }
    fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    ptx = finExecVariable::transLinkTarget(env->findVariable("x2"));
    pty = finExecVariable::transLinkTarget(env->findVariable("y2"));
    if ( ptx == nullptr || pty == nullptr ||
         ptx->getType() != finExecVariable::TP_NUMERIC ||
         pty->getType() != finExecVariable::TP_NUMERIC) {
        delete fopolygon;
        return finErrorKits::EC_INVALID_PARAM;
    }
    fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());

    int idx = 0;
    while ( true ) {
        ptx = finExecFunction::getExtendArgAt(env, idx);
        pty = finExecFunction::getExtendArgAt(env, idx + 1);
        if ( ptx == nullptr || pty == nullptr )
            break;
        if ( ptx->getType() != finExecVariable::TP_NUMERIC ||
             pty->getType() != finExecVariable::TP_NUMERIC) {
            delete fopolygon;
            return finErrorKits::EC_INVALID_PARAM;
        }

        fopolygon->appendPoint(ptx->getNumericValue(), pty->getNumericValue());
        idx += 2;
    }

    errcode = env->getFigureContainer()->appendFigureObject(fopolygon);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fopolygon;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_polygon = {
    /*._funcName     =*/ QString("polygon"),
    /*._paramCsvList =*/ QString("x1,y1,x2,y2"),
    /*._funcCall     =*/ _sysfunc_polygon,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("polygon (x1, y1, x2, y2, ...)"),
    /*._description  =*/ QString("Draw a polygon with the vertexes listed in the argument list."),
};

static finErrorCode _sysfunc_polygon_mat(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finExecVariable *xaryvar = finExecVariable::transLinkTarget(env->findVariable("xary"));
    finExecVariable *yaryvar = finExecVariable::transLinkTarget(env->findVariable("yary"));
    QList<QPointF> ptlist;
    finErrorCode errcode = finExecVariable::transToPointList(xaryvar, yaryvar, &ptlist);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    finFigureObjectPolygon *fopolygon = new finFigureObjectPolygon();
    if ( fopolygon == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    fopolygon->appendPoints(ptlist);
    errcode = env->getFigureContainer()->appendFigureObject(fopolygon);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fopolygon;
        return errcode;
    }

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_polygon_mat = {
    /*._funcName     =*/ QString("polygon_mat"),
    /*._paramCsvList =*/ QString("xary,yary"),
    /*._funcCall     =*/ _sysfunc_polygon_mat,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("polygon_mat (xary, yary) or polygon_mat(xymat)"),
    /*._description  =*/ QString("Draw a polygon at given positions on the panel. The points are assigned with two "
                                 "arrays of x and y coordinates or a matrix with two columns."),
};

static finErrorCode _sysfunc_circle(finExecFunction *self, finExecEnvironment *env,
                                    finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *r;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    r = finExecVariable::transLinkTarget(env->findVariable("r"));

    if ( cx == nullptr || cy == nullptr || r == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC ||
         r->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectEllipse *circle = new finFigureObjectEllipse();
    if ( circle == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    circle->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    circle->setLongRadius(r->getNumericValue());
    circle->setShortRadius(r->getNumericValue());
    circle->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(circle);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete circle;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_circle = {
    /*._funcName     =*/ QString("circle"),
    /*._paramCsvList =*/ QString("cx,cy,r"),
    /*._funcCall     =*/ _sysfunc_circle,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("circle (cx, cy, r)"),
    /*._description  =*/ QString("Draw a circle with center position and radius."),
};

static finErrorCode _sysfunc_ellipse(finExecFunction *self, finExecEnvironment *env,
                                     finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cx, *cy, *lr, *sr, *rad;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    lr = finExecVariable::transLinkTarget(env->findVariable("lr"));
    sr = finExecVariable::transLinkTarget(env->findVariable("sr"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));

    if ( cx == nullptr || cy == nullptr || lr == nullptr || sr == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC ||
         lr->getType() != finExecVariable::TP_NUMERIC ||
         sr->getType() != finExecVariable::TP_NUMERIC ||
         (rad != nullptr && rad->getType() != finExecVariable::TP_NULL &&
                            rad->getType() != finExecVariable::TP_NUMERIC) )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectEllipse *foellipse = new finFigureObjectEllipse();
    if ( foellipse == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    foellipse->setCenterPoint(cx->getNumericValue(), cy->getNumericValue());
    foellipse->setLongRadius(lr->getNumericValue());
    foellipse->setShortRadius(sr->getNumericValue());
    if ( rad != nullptr && rad->getType() == finExecVariable::TP_NUMERIC )
        foellipse->setRadian(rad->getNumericValue());
    else
        foellipse->setRadian(0.0);

    errcode = env->getFigureContainer()->appendFigureObject(foellipse);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete foellipse;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static struct finExecSysFuncRegItem _funcRegItem_ellipse = {
    /*._funcName     =*/ QString("ellipse"),
    /*._paramCsvList =*/ QString("cx,cy,lr,sr,rad"),
    /*._funcCall     =*/ _sysfunc_ellipse,
    /*._category     =*/ _defFuncCtg,
    /*._prototype    =*/ QString("ellipse (cx, cy, lr, sr[, rad])"),
    /*._description  =*/ QString("Draw an ellipse with center position, the radius in x and y coordinates, and "
                                 "optional the radian of slope. "),
};

static finErrorCode _sysfunc_draw_text_base(finExecFunction *self, finExecEnvironment *env,
                                            finExecMachine *machine, finExecFlowControl *flowctl, bool pinned)
{
    finErrorCode errcode;
    finExecVariable *text, *cx, *cy, *rad, *scale;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    text = finExecVariable::transLinkTarget(env->findVariable("text"));
    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    scale = finExecVariable::transLinkTarget(env->findVariable("sc"));

    if ( text == nullptr || cx == nullptr || cy == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( text->getType() != finExecVariable::TP_STRING ||
         cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC ||
         (rad != nullptr && rad->getType() != finExecVariable::TP_NULL &&
                            rad->getType() != finExecVariable::TP_NUMERIC) ||
         (scale != nullptr && scale->getType() != finExecVariable::TP_NULL &&
                              scale->getType() != finExecVariable::TP_NUMERIC))
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectText *fotext = new finFigureObjectText();
    if ( fotext == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;
    fotext->setIsPinned(pinned);

    fotext->setText(text->getStringValue());
    fotext->setBasePoint(cx->getNumericValue(), cy->getNumericValue());
    if ( rad != nullptr && rad->getType() == finExecVariable::TP_NUMERIC )
        fotext->setRadian(rad->getNumericValue());
    else
        fotext->setRadian(0.0);
    if ( scale != nullptr && scale->getType() == finExecVariable::TP_NUMERIC )
        fotext->setScale(scale->getNumericValue());
    else
        fotext->setScale(1.0);

    errcode = env->getFigureContainer()->appendFigureObject(fotext);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete fotext;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_draw_text(finExecFunction *self, finExecEnvironment *env,
                                       finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_draw_text_base(self, env, machine, flowctl, false);
}

static finErrorCode _sysfunc_draw_pinned_text(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_draw_text_base(self, env, machine, flowctl, true);
}

static finErrorCode _sysfunc_draw_image_base(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl, bool pinned)
{
    finErrorCode errcode;
    finExecVariable *image, *cx, *cy, *rad, *sx, *sy;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    image = finExecVariable::transLinkTarget(env->findVariable("image"));
    cx = finExecVariable::transLinkTarget(env->findVariable("cx"));
    cy = finExecVariable::transLinkTarget(env->findVariable("cy"));
    rad = finExecVariable::transLinkTarget(env->findVariable("rad"));
    sx = finExecVariable::transLinkTarget(env->findVariable("sx"));
    sy = finExecVariable::transLinkTarget(env->findVariable("sy"));

    if ( image == nullptr || cx == nullptr || cy == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( (image->getType() != finExecVariable::TP_STRING &&
          image->getType() != finExecVariable::TP_IMAGE) ||
         cx->getType() != finExecVariable::TP_NUMERIC ||
         cy->getType() != finExecVariable::TP_NUMERIC ||
         (rad != nullptr && rad->getType() != finExecVariable::TP_NULL &&
                            rad->getType() != finExecVariable::TP_NUMERIC) ||
         (sx != nullptr && sx->getType() != finExecVariable::TP_NULL &&
                           sx->getType() != finExecVariable::TP_NUMERIC) ||
         (sy != nullptr && sy->getType() != finExecVariable::TP_NULL &&
                           sy->getType() != finExecVariable::TP_NUMERIC))
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectImage *foimg = new finFigureObjectImage();
    if ( foimg == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;
    foimg->setIsPinned(pinned);

    if ( image->getType() == finExecVariable::TP_STRING ) {
        QImage imginst = QImage(image->getStringValue());
        foimg->setImage(imginst);
    } else {
        foimg->setImage(image->getImageValue());
    }
    foimg->setBasePoint(cx->getNumericValue(), cy->getNumericValue());
    if ( rad != nullptr && rad->getType() == finExecVariable::TP_NUMERIC )
        foimg->setRadian(rad->getNumericValue());
    else
        foimg->setRadian(0.0);
    if ( sx != nullptr && sx->getType() == finExecVariable::TP_NUMERIC )
        foimg->setScaleX(sx->getNumericValue());
    else
        foimg->setScaleX(1.0);
    if ( sy != nullptr && sy->getType() == finExecVariable::TP_NUMERIC )
        foimg->setScaleY(sy->getNumericValue());
    else
        foimg->setScaleY(1.0);

    errcode = env->getFigureContainer()->appendFigureObject(foimg);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete foimg;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_draw_image(finExecFunction *self, finExecEnvironment *env,
                                        finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_draw_image_base(self, env, machine, flowctl, false);
}

static finErrorCode _sysfunc_draw_pinned_image(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl)
{
    return _sysfunc_draw_image_base(self, env, machine, flowctl, true);
}

static finErrorCode _sysfunc_axis(finExecFunction *self, finExecEnvironment *env,
                                  finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    finFigureObjectAxis *foaxis = new finFigureObjectAxis();
    if ( foaxis == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finExecVariable *stepxvar = finExecVariable::transLinkTarget(env->findVariable("sx"));
    if ( stepxvar == nullptr || stepxvar->getType() != finExecVariable::TP_NUMERIC )
        foaxis->setAutoStepX();
    else
        foaxis->setStepX(stepxvar->getNumericValue());

    finExecVariable *stepyvar = finExecVariable::transLinkTarget(env->findVariable("sy"));
    if ( stepyvar == nullptr || stepyvar->getType() != finExecVariable::TP_NUMERIC )
        foaxis->setAutoStepY();
    else
        foaxis->setStepY(stepyvar->getNumericValue());

    finExecVariable *titlexvar = finExecVariable::transLinkTarget(env->findVariable("tx"));
    if ( titlexvar == nullptr || titlexvar->getType() != finExecVariable::TP_STRING )
        foaxis->setTitleX(QString("x"));
    else
        foaxis->setTitleX(titlexvar->getStringValue());

    finExecVariable *titleyvar = finExecVariable::transLinkTarget(env->findVariable("ty"));
    if ( titleyvar == nullptr || titleyvar->getType() != finExecVariable::TP_STRING )
        foaxis->setTitleY(QString("y"));
    else
        foaxis->setTitleY(titleyvar->getStringValue());

    finExecVariable *minxvar = finExecVariable::transLinkTarget(env->findVariable("rx1"));
    finExecVariable *maxxvar = finExecVariable::transLinkTarget(env->findVariable("rx2"));
    if ( minxvar == nullptr || minxvar->getType() != finExecVariable::TP_NUMERIC ||
         maxxvar == nullptr || maxxvar->getType() != finExecVariable::TP_NUMERIC )
        foaxis->setAutoRangeX();
    else
        foaxis->setRangeX(minxvar->getNumericValue(), maxxvar->getNumericValue());

    finExecVariable *minyvar = finExecVariable::transLinkTarget(env->findVariable("ry1"));
    finExecVariable *maxyvar = finExecVariable::transLinkTarget(env->findVariable("ry2"));
    if ( minyvar == nullptr || minyvar->getType() != finExecVariable::TP_NUMERIC ||
         maxyvar == nullptr || maxyvar->getType() != finExecVariable::TP_NUMERIC )
        foaxis->setAutoRangeY();
    else
        foaxis->setRangeY(minyvar->getNumericValue(), maxyvar->getNumericValue());

    finErrorCode errcode = env->getFigureContainer()->appendFigureObject(foaxis);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete foaxis;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode
_sysfunc_line3d(finExecFunction *self, finExecEnvironment *env, finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *x1, *y1, *z1, *x2, *y2, *z2;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    x1 = finExecVariable::transLinkTarget(env->findVariable("x1"));
    y1 = finExecVariable::transLinkTarget(env->findVariable("y1"));
    z1 = finExecVariable::transLinkTarget(env->findVariable("z1"));
    x2 = finExecVariable::transLinkTarget(env->findVariable("x2"));
    y2 = finExecVariable::transLinkTarget(env->findVariable("y2"));
    z2 = finExecVariable::transLinkTarget(env->findVariable("z2"));

    if ( x1 == nullptr || y1 == nullptr || z1 == nullptr || x2 == nullptr || y2 == nullptr || z2 == nullptr )
        return finErrorKits::EC_NOT_FOUND;

    if ( x1->getType() != finExecVariable::TP_NUMERIC ||
         y1->getType() != finExecVariable::TP_NUMERIC ||
         z1->getType() != finExecVariable::TP_NUMERIC ||
         x2->getType() != finExecVariable::TP_NUMERIC ||
         y2->getType() != finExecVariable::TP_NUMERIC ||
         z2->getType() != finExecVariable::TP_NUMERIC )
        return finErrorKits::EC_INVALID_PARAM;

    finFigureObjectLine3D *foline3d = new finFigureObjectLine3D();
    if ( foline3d == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    foline3d->setPoint1(x1->getNumericValue(), y1->getNumericValue(), z1->getNumericValue());
    foline3d->setPoint2(x2->getNumericValue(), y2->getNumericValue(), z2->getNumericValue());

    errcode = env->getFigureContainer()->appendFigureObject(foline3d);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete foline3d;
        return errcode;
    }
    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_named_color(finExecFunction *self, finExecEnvironment *env,
                                         finExecMachine *machine, finExecFlowControl *flowctl)
{
    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finExecVariable *colornamevar = finExecVariable::transLinkTarget(env->findVariable("colorname"));
    if ( colornamevar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( colornamevar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString colorname = colornamevar->getStringValue();
    if ( !QColor::isValidColor(colorname) )
        return finErrorKits::EC_INVALID_PARAM;

    QColor color = QColor(colorname);
    if ( !color.isValid() )
        return finErrorKits::EC_INVALID_PARAM;

    finExecVariable *retvar = new finExecVariable();
    if (retvar == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finErrorCode errcode = retvar->setupColorValue(color);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    retvar->setWriteProtected();
    retvar->clearLeftValue();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(retvar);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_read_fig_config(finExecFunction *self, finExecEnvironment *env,
                                             finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finFigureConfig *figconfig;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    figconfig = env->getFigureContainer()->getFigureConfig();
    if ( figconfig == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    if ( cfgnamevar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    cfgvalue = new finExecVariable();
    if ( cfgvalue == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "dot_size") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getDotSize());
    } else if ( QString::compare(cfgname, "border_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getBorderColor());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "fill_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getFillColor());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "start_arrow_type") == 0 ) {
        finFigureArrowType arwtype = figconfig->getStartArrowType();
        cfgvalue->setType(finExecVariable::TP_STRING);
        cfgvalue->setStringValue(finFigureArrow::getTypeName(arwtype));
    } else if ( QString::compare(cfgname, "start_arrow_size") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getStartArrowSize());
    } else if ( QString::compare(cfgname, "start_arrow_rad") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getStartArrowRadian());
    } else if ( QString::compare(cfgname, "end_arrow_type") == 0 ) {
        finFigureArrowType arwtype = figconfig->getEndArrowType();
        cfgvalue->setType(finExecVariable::TP_STRING);
        cfgvalue->setStringValue(finFigureArrow::getTypeName(arwtype));
    } else if ( QString::compare(cfgname, "end_arrow_size") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getEndArrowSize());
    } else if ( QString::compare(cfgname, "end_arrow_rad") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getEndArrowRadian());
    } else if ( QString::compare(cfgname, "font_name") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_STRING);
        cfgvalue->setStringValue(figconfig->getFontName());
    } else if ( QString::compare(cfgname, "font_size") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(figconfig->getFontPointSize());
    } else if ( QString::compare(cfgname, "font_bold") == 0 ) {
        errcode = cfgvalue->setupBoolValue(figconfig->getFontBold());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "font_italic") == 0 ) {
        errcode = cfgvalue->setupBoolValue(figconfig->getFontItalic());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "font_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(figconfig->getFontColor());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else {
        delete cfgvalue;
        return finErrorKits::EC_INVALID_PARAM;
    }

    cfgvalue->setWriteProtected();
    cfgvalue->clearLeftValue();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(cfgvalue);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_write_fig_config(finExecFunction *self, finExecEnvironment *env,
                                              finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finFigureConfig *figconfig;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    figconfig = env->getFigureContainer()->getFigureConfig();
    if ( figconfig == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    cfgvalue = finExecVariable::transLinkTarget(env->findVariable("value"));
    if ( cfgnamevar == nullptr || cfgvalue == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "dot_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setDotSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "border_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setBorderColor(color);
    } else if ( QString::compare(cfgname, "fill_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFillColor(color);
    } else if ( QString::compare(cfgname, "start_arrow_type") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_STRING )
            return finErrorKits::EC_INVALID_PARAM;
        finFigureArrowType arwtype = finFigureArrow::parseTypeString(cfgvalue->getStringValue());
        figconfig->setStartArrowType(arwtype);
    } else if ( QString::compare(cfgname, "start_arrow_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setStartArrowSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "start_arrow_rad") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setStartArrowRadian(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "end_arrow_type") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_STRING )
            return finErrorKits::EC_INVALID_PARAM;
        finFigureArrowType arwtype = finFigureArrow::parseTypeString(cfgvalue->getStringValue());
        figconfig->setEndArrowType(arwtype);
    } else if ( QString::compare(cfgname, "end_arrow_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setEndArrowSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "end_arrow_rad") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setEndArrowRadian(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "font_name") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_STRING )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setFontName(cfgvalue->getStringValue());
    } else if ( QString::compare(cfgname, "font_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        figconfig->setFontPointSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "font_bold") == 0 ) {
        bool blval;
        errcode = cfgvalue->readBoolValue(&blval);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFontBold(blval);
    } else if ( QString::compare(cfgname, "font_italic") == 0 ) {
        bool blval;
        errcode = cfgvalue->readBoolValue(&blval);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFontItalic(blval);
    } else if ( QString::compare(cfgname, "font_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        figconfig->setFontColor(color);
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_read_graph_config(finExecFunction *self, finExecEnvironment *env,
                                               finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finGraphConfig *graphconfig;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    graphconfig = env->getFigureContainer()->getGraphConfig();
    if ( graphconfig == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    if ( cfgnamevar == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    cfgvalue = new finExecVariable();
    if ( cfgvalue == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "unit_size") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getAxisUnitPixelSize());
    } else if ( QString::compare(cfgname, "background_color") == 0 ) {
        errcode = cfgvalue->setupColorValue(graphconfig->getBackgroundColor());
        if ( finErrorKits::isErrorResult(errcode) ) {
            delete cfgvalue;
            return errcode;
        }
    } else if ( QString::compare(cfgname, "panel_width") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getPanelPixelWidth());
    } else if ( QString::compare(cfgname, "panel_height") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getPanelPixelHeight());
    } else if ( QString::compare(cfgname, "panel_size") == 0 ) {
        QSizeF panelsize = graphconfig->getPanelPixelSize();
        cfgvalue->preallocArrayLength(2);

        finExecVariable *itemvar = cfgvalue->getVariableItemAt(0);
        itemvar->setType(finExecVariable::TP_NUMERIC);
        itemvar->setNumericValue(panelsize.width());

        itemvar = cfgvalue->getVariableItemAt(1);
        itemvar->setType(finExecVariable::TP_NUMERIC);
        itemvar->setNumericValue(panelsize.height());
    } else if ( QString::compare(cfgname, "origin_point_x") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getOriginPixelPointX());
    } else if ( QString::compare(cfgname, "origin_point_y") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getOriginPixelPointY());
    } else if ( QString::compare(cfgname, "origin_point") == 0 ) {
        QPointF panelsize = graphconfig->getOriginPixelPoint();
        cfgvalue->preallocArrayLength(2);

        finExecVariable *itemvar = cfgvalue->getVariableItemAt(0);
        itemvar->setType(finExecVariable::TP_NUMERIC);
        itemvar->setNumericValue(panelsize.x());

        itemvar = cfgvalue->getVariableItemAt(1);
        itemvar->setType(finExecVariable::TP_NUMERIC);
        itemvar->setNumericValue(panelsize.y());
    } else if ( QString::compare(cfgname, "axis_rad_z") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getAxisRadZ());
    } else if ( QString::compare(cfgname, "axis_scale_z") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(graphconfig->getAxisScaleZ());
    } else if ( QString::compare(cfgname, "render_level") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_STRING);
        cfgvalue->setStringValue(finGraphConfig::getRenderHintsName(graphconfig->getRenderHints()));
    } else if ( QString::compare(cfgname, "transform") == 0 ) {
        cfgvalue->setType(finExecVariable::TP_STRING);
        cfgvalue->setStringValue(finGraphTrans::getTransformTypeName(graphconfig->getTransformType()));
    } else if ( QString::compare(cfgname, "rect_trans_zoom_x") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_RECT ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        finGraphTransRect *recttrans = static_cast<finGraphTransRect *>(graphconfig->getTransform());
        if ( recttrans == nullptr ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(recttrans->getAxisZoomX());
    } else if ( QString::compare(cfgname, "rect_trans_zoom_y") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_RECT ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        finGraphTransRect *recttrans = static_cast<finGraphTransRect *>(graphconfig->getTransform());
        if ( recttrans == nullptr ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(recttrans->getAxisZoomY());
    } else if ( QString::compare(cfgname, "affine_trans_act_cnt") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        finGraphTransAffine *affinetrans = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affinetrans == nullptr ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(affinetrans->getActionCount());
    } else if ( QString::compare(cfgname, "affine_trans_act_type") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        finGraphTransAffine *affinetrans = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affinetrans == nullptr ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        int extargcnt = finExecFunction::getExtendArgCount(env);
        if ( extargcnt <= 0 ) {
            int actcnt = affinetrans->getActionCount();
            cfgvalue->preallocArrayLength(actcnt);
            for ( int i = 0; i < actcnt; i++ ) {
                finGraphTransAffine::ActionType action = affinetrans->getActionAt(i)._type;
                QString actname = finGraphTransAffine::getAffineTransActionName(action);
                finExecVariable *subitem = cfgvalue->getVariableItemAt(i);
                subitem->setType(finExecVariable::TP_STRING);
                subitem->setStringValue(actname);
            }
        } else {
            finExecVariable *extarg = finExecFunction::getExtendArgAt(env, 0);
            if ( extarg->getType() != finExecVariable::TP_NUMERIC ) {
                delete cfgvalue;
                return finErrorKits::EC_INVALID_PARAM;
            }
            int idx = static_cast<int>(floor(extarg->getNumericValue()));
            if ( idx < 0 || idx >= affinetrans->getActionCount() ) {
                delete cfgvalue;
                return finErrorKits::EC_INVALID_PARAM;
            }
            finGraphTransAffine::ActionType action = affinetrans->getActionAt(idx)._type;
            QString actname = finGraphTransAffine::getAffineTransActionName(action);
            cfgvalue->setType(finExecVariable::TP_STRING);
            cfgvalue->setStringValue(actname);
        }
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(affinetrans->getActionCount());
    } else if ( QString::compare(cfgname, "affine_trans_act_arg") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        finGraphTransAffine *affinetrans = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affinetrans == nullptr ) {
            delete cfgvalue;
            return finErrorKits::EC_STATE_ERROR;
        }
        int extargcnt = finExecFunction::getExtendArgCount(env);
        if ( extargcnt <= 0 ) {
            int actcnt = affinetrans->getActionCount();
            cfgvalue->preallocArrayLength(actcnt);
            for ( int i = 0; i < actcnt; i++ ) {
                finGraphTransAffine::ActionType action = affinetrans->getActionAt(i)._type;
                finExecVariable *subitem = cfgvalue->getVariableItemAt(i);
                if ( action == finGraphTransAffine::AT_ROTATE ) {
                    subitem->setType(finExecVariable::TP_NUMERIC);
                    subitem->setNumericValue(affinetrans->getActionAt(i)._arg1);
                } else {
                    subitem->preallocArrayLength(2);
                    subitem->getVariableItemAt(0)->setType(finExecVariable::TP_NUMERIC);
                    subitem->getVariableItemAt(0)->setNumericValue(affinetrans->getActionAt(i)._arg1);
                    subitem->getVariableItemAt(1)->setType(finExecVariable::TP_NUMERIC);
                    subitem->getVariableItemAt(1)->setNumericValue(affinetrans->getActionAt(i)._arg2);
                }
            }
        } else {
            finExecVariable *extarg = finExecVariable::transLinkTarget(finExecFunction::getExtendArgAt(env, 0));
            if ( extarg->getType() != finExecVariable::TP_NUMERIC ) {
                delete cfgvalue;
                return finErrorKits::EC_INVALID_PARAM;
            }
            int idx = static_cast<int>(floor(extarg->getNumericValue()));
            if ( idx < 0 || idx >= affinetrans->getActionCount() ) {
                delete cfgvalue;
                return finErrorKits::EC_INVALID_PARAM;
            }
            finGraphTransAffine::ActionType action = affinetrans->getActionAt(idx)._type;
            finExecVariable *subitem = cfgvalue->getVariableItemAt(idx);
            if ( action == finGraphTransAffine::AT_ROTATE ) {
                subitem->setType(finExecVariable::TP_NUMERIC);
                subitem->setNumericValue(affinetrans->getActionAt(idx)._arg1);
            } else {
                subitem->preallocArrayLength(2);
                subitem->getVariableItemAt(0)->setType(finExecVariable::TP_NUMERIC);
                subitem->getVariableItemAt(0)->setNumericValue(affinetrans->getActionAt(idx)._arg1);
                subitem->getVariableItemAt(1)->setType(finExecVariable::TP_NUMERIC);
                subitem->getVariableItemAt(1)->setNumericValue(affinetrans->getActionAt(idx)._arg2);
            }
        }
        cfgvalue->setType(finExecVariable::TP_NUMERIC);
        cfgvalue->setNumericValue(affinetrans->getActionCount());
    } else {
        delete cfgvalue;
        return finErrorKits::EC_INVALID_PARAM;
    }

    cfgvalue->setWriteProtected();
    cfgvalue->clearLeftValue();
    flowctl->setFlowNext();
    flowctl->setReturnVariable(cfgvalue);
    return finErrorKits::EC_SUCCESS;
}

static finErrorCode _sysfunc_write_graph_config(finExecFunction *self, finExecEnvironment *env,
                                                finExecMachine *machine, finExecFlowControl *flowctl)
{
    finErrorCode errcode;
    finExecVariable *cfgnamevar, *cfgvalue;
    finGraphConfig *graphconfig;

    if ( self == nullptr || env == nullptr || machine == nullptr || flowctl == nullptr )
        return finErrorKits::EC_NULL_POINTER;
    if ( env->getFigureContainer() == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    graphconfig = env->getFigureContainer()->getGraphConfig();
    if ( graphconfig == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    cfgnamevar = finExecVariable::transLinkTarget(env->findVariable("cfgname"));
    cfgvalue = finExecVariable::transLinkTarget(env->findVariable("value"));
    if ( cfgnamevar == nullptr || cfgvalue == nullptr )
        return finErrorKits::EC_NOT_FOUND;
    if ( cfgnamevar->getType() != finExecVariable::TP_STRING )
        return finErrorKits::EC_INVALID_PARAM;

    QString cfgname = cfgnamevar->getStringValue();
    if ( QString::compare(cfgname, "unit_size") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setAxisUnitPixelSize(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "background_color") == 0 ) {
        QColor color;
        errcode = cfgvalue->readColorValue(&color);
        if ( finErrorKits::isErrorResult(errcode) )
            return errcode;
        graphconfig->setBackgroundColor(color);
    } else if ( QString::compare(cfgname, "panel_width") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setPanelPixelWidth(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "panel_height") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setPanelPixelHeight(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "panel_size") == 0 ) {
        int arylen = 0;
        if ( !cfgvalue->isNumericArray(&arylen) )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setPanelPixelSize(cfgvalue->getVariableItemAt(0)->getNumericValue(),
                                       cfgvalue->getVariableItemAt(1)->getNumericValue());
    } else if ( QString::compare(cfgname, "origin_point_x") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setOriginPixelPointX(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "origin_point_y") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setOriginPixelPointY(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "origin_point") == 0 ) {
        int arylen = 0;
        if ( !cfgvalue->isNumericArray(&arylen) )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setOriginPixelPoint(cfgvalue->getVariableItemAt(0)->getNumericValue(),
                                         cfgvalue->getVariableItemAt(1)->getNumericValue());
    } else if ( QString::compare(cfgname, "axis_rad_z") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setAxisRadZ(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "axis_scale_z") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        graphconfig->setAxisScaleZ(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "render_level") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_STRING )
            return finErrorKits::EC_INVALID_PARAM;
        QPainter::RenderHints hints = finGraphConfig::parseRenderHints(cfgvalue->getStringValue());
        graphconfig->setRenderHints(hints);
    } else if ( QString::compare(cfgname, "transform") == 0 ) {
        if ( cfgvalue->getType() != finExecVariable::TP_STRING )
            return finErrorKits::EC_INVALID_PARAM;
        finGraphTransType type = finGraphTrans::parseTransformType(cfgvalue->getStringValue());
        graphconfig->setTransformType(type);
    } else if ( QString::compare(cfgname, "rect_trans_zoom_x") == 0 ) {
        if ( cfgvalue->getType() !=  finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        if ( graphconfig->getTransformType() != finGraphTrans::TP_RECT )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransRect *recttrans = static_cast<finGraphTransRect *>(graphconfig->getTransform());
        if ( recttrans == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        recttrans->setAxisZoomX(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "rect_trans_zoom_y") == 0 ) {
        if ( cfgvalue->getType() !=  finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        if ( graphconfig->getTransformType() != finGraphTrans::TP_RECT )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransRect *recttrans = static_cast<finGraphTransRect *>(graphconfig->getTransform());
        if ( recttrans == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        recttrans->setAxisZoomY(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "affine_trans_clear_act") == 0 ) {
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransAffine *affine = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affine == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        affine->reset();
    } else if ( QString::compare(cfgname, "affine_trans_append_rotate") == 0 ) {
        if ( cfgvalue->getType() !=  finExecVariable::TP_NUMERIC )
            return finErrorKits::EC_INVALID_PARAM;
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransAffine *affine = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affine == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        affine->appendRotate(cfgvalue->getNumericValue());
    } else if ( QString::compare(cfgname, "affine_trans_append_scale") == 0 ) {
        int arylen = 0;
        if ( !cfgvalue->isNumericArray(&arylen) )
            return finErrorKits::EC_INVALID_PARAM;
        if ( arylen < 2 )
            return finErrorKits::EC_INVALID_PARAM;
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransAffine *affine = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affine == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        affine->appendScale(cfgvalue->getVariableItemAt(0)->getNumericValue(),
                            cfgvalue->getVariableItemAt(1)->getNumericValue());
    } else if ( QString::compare(cfgname, "affine_trans_append_translate") == 0 ) {
        int arylen = 0;
        if ( !cfgvalue->isNumericArray(&arylen) )
            return finErrorKits::EC_INVALID_PARAM;
        if ( arylen < 2 )
            return finErrorKits::EC_INVALID_PARAM;
        if ( graphconfig->getTransformType() != finGraphTrans::TP_AFFINE )
            return finErrorKits::EC_STATE_ERROR;
        finGraphTransAffine *affine = static_cast<finGraphTransAffine *>(graphconfig->getTransform());
        if ( affine == nullptr )
            return finErrorKits::EC_STATE_ERROR;
        affine->appendTranslate(cfgvalue->getVariableItemAt(0)->getNumericValue(),
                                cfgvalue->getVariableItemAt(1)->getNumericValue());
    } else {
        return finErrorKits::EC_INVALID_PARAM;
    }

    flowctl->setFlowNext();
    return finErrorKits::EC_SUCCESS;
}

static finExecSysFuncRegItem _finSysFuncFigureList[] = {
    _funcRegItem_clear_fig,
    _funcRegItem_draw_dot,
    _funcRegItem_line,
    _funcRegItem_polyline,
    _funcRegItem_polyline_mat,
    _funcRegItem_rect,
    _funcRegItem_polygon,
    _funcRegItem_polygon_mat,
    _funcRegItem_circle,
    _funcRegItem_ellipse,
    { QString("draw_text"),          QString("text,cx,cy,rad,sc"),           _sysfunc_draw_text          },
    { QString("draw_pinned_text"),   QString("text,cx,cy,rad,sc"),           _sysfunc_draw_pinned_text   },
    { QString("draw_image"),         QString("image,cx,cy,rad,sx,sy"),       _sysfunc_draw_image         },
    { QString("draw_pinned_image"),  QString("image,cx,cy,rad,sx,sy"),       _sysfunc_draw_pinned_image  },
    { QString("axis"),               QString("sx,sy,tx,ty,rx1,rx2,ry1,ry2"), _sysfunc_axis               },

    { QString("line3d"),             QString("x1,y1,z1,x2,y2,z2"),           _sysfunc_line3d             },

    { QString("named_color"),        QString("colorname"),                   _sysfunc_named_color        },
    { QString("read_fig_config"),    QString("cfgname"),                     _sysfunc_read_fig_config    },
    { QString("write_fig_config"),   QString("cfgname,value"),               _sysfunc_write_fig_config   },
    { QString("read_graph_config"),  QString("cfgname"),                     _sysfunc_read_graph_config  },
    { QString("write_graph_config"), QString("cfgname,value"),               _sysfunc_write_graph_config },

    { QString(), QString(), nullptr, _defFuncCtg, QString(), QString() }
};

finErrorCode finExecFunction::registSysFuncFiguring()
{
    return finExecFunction::registSysFuncFromArray(_finSysFuncFigureList, QString("Figuring"));
}

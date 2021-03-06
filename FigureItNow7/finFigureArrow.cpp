/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finFigureArrow.h"

#include <qmath.h>
#include <QVariant>

#include "finFigureConfig.h"
#include "finGraphConfig.h"
#include "finFigureAlg.h"

QString finFigureArrow::getTypeName(finFigureArrowType type)
{
    switch ( type ) {
      case finFigureArrow::TP_NONE:
        return QString("none");
        break;

      case finFigureArrow::TP_TRIANGLE:
        return QString("triangle");
        break;
    }
    return QString();
}

finFigureArrowType finFigureArrow::parseTypeString(const QString &str)
{
    if ( QString::compare(str, QString("none")) == 0 )
        return finFigureArrow::TP_NONE;
    else if ( QString::compare(str, QString("triangle")) == 0 )
        return finFigureArrow::TP_TRIANGLE;
    else
        return finFigureArrow::TP_NONE;
}

finErrorCode finFigureArrow::fillTypesInComboBox(QComboBox *cmbox)
{
    if ( cmbox == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    cmbox->setEditable(false);
    cmbox->addItem("none", QVariant("none"));
    cmbox->addItem("triangle", QVariant("triangle"));

    return finErrorKits::EC_SUCCESS;
}

finFigureArrow::finFigureArrow()
{
    this->_type = finFigureArrow::TP_NONE;
    this->_rad = M_PI / 4.0;
    this->_length = 12.0;
}

finFigureArrow::finFigureArrow(const finFigureArrow &arrow)
{
    this->_type = arrow._type;
    this->_rad = arrow._rad;
    this->_length = arrow._length;
}

finFigureArrowType finFigureArrow::getType() const
{
    return this->_type;
}

double finFigureArrow::getRadian() const
{
    if ( this->_type == finFigureArrow::TP_NONE )
        return 0.0;

    return this->_rad;
}

double finFigureArrow::getLength() const
{
    if ( this->_type == finFigureArrow::TP_NONE )
        return 0.0;

    return this->_length;
}

finErrorCode finFigureArrow::setType(finFigureArrowType type)
{
    this->_type = type;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureArrow::setRadian(double rad)
{
    rad = rad - floor(rad / M_PI) * M_PI;
    this->_rad = rad;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finFigureArrow::setLength(double length)
{
    if ( length < 0.0 )
        return finErrorKits::EC_INVALID_PARAM;

    this->_length = length;
    return finErrorKits::EC_SUCCESS;
}

finFigureArrow &finFigureArrow::operator = (const finFigureArrow &arrow)
{
    this->_type = arrow._type;
    this->_rad = arrow._rad;
    this->_length = arrow._length;
    return *this;
}

QPen finFigureArrow::getArrowPen(const finFigureConfig *cfg) const
{
    QPen pen = cfg->getBorderPen();
    switch ( pen.capStyle() ) {
      case Qt::FlatCap:
        pen.setColor(QColor(Qt::transparent));
        break;

      case Qt::SquareCap:
        pen.setJoinStyle(Qt::MiterJoin);
        break;

      case Qt::RoundCap:
        pen.setJoinStyle(Qt::RoundJoin);
        break;

      default:
        break;
    }
    return pen;
}

QBrush finFigureArrow::getArrowBrush(const finFigureConfig *cfg) const
{
    return cfg->getBorderPen().brush();
}

QPointF
finFigureArrow::lineShrinkPoint(const QPointF &arwpt, const QPointF &prevpt, const finFigureConfig *cfg) const
{
    if ( cfg == nullptr )
        return arwpt;

    switch ( this->_type ) {
      case finFigureArrow::TP_NONE:
        return arwpt;
        break;

      case finFigureArrow::TP_TRIANGLE:
        return this->lineShrinkPtTriangle(prevpt, arwpt, cfg);
        break;

      default:
        return arwpt;
        break;
    }
}

finErrorCode finFigureArrow::getPixelPath(QList<finFigurePath> *pathlist,
                                          const QPointF &arwpt, const QPointF &prevpt,
                                          const finFigureConfig *cfg) const
{
    if ( pathlist == nullptr || cfg == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    switch ( this->_type ) {
      case finFigureArrow::TP_NONE:
        return finErrorKits::EC_SUCCESS;
        break;

      case finFigureArrow::TP_TRIANGLE:
        return this->getPixelPathTriangle(pathlist, arwpt, prevpt, cfg);
        break;

      default:
        return finErrorKits::EC_NON_IMPLEMENT;
        break;
    }
}

double finFigureArrow::lineShrinkLenTriangle(const finFigureConfig *cfg) const
{
    double w2 = cfg->getBorderPen().widthF() / 2.0;
    double al2 = this->_rad / 2.0;
    double l = w2 / tan(al2);
    if ( l > this->_length )
        l = this->_length;
    return l;
}

QPointF
finFigureArrow::lineShrinkPtTriangle(const QPointF &prevpt, const QPointF &arwpt, const finFigureConfig *cfg) const
{
    double linelen = finFigureAlg::pointsDistance(prevpt, arwpt);
    double shrinklen = this->lineShrinkLenTriangle(cfg);
    if ( shrinklen >= linelen )
        return prevpt;

    return finFigureAlg::movePointInside(arwpt, prevpt, shrinklen);
}

finErrorCode finFigureArrow::getPixelPathTriangle(QList<finFigurePath> *pathlist,
                                                  const QPointF &arwpt, const QPointF &prevpt,
                                                  const finFigureConfig *cfg) const
{
    QPointF trft = finFigureAlg::movePointInside(arwpt, prevpt, this->_length);
    double trftlen = this->_length * tan(this->_rad / 2.0);
    QPointF trhvec = arwpt - trft;
    QPointF trftvec = finFigureAlg::getVerticalVector(trhvec, trftlen);

    QPointF trpt1 = trft + trftvec;
    QPointF trpt2 = trft - trftvec;
    QPainterPath path;
    path.moveTo(arwpt);
    path.lineTo(trpt1);
    path.lineTo(trpt2);
    path.lineTo(arwpt);

    finFigurePath figpath;
    figpath.setPen(this->getArrowPen(cfg));
    figpath.setBrush(this->getArrowBrush(cfg));
    figpath.setPath(path);
    pathlist->append(figpath);

    return finErrorKits::EC_SUCCESS;
}

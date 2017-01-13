#ifndef FINUIFIGUREWIDGET_H
#define FINUIFIGUREWIDGET_H

#include <QWidget>

#include "finErrorCode.h"
#include "finFigureContainer.h"


class finUiFigureWidget : public QWidget
{
protected:
    finFigureContainer _figcontainer;

public:
    finUiFigureWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    const finFigureContainer *getFigureContainer() const;
    finFigureContainer *getFigureContainer();

protected:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // FINUIFIGUREWIDGET_H

#include "finUiColorLabel.h"

#include <QPalette>
#include <QMouseEvent>
#include <QColorDialog>

finUiColorLabel::finUiColorLabel(QWidget *parent) :
    QLabel(parent), _color()
{
    this->setFrameShape(QFrame::Box);
}

finUiColorLabel::~finUiColorLabel()
{
}

const QColor &finUiColorLabel::color() const
{
    return this->_color;
}

void finUiColorLabel::setColor(const QColor &color)
{
    this->_color = color;

    QPalette palette = this->palette();
    palette.setColor(QPalette::Active, QPalette::Base, color);
    palette.setColor(QPalette::Disabled, QPalette::Base, color);
    palette.setColor(QPalette::Inactive, QPalette::Base, color);

    this->setPalette(palette);
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
    this->setText(color.name(QColor::HexArgb));

    emit this->colorChanged(this->_color);
}

void finUiColorLabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton) {
        QColorDialog colordlg;
        colordlg.setOptions(QColorDialog::ShowAlphaChannel);
        colordlg.setCurrentColor(this->_color);
        colordlg.exec();

        if ( colordlg.result() != QDialog::Accepted )
            return;

        this->setColor(colordlg.currentColor());
        ev->accept();
    }
}

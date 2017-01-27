#include "finUiColorLabel.h"

#include <QPalette>
#include <QMouseEvent>
#include <QColorDialog>

finUiColorLabel::finUiColorLabel(QWidget *parent) :
    QLabel(parent), _color()
{
    this->setFrameShape(QFrame::Box);
    this->setAlignment(Qt::AlignCenter);
    this->setColor(this->_color);
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
    if ( color.alphaF() > 0.75 && color.lightnessF() < 0.4 ) {
        palette.setColor(QPalette::Active, QPalette::Text, QColor(Qt::white));
        palette.setColor(QPalette::Disabled, QPalette::Text,  QColor(Qt::lightGray));
        palette.setColor(QPalette::Inactive, QPalette::Text, QColor(Qt::lightGray));
    } else {
        palette.setColor(QPalette::Active, QPalette::Text, QColor(Qt::black));
        palette.setColor(QPalette::Disabled, QPalette::Text,  QColor(Qt::darkGray));
        palette.setColor(QPalette::Inactive, QPalette::Text, QColor(Qt::darkGray));
    }

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

#ifndef FINUICOLORLABEL_H
#define FINUICOLORLABEL_H

#include <QLabel>
#include <QColor>


class finUiColorLabel : public QLabel
{
    Q_OBJECT

protected:
    QColor _color;

public:
    explicit finUiColorLabel(QWidget *parent = 0);
    ~finUiColorLabel();

    const QColor &color() const;
    void setColor(const QColor &color);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *ev);

signals:
    void colorChanged(const QColor &color);
};

#endif // FINUICOLORLABEL_H

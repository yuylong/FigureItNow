#ifndef FINUISYSFUNCLIST_H
#define FINUISYSFUNCLIST_H

#include <QWidget>

namespace Ui {
class finUiSysFuncList;
}

class finUiSysFuncList : public QWidget
{
    Q_OBJECT

public:
    explicit finUiSysFuncList(QWidget *parent = 0);
    ~finUiSysFuncList();

private:
    Ui::finUiSysFuncList *ui;
};

#endif // FINUISYSFUNCLIST_H

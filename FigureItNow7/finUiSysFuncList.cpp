#include "finUiSysFuncList.h"
#include "ui_finUiSysFuncList.h"

finUiSysFuncList::finUiSysFuncList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::finUiSysFuncList)
{
    ui->setupUi(this);
}

finUiSysFuncList::~finUiSysFuncList()
{
    delete ui;
}

#include "finUiGraphConfigDlg.h"
#include "ui_finUiGraphConfigDlg.h"

finUiGraphConfigDlg::finUiGraphConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiGraphConfigDlg)
{
    ui->setupUi(this);
}

finUiGraphConfigDlg::~finUiGraphConfigDlg()
{
    delete ui;
}

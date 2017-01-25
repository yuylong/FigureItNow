#include "finUiFigConfigDlg.h"
#include "ui_finUiFigConfigDlg.h"

finUiFigConfigDlg::finUiFigConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiFigConfigDlg)
{
    ui->setupUi(this);
}

finUiFigConfigDlg::~finUiFigConfigDlg()
{
    delete ui;
}

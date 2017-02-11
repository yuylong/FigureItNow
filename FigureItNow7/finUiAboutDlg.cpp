#include "finUiAboutDlg.h"
#include "ui_finUiAboutDlg.h"

finUiAboutDlg::finUiAboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiAboutDlg)
{
    ui->setupUi(this);
}

finUiAboutDlg::~finUiAboutDlg()
{
    delete ui;
}

#include "finUiEdtrFontConfigDlg.h"
#include "ui_finUiEdtrFontConfigDlg.h"

finUiEdtrFontConfigDlg::finUiEdtrFontConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiEdtrFontConfigDlg)
{
    ui->setupUi(this);
}

finUiEdtrFontConfigDlg::~finUiEdtrFontConfigDlg()
{
    delete ui;
}

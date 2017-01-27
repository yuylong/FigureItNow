#ifndef FINUIGRAPHCONFIGDLG_H
#define FINUIGRAPHCONFIGDLG_H

#include <QDialog>

namespace Ui {
class finUiGraphConfigDlg;
}

class finUiGraphConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit finUiGraphConfigDlg(QWidget *parent = 0);
    ~finUiGraphConfigDlg();

private:
    Ui::finUiGraphConfigDlg *ui;
};

#endif // FINUIGRAPHCONFIGDLG_H

#ifndef FINUIFIGCONFIGDLG_H
#define FINUIFIGCONFIGDLG_H

#include <QDialog>

namespace Ui {
class finUiFigConfigDlg;
}

class finUiFigConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit finUiFigConfigDlg(QWidget *parent = 0);
    ~finUiFigConfigDlg();

private:
    Ui::finUiFigConfigDlg *ui;
};

#endif // FINUIFIGCONFIGDLG_H

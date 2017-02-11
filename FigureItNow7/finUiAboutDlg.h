#ifndef FINUIABOUTDLG_H
#define FINUIABOUTDLG_H

#include <QDialog>

namespace Ui {
class finUiAboutDlg;
}

class finUiAboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit finUiAboutDlg(QWidget *parent = 0);
    ~finUiAboutDlg();

private:
    Ui::finUiAboutDlg *ui;
};

#endif // FINUIABOUTDLG_H

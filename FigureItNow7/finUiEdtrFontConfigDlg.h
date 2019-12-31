#ifndef FINUIEDTRFONTCONFIGDLG_H
#define FINUIEDTRFONTCONFIGDLG_H

#include <QDialog>

namespace Ui {
class finUiEdtrFontConfigDlg;
}

class finUiEdtrFontConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit finUiEdtrFontConfigDlg(QWidget *parent = nullptr);
    ~finUiEdtrFontConfigDlg();

private:
    Ui::finUiEdtrFontConfigDlg *ui;
};

#endif // FINUIEDTRFONTCONFIGDLG_H

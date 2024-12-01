/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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

    void renderVersionInfo();
};

#endif // FINUIABOUTDLG_H

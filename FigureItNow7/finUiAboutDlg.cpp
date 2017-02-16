/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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

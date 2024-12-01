/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "finUiAboutDlg.h"
#include "ui_finUiAboutDlg.h"

#include <QtVersion>

#include "finVersion.h"


finUiAboutDlg::finUiAboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::finUiAboutDlg)
{
    ui->setupUi(this);

    renderVersionInfo();
}

finUiAboutDlg::~finUiAboutDlg()
{
    delete ui;
}

void finUiAboutDlg::renderVersionInfo()
{
    QString renderStr = ui->lblCopyrights->text();

    renderStr.replace("[:version:]", finVersionTools::currentVersionString())
        .replace("[:branch:]", finVersionTools::currentGitBranch())
        .replace("[:gitver:]", finVersionTools::currentGitVersion())
        .replace("[:gittime:]", finVersionTools::currentGitDate())
        .replace("[:qtver:]", qVersion())
        .replace("[:qtbuildver:]", QT_VERSION_STR);

    ui->lblCopyrights->setText(renderStr);
}

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <qmath.h>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>

#include "finExecEnvironment.h"
#include "finGraphPanelScene.h"
#include "finUiFigConfigDlg.h"
#include "finUiGraphConfigDlg.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

finUiScriptEditor *MainWindow::getCurrentEditor() const
{
    if ( ui->tbwDocumentList->currentWidget() == NULL )
        return NULL;

    return ((finUiScriptEditor *)ui->tbwDocumentList->currentWidget());
}

void MainWindow::on_actDraw_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->drawOnPanel();
}

void MainWindow::on_actOpen_triggered()
{
    QFileDialog filedlg(this, QString("Open a Script File"));
    filedlg.setAcceptMode(QFileDialog::AcceptOpen);
    filedlg.setFileMode(QFileDialog::ExistingFile);

    filedlg.exec();
    if( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() )
        return;

    finUiScriptEditor *neweditor = new finUiScriptEditor();
    finErrorCode errcode = neweditor->openFile(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qDebug() << filepaths.first() << " ERROR: cannot open! " << endl;
        delete neweditor;
        return;
    }

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
}

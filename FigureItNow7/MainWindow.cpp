#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <qmath.h>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

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

finUiScriptEditor *MainWindow::getEditorAt(int idx) const
{
    if ( idx < 0 || idx >= ui->tbwDocumentList->count() )
        return NULL;

    return ((finUiScriptEditor *)ui->tbwDocumentList->widget(idx));
}

finErrorCode MainWindow::removeEditorAt(int idx)
{
    if ( idx < 0 || idx >= ui->tbwDocumentList->count() )
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    finUiScriptEditor *editor = this->getEditorAt(idx);
    ui->tbwDocumentList->removeTab(idx);

    if ( editor != NULL )
        delete editor;
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode MainWindow::openScriptFile(const QString &filepath)
{
    finUiScriptEditor *neweditor = new finUiScriptEditor();
    if ( neweditor == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    finErrorCode errcode = neweditor->openFile(filepath);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        delete neweditor;
        return errcode;
    }

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode MainWindow::createNewScriptFile()
{
    finUiScriptEditor *neweditor = new finUiScriptEditor();
    if ( neweditor == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

    finErrorCode errcode = this->openScriptFile(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox msgbox(this);
        msgbox.setText(QString("ERROR: cannot open the file!"));
        msgbox.setIcon(QMessageBox::Critical);
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.exec();
        return;
    }
}

void MainWindow::on_actClose_triggered()
{
    this->removeEditorAt(ui->tbwDocumentList->currentIndex());
}

void MainWindow::on_actWiki_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://github.com/yuylong/FigureItNow/wiki")));
}

void MainWindow::on_tbwDocumentList_tabCloseRequested(int index)
{
    this->removeEditorAt(index);
}

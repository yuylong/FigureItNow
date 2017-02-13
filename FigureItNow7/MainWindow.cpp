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
#include "finUiAboutDlg.h"


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

    return qobject_cast<finUiScriptEditor *>(ui->tbwDocumentList->currentWidget());
}

finUiScriptEditor *MainWindow::getEditorAt(int idx) const
{
    if ( idx < 0 || idx >= ui->tbwDocumentList->count() )
        return NULL;

    return qobject_cast<finUiScriptEditor *>(ui->tbwDocumentList->widget(idx));
}

int MainWindow::findEditorIndex(finUiScriptEditor *editor) const
{
    for ( int i = 0; i < ui->tbwDocumentList->count(); i++ ) {
        finUiScriptEditor *curedit = this->getEditorAt(i);
        if ( curedit == editor )
            return i;
    }
    return -1;
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

    QObject::connect(neweditor, SIGNAL(scriptModificationChanged(bool)),
                     this, SLOT(scriptEditor_scriptModificationChanged(bool)));
    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode MainWindow::createNewScriptFile()
{
    finUiScriptEditor *neweditor = new finUiScriptEditor();
    if ( neweditor == NULL )
        return finErrorCodeKits::FIN_EC_OUT_OF_MEMORY;

    QObject::connect(neweditor, SIGNAL(scriptModificationChanged(bool)),
                     this, SLOT(scriptEditor_scriptModificationChanged(bool)));
    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void MainWindow::on_actNew_triggered()
{
    finErrorCode errcode = this->createNewScriptFile();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be created!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actOpen_triggered()
{
    QFileDialog filedlg(this, QString("Open a Script File"));
    filedlg.setAcceptMode(QFileDialog::AcceptOpen);
    filedlg.setFileMode(QFileDialog::ExistingFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() )
        return;

    finErrorCode errcode = this->openScriptFile(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be open!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actSave_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    if ( !cureditor->isFileOpened() )
        return this->on_actSaveAs_triggered();

    if ( !cureditor->isScriptModified() )
        return;

    finErrorCode errcode = cureditor->saveFile();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be saved on disk!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actSaveAs_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    QFileDialog filedlg(this, QString("Open a Script File"));
    filedlg.setAcceptMode(QFileDialog::AcceptSave);
    filedlg.setFileMode(QFileDialog::AnyFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() ) {
        QMessageBox::warning(this, QString("Warning"),
                             QString("You need to select a file to save."), QMessageBox::Ok);
        return;
    }

    finErrorCode errcode = cureditor->saveAsFile(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be saved on disk!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actClose_triggered()
{
    this->removeEditorAt(ui->tbwDocumentList->currentIndex());
}

void MainWindow::on_actUndo_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->scriptUndo();
}

void MainWindow::on_actRedo_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->scriptRedo();
}

void MainWindow::on_actCut_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->cutScript();
}

void MainWindow::on_actCopy_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->copyScript();
}

void MainWindow::on_actPaste_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->pasteScript();
}

void MainWindow::on_actDraw_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->drawOnPanel();
}

void MainWindow::on_actManual_triggered()
{
    QDesktopServices::openUrl(
                QUrl(QString("https://github.com/yuylong/FigureItNow/wiki/FigureItNow-Script-Language")));
}

void MainWindow::on_actWiki_triggered()
{
    QDesktopServices::openUrl(QUrl(QString("https://github.com/yuylong/FigureItNow/wiki")));
}

void MainWindow::on_actAbout_triggered()
{
    finUiAboutDlg aboutdlg(this);
    aboutdlg.exec();
}

void MainWindow::scriptEditor_scriptModificationChanged(bool)
{
    finUiScriptEditor *cureditor = qobject_cast<finUiScriptEditor *>(sender());
    if ( cureditor == NULL )
        return;

    int tabidx = this->findEditorIndex(cureditor);
    if ( tabidx < 0 || tabidx >= ui->tbwDocumentList->count())
        return;

    ui->tbwDocumentList->setTabText(tabidx, cureditor->getTabTitle());
    if ( cureditor == this->getCurrentEditor() ) {
        this->setWindowTitle(cureditor->getWindowTitle() + QString(" - FigureItNow"));
    }
}

void MainWindow::on_tbwDocumentList_tabCloseRequested(int index)
{
    this->removeEditorAt(index);
}

void MainWindow::on_tbwDocumentList_currentChanged(int)
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL ) {
        this->setWindowTitle("FigureItNow");
    } else {
        this->setWindowTitle(cureditor->getWindowTitle() + QString(" - FigureItNow"));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

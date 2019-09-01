/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <qmath.h>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QPrinter>
#include <QPrintDialog>

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

    this->_fileDirPath = QDir::homePath();
    this->refreshSysFuncList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

finUiScriptEditor *MainWindow::getCurrentEditor() const
{
    if ( ui->tbwDocumentList->currentWidget() == nullptr )
        return nullptr;

    return qobject_cast<finUiScriptEditor *>(ui->tbwDocumentList->currentWidget());
}

finUiScriptEditor *MainWindow::getEditorAt(int idx) const
{
    if ( idx < 0 || idx >= ui->tbwDocumentList->count() )
        return nullptr;

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

finErrorCode MainWindow::openScriptFile(const QString &filepath)
{
    finUiScriptEditor *neweditor = new finUiScriptEditor();
    if ( neweditor == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    finErrorCode errcode = neweditor->openFile(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        delete neweditor;
        return errcode;
    }

    neweditor->applyFigureConfig(&this->_defFigConfig);
    neweditor->applyGraphConfig(&this->_defGraphConfig);
    QObject::connect(neweditor, SIGNAL(scriptModificationChanged(bool)),
                     this, SLOT(scriptEditor_scriptModificationChanged(bool)));

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode MainWindow::createNewScriptFile()
{
    finUiScriptEditor *neweditor = new finUiScriptEditor();
    if ( neweditor == nullptr )
        return finErrorKits::EC_OUT_OF_MEMORY;

    neweditor->applyFigureConfig(&this->_defFigConfig);
    neweditor->applyGraphConfig(&this->_defGraphConfig);
    QObject::connect(neweditor, SIGNAL(scriptModificationChanged(bool)),
                     this, SLOT(scriptEditor_scriptModificationChanged(bool)));

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorKits::EC_SUCCESS;
}

bool MainWindow::saveScriptFile(finUiScriptEditor *editor)
{
    if ( editor == nullptr )
        return true;

    if ( !editor->isFileOpened() )
        return this->saveAsScriptFile(editor);

    if ( !editor->isScriptModified() )
        return true;

    finErrorCode errcode = editor->saveFile();
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be saved on disk!"), QMessageBox::Ok);
        return true;
    }
    return true;
}

bool MainWindow::saveAsScriptFile(finUiScriptEditor *editor)
{
    if ( editor == nullptr )
        return true;

    QString defpath = this->_fileDirPath;
    if ( editor->isFileOpened() )
        defpath = editor->getFilePath();
    QString filepath = QFileDialog::getSaveFileName(this, QString("Save Script File"),
                                                    defpath,
                                                    QString("FigureItNow Script (*.fsc)"));
    if ( filepath.isEmpty() )
        return false;

    this->_fileDirPath = filepath;
    finErrorCode errcode = editor->saveAsFile(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be saved on disk!"), QMessageBox::Ok);
        return true;
    }
    return true;
}

QString MainWindow::getSaveFileQuestionString(finUiScriptEditor *editor) const
{
    QString str;
    QTextStream out(&str, QIODevice::WriteOnly);

    out << editor->getFileDisplayPath() << QString(" is not saved.") << endl;
    out << "Save it?" << endl;
    return str;
}

finErrorCode MainWindow::removeEditorAt(int idx)
{
    if ( idx < 0 || idx >= ui->tbwDocumentList->count() )
        return finErrorKits::EC_NOT_FOUND;

    finUiScriptEditor *editor = this->getEditorAt(idx);
    bool saveres = true;
    if ( editor == nullptr ) {
        ui->tbwDocumentList->removeTab(idx);
        return finErrorKits::EC_SUCCESS;
    }

    QMessageBox::StandardButton resbtn = QMessageBox::Cancel;
    if ( !editor->isScriptModified() )
        goto out;

    resbtn = QMessageBox::question(this, QString("Question"), this->getSaveFileQuestionString(editor),
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                   QMessageBox::Yes);
    switch ( resbtn ) {
      case QMessageBox::Yes:
        saveres = this->saveScriptFile(editor);
        if ( !saveres )
            return finErrorKits::EC_UNREADY_WARN;
        break;

      case QMessageBox::No:
        goto out;
        //break;

      case QMessageBox::Cancel:
      default:
        return finErrorKits::EC_UNREADY_WARN;
        //break;
    }

out:
    ui->tbwDocumentList->removeTab(idx);
    delete editor;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode MainWindow::refreshSysFuncList()
{
    ui->dwcSysFuncList->clearList();

    finExecEnvironment *rootenv = finExecEnvironment::getRootEnvironment();
    if ( rootenv == nullptr )
        return finErrorKits::EC_STATE_ERROR;

    return ui->dwcSysFuncList->installFunctionList();
}

void MainWindow::on_actNew_triggered()
{
    finErrorCode errcode = this->createNewScriptFile();
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be created!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actOpen_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, QString("Open a Script File"),
                                                    this->_fileDirPath,
                                                    QString("FigureItNow Script (*.fsc)"));
    if ( filepath.isEmpty() )
        return;

    this->_fileDirPath = filepath;
    finErrorCode errcode = this->openScriptFile(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be open!"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actSave_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    this->saveScriptFile(cureditor);
}

void MainWindow::on_actSaveAs_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    this->saveAsScriptFile(cureditor);
}

void MainWindow::on_actPrint_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    QPrintDialog printdlg;
    printdlg.setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange |
                        QAbstractPrintDialog::PrintShowPageSize | QAbstractPrintDialog::PrintCurrentPage);
    printdlg.exec();
    if ( printdlg.result() != QDialog::Accepted )
        return;

    QPrinter *printer = printdlg.printer();
    cureditor->printFile(printer);
}

void MainWindow::on_actClose_triggered()
{
    this->removeEditorAt(ui->tbwDocumentList->currentIndex());
}

void MainWindow::on_actQuit_triggered()
{
    this->close();
}

void MainWindow::on_actUndo_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->scriptUndo();
}

void MainWindow::on_actRedo_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->scriptRedo();
}

void MainWindow::on_actCut_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->cutScript();
}

void MainWindow::on_actCopy_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->copyScript();
}

void MainWindow::on_actPaste_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->pasteScript();
}

void MainWindow::on_actDefFigConfig_triggered()
{
    finUiFigConfigDlg figcfgdlg(this);
    figcfgdlg.fillFromFigureConfig(&this->_defFigConfig);

    figcfgdlg.exec();
    if ( figcfgdlg.result() != QDialog::Accepted )
        return;

    figcfgdlg.applyToFigureConfig(&this->_defFigConfig);

    for ( int i = 0; i < ui->tbwDocumentList->count(); i++ ) {
        finUiScriptEditor *editor = this->getEditorAt(i);
        if ( editor == nullptr )
            continue;

        editor->applyFigureConfig(&this->_defFigConfig);
    }
}

void MainWindow::on_actDefGraphConfig_triggered()
{
    finUiGraphConfigDlg graphcfgdlg(this);
    graphcfgdlg.fillFromGraphConfig(&this->_defGraphConfig);

    graphcfgdlg.exec();
    if ( graphcfgdlg.result() != QDialog::Accepted )
        return;

    graphcfgdlg.applyToGraphConfig(&this->_defGraphConfig);

    for ( int i = 0; i < ui->tbwDocumentList->count(); i++ ) {
        finUiScriptEditor *editor = this->getEditorAt(i);
        if ( editor == nullptr )
            continue;

        editor->applyGraphConfig(&this->_defGraphConfig);
    }
}

void MainWindow::on_actDraw_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->applyFigureConfig(&this->_defFigConfig);
    cureditor->applyGraphConfig(&this->_defGraphConfig);

    cureditor->drawOnPanel();
}

bool MainWindow::checkExportWarning(finUiScriptEditor *editor)
{
    if ( !editor->containsFigureObjects() ) {
        QMessageBox::StandardButton resbtn;
        resbtn = QMessageBox::warning(this, QString("Warning"),
                                      QString("No Figure on Panel. Export it anyway?"),
                                      QMessageBox::Ok | QMessageBox::Cancel,
                                      QMessageBox::Cancel);
        if ( resbtn != QMessageBox::Ok )
            return false;
    }
    return true;
}

void MainWindow::on_actExportPDF_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QString defpath = this->_fileDirPath;
    if ( cureditor->isFileOpened() )
        defpath = cureditor->getFilePath();
    QString filepath = QFileDialog::getSaveFileName(this, QString("Export to Adobe PDF"),
                                                    defpath,
                                                    QString("Adobe PDF Document (*.pdf)"));
    if ( filepath.isEmpty() )
        return;

    this->_fileDirPath = filepath;
    finErrorCode errcode = cureditor->exportToPDF(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the Adobe PDF file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actExportImage_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QString defpath = this->_fileDirPath;
    if ( cureditor->isFileOpened() )
        defpath = cureditor->getFilePath();
    QString filepath = QFileDialog::getSaveFileName(this, QString("Export to Image"),
                                                    defpath,
                                                    QString("Portable Network Graphics (*.png);;"
                                                            "Joint Photographic Experts Group (*.jpg *jpeg);;"
                                                            "Windows Bitmap (*.bmp);;"
                                                            "Portable Pixmap (*.ppm);;"
                                                            "Tagged Image File Format (*.tiff);;"
                                                            "X11 Bitmap (*.xbm);;"
                                                            "X11 Pixmap (*.xpm)"));
    if ( filepath.isEmpty() )
        return;

    this->_fileDirPath = filepath;
    finErrorCode errcode = cureditor->exportToImage(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the image file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actExportSVG_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QString defpath = this->_fileDirPath;
    if ( cureditor->isFileOpened() )
        defpath = cureditor->getFilePath();
    QString filepath = QFileDialog::getSaveFileName(this, QString("Export to SVG"),
                                                    defpath,
                                                    QString("Scalable Vector Graphics (*.svg)"));
    if ( filepath.isEmpty() )
        return;

    this->_fileDirPath = filepath;
    finErrorCode errcode = cureditor->exportToSVG(filepath);
    if ( finErrorKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the SVG file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actCopyFig_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    cureditor->copyFigure();
}

void MainWindow::on_actPrintFig_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QPrintDialog printdlg;
    printdlg.setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintShowPageSize);
    printdlg.exec();
    if ( printdlg.result() != QDialog::Accepted )
        return;

    QPrinter *printer = printdlg.printer();
    cureditor->printFigure(printer);
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
    if ( cureditor == nullptr )
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
    if ( cureditor == nullptr ) {
        this->setWindowTitle("FigureItNow");
    } else {
        this->setWindowTitle(cureditor->getWindowTitle() + QString(" - FigureItNow"));
    }
}

void MainWindow::on_dwcSysFuncList_itemTriggerred(const QString &, const QMap<QString, QVariant> &propmap)
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == nullptr )
        return;

    QVariant value = propmap.value("Prototype");
    if ( value.isNull() )
        return;

    cureditor->insertTextAtCurrentPos(value.toString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    finUiScriptEditor *preveditor = nullptr;
    while ( ui->tbwDocumentList->count() > 0 ) {
        finUiScriptEditor *cureditor = this->getEditorAt(0);
        if ( cureditor == preveditor ) {
            QMessageBox::StandardButton resbtn = QMessageBox::Ignore;
            QMessageBox::critical(this, tr("Error"),  tr("Error while closing script files!"),
                                  QMessageBox::Retry | QMessageBox::Ignore | QMessageBox::Abort,
                                  QMessageBox::Retry);

            switch ( resbtn ) {
              case QMessageBox::Retry:
                break;

              case QMessageBox::Ignore:
                ui->tbwDocumentList->removeTab(0);
                delete cureditor;
                continue;
                //break;

              case QMessageBox::Abort:
              default:
                event->accept();
                return;
                //break;
            }
        }

        finErrorCode errcode = this->removeEditorAt(0);
        if ( errcode == finErrorKits::EC_UNREADY_WARN ) {
            event->ignore();
            return;
        }
        preveditor = cureditor;
    }
    event->accept();
}


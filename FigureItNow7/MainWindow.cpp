#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <qmath.h>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
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

    neweditor->applyFigureConfig(&this->_defFigConfig);
    neweditor->applyGraphConfig(&this->_defGraphConfig);
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

    neweditor->applyFigureConfig(&this->_defFigConfig);
    neweditor->applyGraphConfig(&this->_defGraphConfig);
    QObject::connect(neweditor, SIGNAL(scriptModificationChanged(bool)),
                     this, SLOT(scriptEditor_scriptModificationChanged(bool)));

    ui->tbwDocumentList->addTab(neweditor, neweditor->getTabTitle());
    ui->tbwDocumentList->setCurrentWidget(neweditor);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool MainWindow::saveScriptFile(finUiScriptEditor *editor)
{
    if ( editor == NULL )
        return true;

    if ( !editor->isFileOpened() )
        return this->saveAsScriptFile(editor);

    if ( !editor->isScriptModified() )
        return true;

    finErrorCode errcode = editor->saveFile();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("The file cannot be saved on disk!"), QMessageBox::Ok);
        return true;
    }
    return true;
}

bool MainWindow::saveAsScriptFile(finUiScriptEditor *editor)
{
    if ( editor == NULL )
        return true;

    QFileDialog filedlg(this, QString("Save Script File"));
    filedlg.setAcceptMode(QFileDialog::AcceptSave);
    filedlg.setFileMode(QFileDialog::AnyFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return false;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() ) {
        QMessageBox::warning(this, QString("Warning"),
                             QString("You need to select a file to save."), QMessageBox::Ok);
        return false;
    }

    finErrorCode errcode = editor->saveAsFile(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
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
        return finErrorCodeKits::FIN_EC_NOT_FOUND;

    finUiScriptEditor *editor = this->getEditorAt(idx);
    bool saveres = true;
    if ( editor == NULL ) {
        ui->tbwDocumentList->removeTab(idx);
        return finErrorCodeKits::FIN_EC_SUCCESS;
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
            return finErrorCodeKits::FIN_EC_UNREADY_WARN;
        break;

      case QMessageBox::No:
        goto out;
        break;

      case QMessageBox::Cancel:
      default:
        return finErrorCodeKits::FIN_EC_UNREADY_WARN;
        break;
    }

out:
    ui->tbwDocumentList->removeTab(idx);
    delete editor;
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

    this->saveScriptFile(cureditor);
}

void MainWindow::on_actSaveAs_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    this->saveAsScriptFile(cureditor);
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
        if ( editor == NULL )
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
        if ( editor == NULL )
            continue;

        editor->applyGraphConfig(&this->_defGraphConfig);
    }
}

void MainWindow::on_actDraw_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
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
    if ( cureditor == NULL )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QFileDialog filedlg(this, QString("Export to Adobe PDF"));
    filedlg.setAcceptMode(QFileDialog::AcceptSave);
    filedlg.setFileMode(QFileDialog::AnyFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() ) {
        QMessageBox::warning(this, QString("Warning"),
                             QString("You need to select a file to export."), QMessageBox::Ok);
        return;
    }

    finErrorCode errcode = cureditor->exportToPDF(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the Adobe PDF file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actExportImage_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QFileDialog filedlg(this, QString("Export to Image"));
    filedlg.setAcceptMode(QFileDialog::AcceptSave);
    filedlg.setFileMode(QFileDialog::AnyFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() ) {
        QMessageBox::warning(this, QString("Warning"),
                             QString("You need to select a file to export."), QMessageBox::Ok);
        return;
    }

    finErrorCode errcode = cureditor->exportToImage(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the image file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actExportSVG_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QFileDialog filedlg(this, QString("Export to SVG"));
    filedlg.setAcceptMode(QFileDialog::AcceptSave);
    filedlg.setFileMode(QFileDialog::AnyFile);

    filedlg.exec();
    if ( filedlg.result() != QDialog::Accepted )
        return;

    QStringList filepaths = filedlg.selectedFiles();
    if ( filepaths.empty() ) {
        QMessageBox::warning(this, QString("Warning"),
                             QString("You need to select a file to export."), QMessageBox::Ok);
        return;
    }

    finErrorCode errcode = cureditor->exportToSVG(filepaths.first());
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        QMessageBox::critical(this, QString("Error"),
                              QString("Cannot write the SVG file."), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_actCopyFig_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->copyFigure();
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
    finUiScriptEditor *preveditor = NULL;
    while ( ui->tbwDocumentList->count() > 0 ) {
        finUiScriptEditor *cureditor = this->getEditorAt(0);
        if ( cureditor == preveditor ) {
            QMessageBox::StandardButton resbtn = QMessageBox::Ignore;
            QMessageBox::critical(this, QString("Error"),  QString("Error while closing script files!"),
                                  QMessageBox::Retry | QMessageBox::Ignore | QMessageBox::Abort,
                                  QMessageBox::Retry);

            switch ( resbtn ) {
              case QMessageBox::Retry:
                break;

              case QMessageBox::Ignore:
                ui->tbwDocumentList->removeTab(0);
                delete cureditor;
                continue;
                break;

              case QMessageBox::Abort:
              default:
                event->accept();
                return;
                break;
            }
        }

        finErrorCode errcode = this->removeEditorAt(0);
        if ( errcode == finErrorCodeKits::FIN_EC_UNREADY_WARN ) {
            event->ignore();
            return;
        }
        preveditor = cureditor;
    }
    event->accept();
}

void MainWindow::on_actPrintFig_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;
    if ( !checkExportWarning(cureditor) )
        return;

    QPrintDialog printdlg;
    printdlg.exec();
    if ( printdlg.result() != QDialog::Accepted )
        return;

    QPrinter *printer = printdlg.printer();
    cureditor->printFigure(printer);
}

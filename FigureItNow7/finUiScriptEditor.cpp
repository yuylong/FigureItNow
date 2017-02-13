#include "finUiScriptEditor.h"
#include "ui_finUiScriptEditor.h"

#include <QDebug>
#include <QStringBuilder>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QMessageBox>
#include <QPageSize>
#include <QMarginsF>
#include <QPdfWriter>

#include "finGraphPanelScene.h"
#include "finGraphPanelWidget.h"


finUiScriptEditor::finUiScriptEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::finUiScriptEditor)
{
    finErrorCode errcode;
    this->_filepath = QString();
    this->_filename = QString("Unnamed");

    errcode = this->_machine.initEnvironmentFromRoot();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        qDebug() << "ERROR: Cannot init the execution machine for script code!";

    this->_machine.setFigureContainer(&this->_figContainer);
    this->_scenePainter.setScene(&this->_figScene);

    ui->setupUi(this);
    this->_syntaxHighlighter = new finUiSyntaxHighlighter(ui->pteScriptCode->document());
    ui->pteScriptCode->setCurrentCharFormat(this->_syntaxHighlighter->getBaseFormat());
    //ui->gvwGraphPanel->setScene(&this->_figScene);
}

finUiScriptEditor::~finUiScriptEditor()
{
    if ( this->_syntaxHighlighter != NULL )
        delete this->_syntaxHighlighter;

    delete ui;
}

const QString &finUiScriptEditor::getFilePath() const
{
    return this->_filepath;
}

const QString &finUiScriptEditor::getFilename() const
{
    return this->_filename;
}

const QString &finUiScriptEditor::getFileDisplayPath() const
{
    if ( this->_filepath.isEmpty() )
        return this->_filename;
    else
        return this->_filepath;
}

finErrorCode finUiScriptEditor::setFilename(const QString &filepath)
{
    this->_filepath = filepath;
    this->_filename = filepath.split(QRegExp(QString("[\\\\\\/]"))).last();
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finUiScriptEditor::isFileOpened() const
{
    return !this->_filepath.isEmpty();
}

bool finUiScriptEditor::isScriptModified() const
{
    QTextDocument *scriptdoc = ui->pteScriptCode->document();
    if ( scriptdoc == NULL )
        return false;

    return scriptdoc->isModified();
}

QString finUiScriptEditor::getTabTitle() const
{
    if ( ui->pteScriptCode->document()->isModified() )
        return this->_filename + QString("*");
    else
        return this->_filename;
}

QString finUiScriptEditor::getWindowTitle() const
{
    QString titlebase = this->_filepath;
    if ( !this->isFileOpened() )
        titlebase = this->_filename;

    if ( ui->pteScriptCode->document()->isModified() )
        return titlebase + QString("*");
    else
        return titlebase;
}

finErrorCode finUiScriptEditor::openFile(const QString &filepath)
{
    QFile file(filepath);
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
         return finErrorCodeKits::FIN_EC_FILE_NOT_OPEN;

    QTextStream in(&file);
    ui->pteScriptCode->setPlainText(in.readAll());
    file.close();
    ui->pteScriptCode->document()->setModified(false);

    this->setFilename(filepath);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finUiScriptEditor::saveAsFile(const QString &filepath)
{
    this->setFilename(filepath);
    return this->saveFile();
}

finErrorCode finUiScriptEditor::saveFile()
{
    if ( !this->isFileOpened() )
        return finErrorCodeKits::FIN_EC_STATE_ERROR;

    QFile file(this->_filepath);
    if ( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
        return finErrorCodeKits::FIN_EC_FILE_NOT_OPEN;

    QTextStream out(&file);
    out << ui->pteScriptCode->toPlainText();
    out.flush();
    file.close();
    ui->pteScriptCode->document()->setModified(false);
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

bool finUiScriptEditor::scriptUndoAvailable() const
{
    return ui->pteScriptCode->document()->isUndoAvailable();
}

bool finUiScriptEditor::scriptRedoAvailable() const
{
    return ui->pteScriptCode->document()->isRedoAvailable();
}

void finUiScriptEditor::scriptUndo()
{
    if ( !this->scriptUndoAvailable() )
        return;

    return ui->pteScriptCode->undo();
}

void finUiScriptEditor::scriptRedo()
{
    if ( !this->scriptRedoAvailable() )
        return;

    return ui->pteScriptCode->redo();
}

void finUiScriptEditor::copyScript()
{
    ui->pteScriptCode->copy();
}

void finUiScriptEditor::cutScript()
{
    ui->pteScriptCode->cut();
}

void finUiScriptEditor::pasteScript()
{
    if ( !ui->pteScriptCode->canPaste() )
        return;

    ui->pteScriptCode->paste();
}

finErrorCode finUiScriptEditor::applyFigureConfig(finFigureConfig *figconfig)
{
    finFigureConfig *mycfg = this->_figContainer.getFigureConfig();
    return figconfig->cloneFigureConfig(mycfg);
}

finErrorCode finUiScriptEditor::applyGraphConfig(finGraphConfig *graphconfig)
{
    finGraphConfig *mycfg = this->_figContainer.getGraphConfig();
    return mycfg->copyGraphConfig(graphconfig);
}

bool finUiScriptEditor::containsFigureObjects() const
{
    return (this->_figContainer.getFigureObjectCount() > 0);
}

finErrorCode finUiScriptEditor::drawOnPanel()
{
    if ( ui->gvwGraphPanel->scene() == NULL )
        ui->gvwGraphPanel->setScene(&this->_figScene);

    finErrorCode errcode;
    this->_machine.setScriptCode(ui->pteScriptCode->toPlainText());

    errcode = this->_machine.compile();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when compile! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_machine.execute();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when execute! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_scenePainter.setFigureContainer(&this->_figContainer);
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when draw! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_scenePainter.draw();
    if ( finErrorCodeKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when apply drawing! (" << errcode << ")";
        return errcode;
    }
    return finErrorCodeKits::FIN_EC_SUCCESS;
}

finErrorCode finUiScriptEditor::exportToPDF(const QString &filepath)
{
    static const int _defResolution = 72;

    finGraphConfig *graphcfg = this->_figContainer.getGraphConfig();
    QPdfWriter pdfwrt(filepath);

    pdfwrt.setPageSize(QPageSize(graphcfg->getPanelPixelSize() / _defResolution, QPageSize::Inch));
    pdfwrt.setPageMargins(QMarginsF(0.0, 0.0, 0.0, 0.0));
    pdfwrt.setResolution(_defResolution);

    finGraphPanelWidget graphpanel;
    graphpanel.setWidget(&pdfwrt);
    graphpanel.setFigureContainer(&this->_figContainer);

    finErrorCode errcode = graphpanel.draw();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        return errcode;

    return finErrorCodeKits::FIN_EC_SUCCESS;
}

void finUiScriptEditor::on_pteScriptCode_modificationChanged(bool modified)
{
    emit this->scriptModificationChanged(modified);
}

void finUiScriptEditor::on_pteScriptCode_copyAvailable(bool yes)
{
    emit this->scriptCopyAvailable(yes);
}

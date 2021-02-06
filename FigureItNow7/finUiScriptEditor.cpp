/*-
 * GNU GENERAL PUBLIC LICENSE, version 3
 * See LICENSE file for detail.
 *
 * Author: Yulong Yu
 * Copyright(c) 2015-2017 Yulong Yu. All rights reserved.
 */

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
#include <QGuiApplication>
#include <QClipboard>
#include <QByteArray>
#include <QBuffer>
#include <QMimeData>

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
    if ( finErrorKits::isErrorResult(errcode) )
        qDebug() << "ERROR: Cannot init the execution machine for script code!";

    this->_machine.setFigureContainer(&this->_figContainer);
    this->_machine.setExecuteErrorDumper(&this->_compilerOutput);
    this->_scenePainter.setScene(&this->_figScene);

    ui->setupUi(this);
    this->_syntaxHighlighter = new finUiSyntaxHighlighter(ui->pteScriptCode->document());
    ui->pteScriptCode->setCurrentCharFormat(this->_syntaxHighlighter->getBaseFormat());
    //ui->gvwGraphPanel->setScene(&this->_figScene);
}

finUiScriptEditor::~finUiScriptEditor()
{
    if ( this->_syntaxHighlighter != nullptr )
        delete this->_syntaxHighlighter;

    delete ui;
}

QPlainTextEdit *finUiScriptEditor::getCompilerOutput() const
{
    return this->_compilerOutput.getWidget();
}

finErrorCode finUiScriptEditor::setCompilerOutput(QPlainTextEdit *pte)
{
    return this->_compilerOutput.setWidget(pte);
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
    this->_filename = filepath.split(QRegularExpression(QString("[\\\\\\/]"))).last();
    return finErrorKits::EC_SUCCESS;
}

bool finUiScriptEditor::isFileOpened() const
{
    return !this->_filepath.isEmpty();
}

bool finUiScriptEditor::isScriptModified() const
{
    QTextDocument *scriptdoc = ui->pteScriptCode->document();
    if ( scriptdoc == nullptr )
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
         return finErrorKits::EC_FILE_NOT_OPEN;

    QTextStream in(&file);
    ui->pteScriptCode->setPlainText(in.readAll());
    file.close();
    ui->pteScriptCode->document()->setModified(false);

    this->setFilename(filepath);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::saveAsFile(const QString &filepath)
{
    this->setFilename(filepath);
    return this->saveFile();
}

finErrorCode finUiScriptEditor::saveFile()
{
    if ( !this->isFileOpened() )
        return finErrorKits::EC_STATE_ERROR;

    QFile file(this->_filepath);
    if ( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
        return finErrorKits::EC_FILE_NOT_OPEN;

    QTextStream out(&file);
    out << ui->pteScriptCode->toPlainText();
    out.flush();
    file.close();
    ui->pteScriptCode->document()->setModified(false);
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::printFile(QPrinter *printer)
{
    ui->pteScriptCode->print(printer);
    return finErrorKits::EC_SUCCESS;
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

void finUiScriptEditor::insertTextAtCurrentPos(const QString &text)
{
    ui->pteScriptCode->insertPlainText(text);
}

finErrorCode finUiScriptEditor::getFigureImage(QImage *outimg)
{
    if ( outimg == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finGraphConfig *graphcfg = this->_figContainer.getGraphConfig();
    QImage img(graphcfg->getPanelPixelSize().toSize(), QImage::Format_ARGB32);
    //outimg.fill(Qt::transparent);

    finGraphPanelWidget graphpanel;
    graphpanel.setWidget(&img);
    graphpanel.setFigureContainer(&this->_figContainer);

    finErrorCode errcode = graphpanel.draw();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    *outimg = img;
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::getFigureSVGImage(QSvgGenerator *svggen)
{
    if ( svggen == nullptr )
        return finErrorKits::EC_NULL_POINTER;

    finGraphConfig *graphcfg = this->_figContainer.getGraphConfig();
    svggen->setTitle(this->_filename);
    svggen->setDescription(QString("Generated by FigureItNow7."));
    svggen->setSize(graphcfg->getPanelPixelSize().toSize());
    svggen->setViewBox(graphcfg->getWholePanelPixelRect().toRect());

    finGraphPanelWidget graphpanel;
    graphpanel.setWidget(svggen);
    graphpanel.setFigureContainer(&this->_figContainer);

    finErrorCode errcode = graphpanel.draw();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

void finUiScriptEditor::copyFigure()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if ( clipboard == nullptr )
        return;

    QMimeData *md = new QMimeData();
    if ( md == nullptr )
        return;

    QImage outimg;
    finErrorCode errcode = this->getFigureImage(&outimg);
    if ( finErrorKits::isErrorResult(errcode) )
        return;

    md->setImageData(outimg);

    QByteArray svgdata;
    QBuffer svgbuf(&svgdata);
    svgbuf.open(QIODevice::WriteOnly);

    QSvgGenerator svggen;
    svggen.setOutputDevice(&svgbuf);
    this->getFigureSVGImage(&svggen);
    svgbuf.close();

    md->setData(QString("image/svg+xml"), svgdata);
    clipboard->setMimeData(md);
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
    if ( ui->gvwGraphPanel->scene() == nullptr )
        ui->gvwGraphPanel->setScene(&this->_figScene);

    finErrorCode errcode;
    this->_machine.setScriptCode(ui->pteScriptCode->toPlainText());

    errcode = this->_machine.compile();
    if ( finErrorKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when compile! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_machine.execute();
    if ( finErrorKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when execute! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_scenePainter.setFigureContainer(&this->_figContainer);
    if ( finErrorKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when draw! (" << errcode << ")";
        return errcode;
    }

    errcode = this->_scenePainter.draw();
    if ( finErrorKits::isErrorResult(errcode) ) {
        qDebug() << "ERROR when apply drawing! (" << errcode << ")";
        return errcode;
    }
    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::exportToPDF(const QString &filepath)
{
    static const int _defResolution = 72;

    finGraphConfig *graphcfg = this->_figContainer.getGraphConfig();
    QPdfWriter pdfwrt(filepath);

    pdfwrt.setTitle(this->_filename);
    pdfwrt.setCreator(QString("FigureItNow 7"));
    pdfwrt.setPageSize(QPageSize(graphcfg->getPanelPixelSize() / _defResolution, QPageSize::Inch));
    pdfwrt.setPageMargins(QMarginsF(0.0, 0.0, 0.0, 0.0));
    pdfwrt.setResolution(_defResolution);

    finGraphPanelWidget graphpanel;
    graphpanel.setWidget(&pdfwrt);
    graphpanel.setFigureContainer(&this->_figContainer);

    finErrorCode errcode = graphpanel.draw();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::exportToImage(const QString &filepath)
{
    QImage outimg;
    finErrorCode errcode = this->getFigureImage(&outimg);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    bool saveres = outimg.save(filepath);
    if ( !saveres )
        return finErrorKits::EC_FILE_NOT_OPEN;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::exportToSVG(const QString &filepath)
{
    QSvgGenerator svggen;
    svggen.setFileName(filepath);

    finErrorCode errcode = this->getFigureSVGImage(&svggen);
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

finErrorCode finUiScriptEditor::printFigure(QPrinter *printer)
{
    static const int _defResolution = 72;

    finGraphConfig *graphcfg = this->_figContainer.getGraphConfig();
    printer->setPageSize(QPageSize(graphcfg->getPanelPixelSize() / _defResolution, QPageSize::Inch));
    printer->setResolution(_defResolution);

    finGraphPanelWidget graphpanel;
    graphpanel.setWidget(printer);
    graphpanel.setFigureContainer(&this->_figContainer);

    finErrorCode errcode = graphpanel.draw();
    if ( finErrorKits::isErrorResult(errcode) )
        return errcode;

    return finErrorKits::EC_SUCCESS;
}

void finUiScriptEditor::on_pteScriptCode_modificationChanged(bool modified)
{
    emit this->scriptModificationChanged(modified);
}

void finUiScriptEditor::on_pteScriptCode_copyAvailable(bool yes)
{
    emit this->scriptCopyAvailable(yes);
}

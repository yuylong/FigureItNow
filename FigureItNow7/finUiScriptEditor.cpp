#include "finUiScriptEditor.h"
#include "ui_finUiScriptEditor.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>

#include "finGraphPanelScene.h"


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

bool finUiScriptEditor::isFileOpened() const
{
    return !this->_filepath.isEmpty();
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

finErrorCode finUiScriptEditor::setFilename(const QString &filepath)
{
    this->_filepath = filepath;
    this->_filename = filepath.split(QRegExp(QString("[\\\\\\/]"))).last();
    return finErrorCodeKits::FIN_EC_SUCCESS;
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

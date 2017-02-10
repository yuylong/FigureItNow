#include "finUiScriptEditor.h"
#include "ui_finUiScriptEditor.h"

#include <QDebug>

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

QString finUiScriptEditor::getTabTitle() const
{
    if ( ui->pteScriptCode->document()->isModified() )
        return this->_filename + QString("*");
    else
        return this->_filename;
}

QString finUiScriptEditor::getWindowTitle() const
{
    if ( ui->pteScriptCode->document()->isModified() )
        return this->_filepath + QString("*");
    else
        return this->_filepath;
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

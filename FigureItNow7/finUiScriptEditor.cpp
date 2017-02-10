#include "finUiScriptEditor.h"
#include "ui_finUiScriptEditor.h"

finUiScriptEditor::finUiScriptEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::finUiScriptEditor)
{
    ui->setupUi(this);
    this->_syntaxHighlighter = new finUiSyntaxHighlighter(ui->pteScriptCode->document());
}

finUiScriptEditor::~finUiScriptEditor()
{
    if ( this->_syntaxHighlighter != NULL )
        delete this->_syntaxHighlighter;

    delete ui;
}

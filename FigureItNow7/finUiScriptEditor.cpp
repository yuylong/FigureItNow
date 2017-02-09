#include "finUiScriptEditor.h"
#include "ui_finUiScriptEditor.h"

finUiScriptEditor::finUiScriptEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::finUiScriptEditor)
{
    ui->setupUi(this);
}

finUiScriptEditor::~finUiScriptEditor()
{
    delete ui;
}

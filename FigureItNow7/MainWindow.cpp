#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <qmath.h>
#include <QPainter>

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

void MainWindow::on_actDraw_triggered()
{
    finUiScriptEditor *cureditor = this->getCurrentEditor();
    if ( cureditor == NULL )
        return;

    cureditor->drawOnPanel();
}

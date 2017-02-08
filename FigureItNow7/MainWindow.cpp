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
    finErrorCode errcode;
    this->synreader = NULL;
    errcode = this->machine.initEnvironmentFromRoot();
    printf ("init Env (errno=%d)\n", errcode);

    ui->setupUi(this);
    this->synhigh = new finUiSyntaxHighlighter(ui->plainTextEdit->document());
    this->figui = new finUiFigureWidget(this);
    this->figui->setObjectName("FigureContainer");
    this->figui->setGeometry(QRect(820, 50, 640, 480));
    ui->graphicsView->setScene(&this->scene);

    errcode = this->machine.setFigureContainer(this->figui->getFigureContainer());
    printf ("init Fig Container (errno=%d)\n", errcode);

    fflush(stdout);
}

MainWindow::~MainWindow()
{
    delete this->figui;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (this->synreader != NULL)
        delete this->synreader;

    this->synreader = new finSyntaxReader();
    if (ui->plainTextEdit->toPlainText().trimmed().size() == 0)
        this->synreader->setScriptCode(ui->lineEdit->text());
    else
        this->synreader->setScriptCode(ui->plainTextEdit->toPlainText());
    this->synreader->startRead();
}



void MainWindow::on_pushButton_2_clicked()
{
    if (this->synreader == NULL) {
        ui->lineEdit_2->setText(QString("XXX"));
        return;
    }

    finErrorCode errcode;
    errcode = this->synreader->readNextToken();

    if (finErrorCodeKits::isErrorResult(errcode)) {
        ui->lineEdit_2->setText(QString("Err ")+QString().setNum((int)errcode));
        return;
    }

    if (errcode == finErrorCodeKits::FIN_EC_REACH_BOTTOM) {
        ui->lineEdit_2->setText(QString("BOT"));
        return;
    }

    QList<finSyntaxNode *> *synstack = this->synreader->getSyntaxStack();

    for (int i = 0; i < synstack->count(); i++) {
        finSyntaxNode *node = synstack->at(i);
        node->dump();
    }
    printf("\n");
    fflush(stdout);

    if ( !synstack->empty() )
        ui->lineEdit_2->setText(synstack->first()->getCommandLexNode()->getString());
    else
        ui->lineEdit_2->setText(QString("EMPTY"));
}

void MainWindow::on_pushButton_3_clicked()
{
    finErrorCode errcode;

    this->machine.setScriptCode(ui->plainTextEdit->toPlainText());

    printf("BlockCounts:%d\n", ui->plainTextEdit->blockCount());

    errcode = this->machine.compile();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        printf("ERROR when compile! (%d)\n", errcode);

    errcode = this->machine.execute();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        printf("ERROR when execute! (%d)\n", errcode);

    if (this->machine.getSyntaxTree() != NULL)
        this->machine.getSyntaxTree()->getRootNode()->dump();
    else
        printf("Syntax Tree is NULL\n");

    finFigureContainer *figcontainer = this->figui->getFigureContainer();
    figcontainer->dump();
    this->figui->applyFigure();
    fflush(stdout);

    finGraphPanelScene gp;
    gp.setFigureContainer(figcontainer);
    gp.setScene(&this->scene);
    gp.draw();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    e->accept();
}


void MainWindow::on_pushButton_4_clicked()
{
    finFigureContainer *figcontainer = this->figui->getFigureContainer();
    finFigureConfig *figconfig = figcontainer->getFigureConfig();

    finUiFigConfigDlg figcfgdlg;
    figcfgdlg.fillFromFigureConfig(figconfig);
    figcfgdlg.exec();

    if ( figcfgdlg.result() != QDialog::Accepted )
        return;

    figcfgdlg.applyToFigureConfig(figconfig);
}

void MainWindow::on_pushButton_5_clicked()
{
    finFigureContainer *figcontainer = this->figui->getFigureContainer();
    finGraphConfig *graphconfig = figcontainer->getGraphConfig();

    finUiGraphConfigDlg graphcfgdlg;
    graphcfgdlg.fillFromGraphConfig(graphconfig);
    graphcfgdlg.exec();

    if ( graphcfgdlg.result() != QDialog::Accepted )
        return;

    graphcfgdlg.applyToGraphConfig(graphconfig);
}

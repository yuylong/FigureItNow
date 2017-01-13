#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QPainter>

#include "finExecEnvironment.h"
#include "finGraphPanelScene.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    finErrorCode errcode;
    this->synreader = NULL;
    errcode = this->machine.initEnvironmentFromRoot();
    printf ("init Env (errno=%d)\n", errcode);

    ui->setupUi(this);
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


    ui->lineEdit_2->setText(synstack->first()->getCommandLexNode()->getString());
}

void MainWindow::on_pushButton_3_clicked()
{
    finErrorCode errcode;

    this->machine.setScriptCode(ui->plainTextEdit->toPlainText());

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

    this->figui->getFigureContainer()->dump();
    fflush(stdout);

    finGraphPanelScene gp;
    gp.setScene(&this->scene);
    gp.drawContainer(this->figui->getFigureContainer());

    this->repaint();
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    e->accept();
}


#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->synreader = NULL;
    this->machine.initEnvironmentFromRoot();
    this->machine.setFigureContainer(&this->figContainer);
    ui->setupUi(this);
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

    this->machine.setScriptCode(this->ui->lineEdit->text());
    this->machine.releaseCompile();
    errcode = this->machine.compile();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        printf("ERROR when compile!\n");

    errcode = this->machine.execute();
    if ( finErrorCodeKits::isErrorResult(errcode) )
        printf("ERROR when execute!\n");

    this->figContainer.dump();
}

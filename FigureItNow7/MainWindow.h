#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "finLexNode.h"
#include "finErrorCode.h"
#include "finLexReader.h"
#include "finSyntaxReader.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finGraphPanelWidget.h"
#include "finUiFigureWidget.h"


namespace Ui {
class MainWindow;
}

/*!
 * \brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    /*!
     * \brief ui
     */
    Ui::MainWindow *ui;
    finUiFigureWidget *figui;

    void paintEvent(QPaintEvent *e);

    finSyntaxReader *synreader;
    finExecMachine machine;
    QGraphicsScene scene;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "finlexnode.h"
#include "finerrorcode.h"
#include "finlexreader.h"
#include "finSyntaxReader.h"

#include "finfigureobject.h"

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

private:
    /*!
     * \brief ui
     */
    Ui::MainWindow *ui;


    finSyntaxReader *synreader;
};

#endif // MAINWINDOW_H

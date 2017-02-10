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
#include "finUiSyntaxHighlighter.h"
#include "finUiScriptEditor.h"


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
    void on_actDraw_triggered();

    void on_actOpen_triggered();

private:
    /*!
     * \brief ui
     */
    Ui::MainWindow *ui;

    finUiScriptEditor *getCurrentEditor() const;
};

#endif // MAINWINDOW_H

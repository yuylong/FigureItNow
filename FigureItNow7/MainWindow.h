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

    finErrorCode openScriptFile(const QString &filepath);
    finErrorCode createNewScriptFile();

private slots:
    void on_actOpen_triggered();
    void on_actSave_triggered();
    void on_actSaveAs_triggered();
    void on_actClose_triggered();

    void on_actDraw_triggered();

    void on_actWiki_triggered();

    void on_tbwDocumentList_tabCloseRequested(int index);


private:
    /*!
     * \brief ui
     */
    Ui::MainWindow *ui;

    finUiScriptEditor *getCurrentEditor() const;
    finUiScriptEditor *getEditorAt(int idx) const;
    finErrorCode removeEditorAt(int idx);
};

#endif // MAINWINDOW_H

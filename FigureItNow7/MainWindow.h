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
    void on_actNew_triggered();
    void on_actOpen_triggered();
    void on_actSave_triggered();
    void on_actSaveAs_triggered();
    void on_actClose_triggered();
    void on_actQuit_triggered();

    void on_actUndo_triggered();
    void on_actRedo_triggered();
    void on_actCut_triggered();
    void on_actCopy_triggered();
    void on_actPaste_triggered();
    void on_actDefFigConfig_triggered();
    void on_actDefGraphConfig_triggered();

    void on_actDraw_triggered();

    void on_actManual_triggered();
    void on_actWiki_triggered();
    void on_actAbout_triggered();

    void scriptEditor_scriptModificationChanged(bool modified);
    void on_tbwDocumentList_currentChanged(int index);
    void on_tbwDocumentList_tabCloseRequested(int index);


    void on_actExportPDF_triggered();

    void on_actExportImage_triggered();

    void on_actCopyFig_triggered();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    /*!
     * \brief ui
     */
    Ui::MainWindow *ui;
    finFigureConfig _defFigConfig;
    finGraphConfig _defGraphConfig;

    finUiScriptEditor *getCurrentEditor() const;
    finUiScriptEditor *getEditorAt(int idx) const;
    int findEditorIndex(finUiScriptEditor *editor) const;

    QString getSaveFileQuestionString(finUiScriptEditor *editor) const;
    bool saveScriptFile(finUiScriptEditor *editor);
    bool saveAsScriptFile(finUiScriptEditor *editor);
    finErrorCode removeEditorAt(int idx);

    bool checkExportWarning(finUiScriptEditor *editor);
};

#endif // MAINWINDOW_H

#ifndef FINUISCRIPTEDITOR_H
#define FINUISCRIPTEDITOR_H

#include <QWidget>
#include <QGraphicsScene>

#include "finErrorCode.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finGraphPanelScene.h"
#include "finUiSyntaxHighlighter.h"

namespace Ui {
class finUiScriptEditor;
}

class finUiScriptEditor : public QWidget
{
    Q_OBJECT

protected:
    finUiSyntaxHighlighter *_syntaxHighlighter;
    finExecMachine _machine;
    finFigureContainer _figContainer;
    finGraphPanelScene _scenePainter;
    QGraphicsScene _figScene;

public:
    explicit finUiScriptEditor(QWidget *parent = 0);
    ~finUiScriptEditor();

    finErrorCode drawOnPanel();


private:
    Ui::finUiScriptEditor *ui;
};

#endif // FINUISCRIPTEDITOR_H

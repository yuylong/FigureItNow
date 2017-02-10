#ifndef FINUISCRIPTEDITOR_H
#define FINUISCRIPTEDITOR_H

#include <QWidget>

#include "finErrorCode.h"
#include "finUiSyntaxHighlighter.h"

namespace Ui {
class finUiScriptEditor;
}

class finUiScriptEditor : public QWidget
{
    Q_OBJECT

protected:
    finUiSyntaxHighlighter *_syntaxHighlighter;

public:
    explicit finUiScriptEditor(QWidget *parent = 0);
    ~finUiScriptEditor();



private:
    Ui::finUiScriptEditor *ui;
};

#endif // FINUISCRIPTEDITOR_H

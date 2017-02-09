#ifndef FINUISCRIPTEDITOR_H
#define FINUISCRIPTEDITOR_H

#include <QWidget>

namespace Ui {
class finUiScriptEditor;
}

class finUiScriptEditor : public QWidget
{
    Q_OBJECT

public:
    explicit finUiScriptEditor(QWidget *parent = 0);
    ~finUiScriptEditor();

private:
    Ui::finUiScriptEditor *ui;
};

#endif // FINUISCRIPTEDITOR_H

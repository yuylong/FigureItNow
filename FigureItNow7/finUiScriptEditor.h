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

    QString _filepath;
    QString _filename;

public:
    explicit finUiScriptEditor(QWidget *parent = 0);
    ~finUiScriptEditor();

    const QString &getFilePath() const;
    const QString &getFilename() const;
    bool isFileOpened() const;

    QString getTabTitle() const;
    QString getWindowTitle() const;

    finErrorCode openFile(const QString &filepath);
    finErrorCode saveAsFile(const QString &filepath);
    finErrorCode saveFile();

    finErrorCode drawOnPanel();


private:
    Ui::finUiScriptEditor *ui;

    finErrorCode setFilename(const QString &filepath);
};

#endif // FINUISCRIPTEDITOR_H

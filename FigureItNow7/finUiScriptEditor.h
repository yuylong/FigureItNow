#ifndef FINUISCRIPTEDITOR_H
#define FINUISCRIPTEDITOR_H

#include <QWidget>
#include <QTabWidget>
#include <QGraphicsScene>
#include <QImage>
#include <QSvgGenerator>

#include "finErrorCode.h"
#include "finExecMachine.h"
#include "finFigureContainer.h"
#include "finFigureConfig.h"
#include "finGraphPanelScene.h"
#include "finGraphConfig.h"
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
    const QString &getFileDisplayPath() const;
    bool isFileOpened() const;
    bool isScriptModified() const;

    QString getTabTitle() const;
    QString getWindowTitle() const;

    finErrorCode openFile(const QString &filepath);
    finErrorCode saveAsFile(const QString &filepath);
    finErrorCode saveFile();

    bool scriptUndoAvailable() const;
    bool scriptRedoAvailable() const;
    void scriptUndo();
    void scriptRedo();

    void copyScript();
    void cutScript();
    void pasteScript();
    void copyFigure();

    finErrorCode applyFigureConfig(finFigureConfig *figconfig);
    finErrorCode applyGraphConfig(finGraphConfig *graphconfig);

    bool containsFigureObjects() const;
    finErrorCode drawOnPanel();
    finErrorCode exportToPDF(const QString &filepath);
    finErrorCode exportToImage(const QString &filepath);
    finErrorCode exportToSVG(const QString &filepath);

private:
    Ui::finUiScriptEditor *ui;

    finErrorCode setFilename(const QString &filepath);
    finErrorCode getFigureImage(QImage *outimg);
    finErrorCode getFigureSVGImage(QSvgGenerator *svggen);

private slots:
    void on_pteScriptCode_modificationChanged(bool modified);
    void on_pteScriptCode_copyAvailable(bool yes);

signals:
    void scriptModificationChanged(bool modified);
    void scriptCopyAvailable(bool yes);
};

#endif // FINUISCRIPTEDITOR_H

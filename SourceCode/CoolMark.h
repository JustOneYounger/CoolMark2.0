#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CoolMark.h"
#include "CoolMarkInclude.h"
#include "CoolMarkMarkDownEditer.h"
#include "CoolMarkCssEditer.h"
#include "FilesThread.h"
#include "OutputPDF.h"
#include "CoolMarkMenuBar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CoolMarkClass; };
QT_END_NAMESPACE

class CoolMark : public QMainWindow
{
    Q_OBJECT

public:
    CoolMark(QWidget *parent = nullptr);
    ~CoolMark();
    
    QString MainPath = QCoreApplication::applicationDirPath();
    QString python_converter_path = MainPath + "/python/dist/MarkDownToHtml/MarkDownToHtml.exe";
    MarkdownConverterThread* converterThread;
    CombinationThread* combinationThread;

private:
    Ui::CoolMarkClass *ui;
    CoolMarkMarkDownEditer* markdownediter;
    CoolMarkCssEditer* cssediter;
    OutputPDF* outputdialog;
    CoolMarkMenuBar* mymenubar;

//Define basic window functions
private:
    void BasicWindow();
    bool isMove;
    QPoint MousePos;
    bool isMax = true;
    bool isOutput = false;
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void InitWindow();
    void ShortcutWindow();
    void Converter();
    void Preview();
    void Open();
    void Save();

private:
    QShortcut* ChangeHelp;
    QShortcut* ShortcutPreview;
    QShortcut* OpenMarkdown;
    QShortcut* SaveMarkdown;
    QShortcut* Output;
};

#pragma once
#include "CoolMarkInclude.h"

class CoolMarkCssEditerHighlight : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    CoolMarkCssEditerHighlight(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QRegularExpression Class_Expression;
    QTextCharFormat Class_Format;

    QRegularExpression Attribute_Expression;
    QTextCharFormat Attribute_Format;

    QRegularExpression Comment_StartExpression;
    QRegularExpression Comment_EndExpression;
    QTextCharFormat Comment_Format;
};

class CoolMarkCssEditer :public QPlainTextEdit
{
    Q_OBJECT
public:
    CoolMarkCssEditer(QWidget* parent = nullptr);
    ~CoolMarkCssEditer();

public:
    //左侧行号
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

protected:
    //左侧行号
    void resizeEvent(QResizeEvent* event) override;

private slots:
    //左侧行号
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    QWidget* cssediterlinenumberarea;
    CoolMarkCssEditerHighlight* cssediterhighlight;

protected:
    void keyPressEvent(QKeyEvent* e) override;
};
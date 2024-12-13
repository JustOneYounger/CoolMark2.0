#pragma once
#include "CoolMarkInclude.h"

class CoolMarkMarkDownEditerHighlight : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    CoolMarkMarkDownEditerHighlight(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QRegularExpression Title_Expression;
    QTextCharFormat Title_Format;

    QRegularExpression A_Expression;
    QTextCharFormat A_Format;

    QRegularExpression Import_Expression;
    QTextCharFormat Import_Format;

    QRegularExpression Bold_Expression;
    QTextCharFormat Bold_Format;

    QRegularExpression Italic_Expression;
    QTextCharFormat Italic_Format;

    QRegularExpression Delete_Expression;
    QTextCharFormat Delete_Format;

    QRegularExpression Footnote_Expression;
    QTextCharFormat Footnote_Format;
};

class CoolMarkMarkDownEditer :public QPlainTextEdit
{
    Q_OBJECT
public:
    CoolMarkMarkDownEditer(QWidget* parent = nullptr);
    ~CoolMarkMarkDownEditer();

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
    QWidget* markdownediterlinenumberarea;
    CoolMarkMarkDownEditerHighlight* markdownediterhighlight;

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QMenu* mouseMenu;
    QMenu* insert;
    QMenu* Heading;

    QAction* insertH1;
    QAction* insertH2;
    QAction* insertH3;
    QAction* insertH4;
    QAction* insertH5;
    QAction* insertH6;

    QAction* insertBold;
    QAction* insertItalic;
    QAction* insertBlockquote;
    QAction* insertOrderedList;
    QAction* insertUnorderedList;
    QAction* insertCode;
    QAction* insertHorizontalRule;
    QAction* insertLink;
    QAction* insertImage;
    QAction* insertCodeBlock;
    QAction* insertTable;
    QAction* insertFootnote;
    QAction* insertDefinitionlist;
    QAction* insertStrikethrough;
    QAction* insertTasklist;
};

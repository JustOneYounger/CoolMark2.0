#include "CoolMarkMarkDownEditer.h"

class CoolMarkMarkDownEditerLineNumber : public QWidget
{
public:
    CoolMarkMarkDownEditerLineNumber(CoolMarkMarkDownEditer* editer) :QWidget(editer), markdownediter(editer) {};
    QSize sizeHint() const override { return QSize(markdownediter->lineNumberAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent* event) override { markdownediter->lineNumberAreaPaintEvent(event); }
private:
    CoolMarkMarkDownEditer* markdownediter;
};

CoolMarkMarkDownEditerHighlight::CoolMarkMarkDownEditerHighlight(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    Title_Expression = QRegularExpression("^#{1,6} .*");
    Title_Format.setForeground(QColor("#FD971F"));

    A_Expression = QRegularExpression(R"(\[.*\]\(.*\)|\!\[.*\]\(.*\)|<.*>)");
    A_Format.setForeground(QColor("#66D9EF"));

    Import_Expression = QRegularExpression("^>+ .*");
    Import_Format.setForeground(QColor("#F92672"));

    Bold_Expression = QRegularExpression(R"(\*\*.*?\*\*|\_\_.*?\_\_)");
    Bold_Format.setFontWeight(1000);
    Bold_Format.setForeground(QColor("#C2D94C"));

    Italic_Expression = QRegularExpression(R"(\*.*?\*|\_.*?\_)");
    Italic_Format.setFontItalic(true);
    Italic_Format.setForeground(QColor("#C2D94C"));

    Delete_Expression = QRegularExpression(R"(\~\~.*?\~\~)");
    Delete_Format.setForeground(QColor("#5C6773"));

    Footnote_Expression = QRegularExpression(R"(\[\^\d+\])");
    Footnote_Format.setForeground(QColor("#9B59B6"));
}

void CoolMarkMarkDownEditerHighlight::highlightBlock(const QString& text) {
    QRegularExpressionMatchIterator match_Title_Iterator = Title_Expression.globalMatch(text);
    while (match_Title_Iterator.hasNext()) {
        QRegularExpressionMatch match_Title = match_Title_Iterator.next();
        setFormat(match_Title.capturedStart(), match_Title.capturedLength(), Title_Format);
    }
    QRegularExpressionMatchIterator match_A_Iterator = A_Expression.globalMatch(text);
    while (match_A_Iterator.hasNext()) {
        QRegularExpressionMatch match_A = match_A_Iterator.next();
        setFormat(match_A.capturedStart(), match_A.capturedLength(), A_Format);
    }
    QRegularExpressionMatchIterator match_Import_Iterator = Import_Expression.globalMatch(text);
    while (match_Import_Iterator.hasNext()) {
        QRegularExpressionMatch match_Import = match_Import_Iterator.next();
        setFormat(match_Import.capturedStart(), match_Import.capturedLength(), Import_Format);
    }
    QRegularExpressionMatchIterator match_Bold_Iterator = Bold_Expression.globalMatch(text);
    while (match_Bold_Iterator.hasNext()) {
        QRegularExpressionMatch match_Bold = match_Bold_Iterator.next();
        setFormat(match_Bold.capturedStart(), match_Bold.capturedLength(), Bold_Format);
    }
    QRegularExpressionMatchIterator match_Italic_Iterator = Italic_Expression.globalMatch(text);
    while (match_Italic_Iterator.hasNext()) {
        QRegularExpressionMatch match_Italic = match_Italic_Iterator.next();
        setFormat(match_Italic.capturedStart(), match_Italic.capturedLength(), Italic_Format);
    }
    QRegularExpressionMatchIterator match_Delete_Iterator = Delete_Expression.globalMatch(text);
    while (match_Delete_Iterator.hasNext()) {
        QRegularExpressionMatch match_Delete = match_Delete_Iterator.next();
        setFormat(match_Delete.capturedStart(), match_Delete.capturedLength(), Delete_Format);
    }
    QRegularExpressionMatchIterator match_Footnote_Iterator = Footnote_Expression.globalMatch(text);
    while (match_Footnote_Iterator.hasNext()) {
        QRegularExpressionMatch match_Footnote = match_Footnote_Iterator.next();
        setFormat(match_Footnote.capturedStart(), match_Footnote.capturedLength(), Footnote_Format);
    }
}

CoolMarkMarkDownEditer::CoolMarkMarkDownEditer(QWidget* parent)
	:QPlainTextEdit(parent)
	,markdownediterlinenumberarea(new CoolMarkMarkDownEditerLineNumber(this))
{
    markdownediterhighlight = new CoolMarkMarkDownEditerHighlight(document());
    setStyleSheet("background:transparent;color:#E6DB74;font-family:'Consolas';font-size:16px;border:none;");
    setTabStopDistance(fontMetrics().horizontalAdvance(" ") * 8);
    setWordWrapMode(QTextOption::NoWrap);
    //左侧行号渲染
    connect(this, &CoolMarkMarkDownEditer::blockCountChanged, this, &CoolMarkMarkDownEditer::updateLineNumberAreaWidth);
    connect(this, &CoolMarkMarkDownEditer::updateRequest, this, &CoolMarkMarkDownEditer::updateLineNumberArea);
    connect(this, &CoolMarkMarkDownEditer::cursorPositionChanged, this, &CoolMarkMarkDownEditer::highlightCurrentLine);
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    this->setPlaceholderText("Ctrl+O Open Markdown file\n"
        "Ctrl+S Save Markdown file\n"
        "Ctrl+H Jump to Markdown syntax help\n"
        "Ctrl+R Preview Markdown to HTML conversion\n"
        "F10    Export Markdown to PDF\n"
        "\n"
        "@[keyword]+Insert shortcut key combination to quickly insert a specified fragment in the Markdown editor\n"
        "[keyword] list\n"
        "@h1        Level 1 Title\n"
        "@h2        Level 2 Title\n"
        "@h3        Level 3 Title\n"
        "@h4        Level 4 Title\n"
        "@h5        Level 5 Title\n"
        "@h6        Level 6 Title\n"
        "@list      Default unordered list\n"
        "@ulist     Unordered list\n"
        "@olist     Ordered List\n"
        "@c1        Code\n"
        "@c2        Code Block\n"
        "@link      Link\n"
        "@image     Image\n"
        "@table     Table\n"
        "@footnote  Footnote\n"
        "@deflist   Definition List\n"
        "@tasklist  Task List\n"
        "\n"
        "Thank you for using it.\n"
        "We look forward to you joining CoolMark's production team.\n"
        "Welcome to join and discussion.\n"
        "\n"
        "Σ＞―(灬⁺д⁺灬)♡―――＞     ✧(≖ ◡ ≖✿)"
    );
}

CoolMarkMarkDownEditer::~CoolMarkMarkDownEditer() {
    delete markdownediterlinenumberarea;
    delete markdownediterhighlight;
    delete mouseMenu;
    delete insert;
    delete Heading;
    delete insertH1;
    delete insertH2;
    delete insertH3;
    delete insertH4;
    delete insertH5;
    delete insertH6;
    delete insertBold;
    delete insertItalic;
    delete insertBlockquote;
    delete insertOrderedList;
    delete insertUnorderedList;
    delete insertCode;
    delete insertHorizontalRule;
    delete insertLink;
    delete insertImage;
    delete insertCodeBlock;
    delete insertTable;
    delete insertFootnote;
    delete insertDefinitionlist;
    delete insertStrikethrough;
    delete insertTasklist;
}

int CoolMarkMarkDownEditer::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}
void CoolMarkMarkDownEditer::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void CoolMarkMarkDownEditer::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        markdownediterlinenumberarea->scroll(0, dy);
    else
        markdownediterlinenumberarea->update(0, rect.y(), markdownediterlinenumberarea->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
void CoolMarkMarkDownEditer::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    markdownediterlinenumberarea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CoolMarkMarkDownEditer::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        // QColor lineColor = QColor("#ECF5FF");
        // selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}
void CoolMarkMarkDownEditer::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(markdownediterlinenumberarea);
    // painter.fillRect(event->rect(), Qt::white);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    int currentLineNumber = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber == currentLineNumber) {
                painter.setPen(QColor("#E6DB74"));
                QFont font = painter.font();
                font.setBold(true);
                painter.setFont(font);
            }
            else {
                painter.setPen(QColor("#75715E"));
                QFont font = painter.font();
                font.setBold(false);
                painter.setFont(font);
            }
            painter.drawText(0, top, markdownediterlinenumberarea->width(), fontMetrics().height(),
                Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CoolMarkMarkDownEditer::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Insert) {
        QString text = this->toPlainText();
        text.replace(QRegularExpression("@h1"), "# Level1");
        text.replace(QRegularExpression("@h2"), "## Level2");
        text.replace(QRegularExpression("@h3"), "### Level3");
        text.replace(QRegularExpression("@h4"), "#### Level4");
        text.replace(QRegularExpression("@h5"), "##### Level5");
        text.replace(QRegularExpression("@h6"), "###### Level6");
        text.replace(QRegularExpression("@list"), "- ListItem1\n- ListItem2\n- ListItem3");
        text.replace(QRegularExpression("@olist"), "1. ListItem1\n2. ListItem2\n3. ListItem3");
        text.replace(QRegularExpression("@ulist"), "- ListItem1\n- ListItem2\n- ListItem3");
        text.replace(QRegularExpression("@link"), "[title](https://www.example.com)");
        text.replace(QRegularExpression("@image"), "![alt text](image.jpg)");
        text.replace(QRegularExpression("@c1"), "`code`");
        text.replace(QRegularExpression("@c2"), "```\nThis is a Fenced Code Block\nYou can input anything you want\n```");
        text.replace(QRegularExpression("@table"), "| col1 | col2 | col3 |\n| ---- | ---- | ---- |\n| row1 | row1 | row1 |\n| row2 | row2 | row2 |\n| row3 | row3 | row3 |");
        text.replace(QRegularExpression("@footnote"), "Here's a sentence with a footnote[^1]\n[^1]: This is the footnote");
        text.replace(QRegularExpression("@deflist"), "term\n: definition");
        text.replace(QRegularExpression("@tasklist"), "- [x] FirstItem\n- [ ] SecondItem");
        this->setPlainText(text);
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
    }
    else {
        QPlainTextEdit::keyPressEvent(e);
    }
}

void CoolMarkMarkDownEditer::contextMenuEvent(QContextMenuEvent* event) {
    mouseMenu = createStandardContextMenu();
    insert = new QMenu("Insert",mouseMenu);
    Heading = new QMenu("标题",insert);
    mouseMenu->insertMenu(mouseMenu->actions().first(), insert);
    insert->addMenu(Heading);
    insertH1 = new QAction("标题1", Heading);
    insertH2 = new QAction("标题2", Heading);
    insertH3 = new QAction("标题3", Heading);
    insertH4 = new QAction("标题4", Heading);
    insertH5 = new QAction("标题5", Heading);
    insertH6 = new QAction("标题6", Heading);

    Heading->addAction(insertH1);
    Heading->addAction(insertH2);
    Heading->addAction(insertH3);
    Heading->addAction(insertH4);
    Heading->addAction(insertH5);
    Heading->addAction(insertH6);

    insertBold = new QAction("粗体", insert);
    insertItalic = new QAction("斜体", insert);
    insertBlockquote = new QAction("引用块", insert);
    insertOrderedList = new QAction("有序列表", insert);
    insertUnorderedList = new QAction("无序列表", insert);
    insertCode = new QAction("代码", insert);
    insertHorizontalRule = new QAction("分隔线", insert);
    insertLink = new QAction("链接", insert);
    insertImage = new QAction("图片", insert);
    insertCodeBlock = new QAction("代码块", insert);
    insertTable = new QAction("表格", insert);
    insertFootnote = new QAction("脚注", insert);
    insertDefinitionlist = new QAction("自定义列表", insert);
    insertStrikethrough = new QAction("删除线", insert);
    insertTasklist = new QAction("任务列表", insert);

    insert->addAction(insertBold);
    insert->addAction(insertItalic);
    insert->addAction(insertBlockquote);
    insert->addAction(insertOrderedList);
    insert->addAction(insertUnorderedList);
    insert->addAction(insertCode);
    insert->addAction(insertHorizontalRule);
    insert->addAction(insertLink);
    insert->addAction(insertImage);
    insert->addAction(insertCodeBlock);
    insert->addAction(insertTable);
    insert->addAction(insertFootnote);
    insert->addAction(insertDefinitionlist);
    insert->addAction(insertStrikethrough);
    insert->addAction(insertTasklist);

    connect(insertH1, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("# Level1\n");
        setTextCursor(cursor);
        });

    connect(insertH2, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("## Level2\n");
        setTextCursor(cursor);
        });

    connect(insertH3, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("### Level3\n");
        setTextCursor(cursor);
        });

    connect(insertH4, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("#### Level4\n");
        setTextCursor(cursor);
        });

    connect(insertH5, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("##### Level5\n");
        setTextCursor(cursor);
        });

    connect(insertH6, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("###### Level6\n");
        setTextCursor(cursor);
        });

    connect(insertBold, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("**Bold**\n");
        setTextCursor(cursor);
        });

    connect(insertItalic, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("*Italic*\n");
        setTextCursor(cursor);
        });

    connect(insertBlockquote, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("> Blockquote\n");
        setTextCursor(cursor);
        });

    connect(insertOrderedList, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("1. ListItem\n2. ListItem\n3. ListItem\n");
        setTextCursor(cursor);
        });

    connect(insertUnorderedList, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("- ListItem\n- ListItem\n- ListItem\n");
        setTextCursor(cursor);
        });

    connect(insertCode, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("`Code`\n");
        setTextCursor(cursor);
        });

    connect(insertHorizontalRule, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("---\n");
        setTextCursor(cursor);
        });

    connect(insertLink, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("[title](https://www.example.com)\n");
        setTextCursor(cursor);
        });

    connect(insertImage, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("![alt text](Image.jpg)\n");
        setTextCursor(cursor);
        });

    connect(insertCodeBlock, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("```\nThis is a Fenced Code Block\nYou can input anything you want\n```\n");
        setTextCursor(cursor);
        });

    connect(insertTable, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("| Col1 | Col2 | Col3 |\n| ---- | ---- | ---- |\n| Row1 | Row1 | Row1 |\n| Row2 | Row2 | Row2 |\n| Row3 | Row3 | Row3 |\n");
        setTextCursor(cursor);
        });

    connect(insertFootnote, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("Here's a sentence with a footnote[^1]\n[^1]:This is the footnote\n");
        setTextCursor(cursor);
        });

    connect(insertDefinitionlist, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("Term\n: Definition\n");
        setTextCursor(cursor);
        });

    connect(insertStrikethrough, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("~~DeleteText~~\n");
        setTextCursor(cursor);
        });

    connect(insertTasklist, &QAction::triggered, this, [this]() {
        QTextCursor cursor = textCursor();
        cursor.insertBlock();
        cursor.insertText("- [x] FirstItem\n- [ ] SecondItem\n- [ ] ThirdItem\n");
        setTextCursor(cursor);
        });

    mouseMenu->exec(event->globalPos());
}
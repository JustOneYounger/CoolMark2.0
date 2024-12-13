#include "CoolMarkCssEditer.h"

class CoolMarkCssEditerLineNumber : public QWidget
{
public:
    CoolMarkCssEditerLineNumber(CoolMarkCssEditer* editer) :QWidget(editer), cssediter(editer) {};
    QSize sizeHint() const override { return QSize(cssediter->lineNumberAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent* event) override { cssediter->lineNumberAreaPaintEvent(event); }
private:
    CoolMarkCssEditer* cssediter;
};

CoolMarkCssEditerHighlight::CoolMarkCssEditerHighlight(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    Class_Expression = QRegularExpression(R"((.*){)");
    Class_Format.setForeground(QColor("#F92672"));

    Attribute_Expression = QRegularExpression(R"((?:\s*)([a-zA-Z0-9\-]*):.*;)");
    Attribute_Format.setForeground(QColor("#66D9EF"));

    Comment_StartExpression= QRegularExpression("/\\*");
    Comment_EndExpression = QRegularExpression("\\*/");
    Comment_Format.setForeground(QColor("#75715E"));
}

void CoolMarkCssEditerHighlight::highlightBlock(const QString& text) {
    QRegularExpressionMatchIterator match_Class_Iterator = Class_Expression.globalMatch(text);
    while (match_Class_Iterator.hasNext()) {
        QRegularExpressionMatch match_Class = match_Class_Iterator.next();
        setFormat(match_Class.capturedStart(1), match_Class.capturedLength(1), Class_Format);
    }

    QRegularExpressionMatchIterator match_Attribute_Iterator = Attribute_Expression.globalMatch(text);
    while (match_Attribute_Iterator.hasNext()) {
        QRegularExpressionMatch match_Attribute = match_Attribute_Iterator.next();
        setFormat(match_Attribute.capturedStart(1), match_Attribute.capturedLength(1), Attribute_Format);
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)startIndex = text.indexOf(Comment_StartExpression);
    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch;
        int endIndex = text.indexOf(Comment_EndExpression, startIndex, &endMatch);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, Comment_Format);
        startIndex = text.indexOf(Comment_StartExpression,startIndex + commentLength);
    }
}

CoolMarkCssEditer::CoolMarkCssEditer(QWidget* parent)
    : QPlainTextEdit(parent)
    , cssediterlinenumberarea(new CoolMarkCssEditerLineNumber(this))
{
    cssediterhighlight = new CoolMarkCssEditerHighlight(document());
    setStyleSheet("background:transparent;color:#E6DB74;font-family:'Consolas';font-size:14px;border:none;");
    setTabStopDistance(fontMetrics().horizontalAdvance(" ") * 8);
    setWordWrapMode(QTextOption::NoWrap);
    //左侧行号渲染
    connect(this, &CoolMarkCssEditer::blockCountChanged, this, &CoolMarkCssEditer::updateLineNumberAreaWidth);
    connect(this, &CoolMarkCssEditer::updateRequest, this, &CoolMarkCssEditer::updateLineNumberArea);
    connect(this, &CoolMarkCssEditer::cursorPositionChanged, this, &CoolMarkCssEditer::highlightCurrentLine);
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CoolMarkCssEditer::~CoolMarkCssEditer() {
    delete cssediterlinenumberarea;
    delete cssediterhighlight;
}

int CoolMarkCssEditer::lineNumberAreaWidth()
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
void CoolMarkCssEditer::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void CoolMarkCssEditer::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        cssediterlinenumberarea->scroll(0, dy);
    else
        cssediterlinenumberarea->update(0, rect.y(), cssediterlinenumberarea->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
void CoolMarkCssEditer::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    cssediterlinenumberarea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CoolMarkCssEditer::highlightCurrentLine()
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
void CoolMarkCssEditer::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(cssediterlinenumberarea);
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
            painter.drawText(0, top, cssediterlinenumberarea->width(), fontMetrics().height(),
                Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CoolMarkCssEditer::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_ParenLeft || e->key() == Qt::Key_BraceLeft || e->key() == Qt::Key_BracketLeft) {
        QTextCursor cursor = textCursor();
        if (e->key() == Qt::Key_ParenLeft) {
            cursor.insertText("()");
        }
        else if (e->key() == Qt::Key_BraceLeft) {
            cursor.insertText("{}");
        }
        else {
            cursor.insertText("[]");
        }
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
    }
    else if (e->key() == Qt::Key_ParenRight || e->key() == Qt::Key_BraceRight || e->key() == Qt::Key_BracketRight) {
        QTextCursor cursor = textCursor();

        QTextCursor nextChar_cursor = textCursor();
        nextChar_cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QString nextChar = nextChar_cursor.selectedText();

        if (e->key() == Qt::Key_ParenRight && nextChar == ")") {
            cursor.movePosition(QTextCursor::Right);
            setTextCursor(cursor);
        }
        else if (e->key() == Qt::Key_BraceRight && nextChar == "}") {
            cursor.movePosition(QTextCursor::Right);
            setTextCursor(cursor);
        }
        else if (e->key() == Qt::Key_BracketRight && nextChar == "]") {
            cursor.movePosition(QTextCursor::Right);
            setTextCursor(cursor);
        }
        else {
            cursor.insertText(e->text());
            setTextCursor(cursor);
        }
    }
    else if (e->text() == "'" || e->text() == "\"") {
        QTextCursor cursor_select = textCursor();
        QTextCursor cursor_work = textCursor();
        if (!cursor_select.atEnd()) {
            cursor_select.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
            QString select = cursor_select.selectedText();
            if (select == "'" && e->text() == "'") {
                cursor_work.movePosition(QTextCursor::Right);
                setTextCursor(cursor_work);
            }
            else if (select == "\"" && e->text() == "\"") {
                cursor_work.movePosition(QTextCursor::Right);
                setTextCursor(cursor_work);
            }
            else if (select != "'" && e->text() == "'") {
                cursor_work.insertText("''");
                cursor_work.movePosition(QTextCursor::Left);
                setTextCursor(cursor_work);
            }
            else if (select != "\"" && e->text() == "\"") {
                cursor_work.insertText("\"\"");
                cursor_work.movePosition(QTextCursor::Left);
                setTextCursor(cursor_work);
            }
        }
        else {
            if (e->text() == "'") {
                cursor_work.insertText("''");
                cursor_work.movePosition(QTextCursor::Left);
                setTextCursor(cursor_work);
            }
            else if (e->text() == "\"") {
                cursor_work.insertText("\"\"");
                cursor_work.movePosition(QTextCursor::Left);
                setTextCursor(cursor_work);
            }
        }
    }
    else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        QString text = cursor.block().text();
        QTextCursor cursor_select_left = textCursor();
        QTextCursor cursor_select_right = textCursor();
        cursor_select_left.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        cursor_select_right.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QString selectedChar_left = cursor_select_left.selectedText();
        QString selectedChar_right = cursor_select_right.selectedText();
        if (selectedChar_left == "{" && selectedChar_right == "}") {
            cursor.insertBlock();
            cursor.insertText("\t\n");
            cursor.movePosition(QTextCursor::Left);
            setTextCursor(cursor);
        }
        else {
            if (text.startsWith("\t")) {
                cursor.insertBlock();
                cursor.insertText("\t");
                setTextCursor(cursor);
            }
            else {
                cursor.insertBlock();
                setTextCursor(cursor);
            }
        }
    }
    else if (e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Slash) {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection()) {
            int selectionStart = cursor.selectionStart();
            int selectionEnd = cursor.selectionEnd();
            QTextCursor startCursor = textCursor();
            QTextCursor endCursor = textCursor();
            startCursor.setPosition(selectionStart);
            endCursor.setPosition(selectionEnd);

            startCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 3);
            endCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 3);

            QString startText = startCursor.selectedText();
            QString endText = endCursor.selectedText();
            if (startText == "/* " && endText == " */" ) {
                startCursor.removeSelectedText();
                endCursor.removeSelectedText();
                endCursor.movePosition(QTextCursor::Down);
                setTextCursor(endCursor);
            }
            else {
                QTextCursor WorkstartCursor = textCursor();
                QTextCursor WorkendCursor = textCursor();
                WorkstartCursor.setPosition(selectionStart);
                WorkendCursor.setPosition(selectionEnd);
                WorkstartCursor.insertText("/* ");
                WorkendCursor.insertText(" */");
                WorkendCursor.movePosition(QTextCursor::Down);
                setTextCursor(WorkendCursor);
            }
        }
        else {
            QString text = cursor.block().text();
            if (text.startsWith("/*") && text.endsWith("*/")) {
                QTextCursor startCursor = textCursor();
                QTextCursor endCursor = textCursor();

                startCursor.movePosition(QTextCursor::StartOfBlock);
                startCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2);
                startCursor.removeSelectedText();

                endCursor.movePosition(QTextCursor::EndOfBlock);
                endCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 2);
                endCursor.removeSelectedText();
                setTextCursor(cursor);
            }
            else {
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.insertText("/*");
                cursor.movePosition(QTextCursor::EndOfBlock);
                cursor.insertText("*/");
                setTextCursor(cursor);
            }
        }
    }
    else {
        QPlainTextEdit::keyPressEvent(e);
    }
}
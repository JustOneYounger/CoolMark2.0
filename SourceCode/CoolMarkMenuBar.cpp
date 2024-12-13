#include "CoolMarkMenuBar.h"

CoolMarkMenuBar::CoolMarkMenuBar(QWidget* parent)
	: QMenuBar(parent)
{
	this->setObjectName("CoolMarkMenu");
	this->setStyleSheet(R"(
	#CoolMarkMenu{
	background:#363637;
	color:white;
	font-family:'Consolas';
	font-size:14px;
	}
	#CoolMarkMenu::item:selected{
	background:#1D1D1D;
	color:#409EFF;
	}
	)");
	parentWindow = this->parentWidget();
	parentCoolMarkMarkDownEditer = parentWindow->findChild<CoolMarkMarkDownEditer*>("MarkDownEditer");
	parentLabelFileName = parentWindow->findChild<QLabel*>("label_filename");
	parentLineEditStatus = parentWindow->findChild<QLineEdit*>("lineEdit_status");
	parentQTabWidget = parentWindow->findChild<QTabWidget*>("tabWidget");
	initMenu();
	setconnect();
}

CoolMarkMenuBar::~CoolMarkMenuBar()
{
	delete parentCoolMarkMarkDownEditer;
	delete parentLabelFileName;
	delete parentQTabWidget;
}

void CoolMarkMenuBar::initMenu() {
	File = new QMenu("文件");
	Edit = new QMenu("编辑");
	Window = new QMenu("窗体");
	Help = new QMenu("帮助");

	this->addMenu(File);
	this->addMenu(Edit);
	this->addMenu(Window);
	this->addMenu(Help);

	OpenFile = new QAction("打开Markdown文件", this); 
	SaveFile = new QAction("保存Markdown文件", this);
	SaveAnotherFile = new QAction("另存为Markdown文件", this);

	File->addAction(OpenFile);
	File->addAction(SaveFile);
	File->addAction(SaveAnotherFile);

	Heading = new QMenu("标题");
	Edit->addMenu(Heading);

	insertH1 = new QAction("标题1", this);
	insertH2 = new QAction("标题2", this);
	insertH3 = new QAction("标题3", this);
	insertH4 = new QAction("标题4", this);
	insertH5 = new QAction("标题5", this);
	insertH6 = new QAction("标题6", this);

	Heading->addAction(insertH1);
	Heading->addAction(insertH2);
	Heading->addAction(insertH3);
	Heading->addAction(insertH4);
	Heading->addAction(insertH5);
	Heading->addAction(insertH6);

	insertBold = new QAction("粗体", this);
	insertItalic = new QAction("斜体", this);
	insertBlockquote = new QAction("引用块", this);
	insertOrderedList = new QAction("有序列表", this);
	insertUnorderedList = new QAction("无序列表", this);
	insertCode = new QAction("代码", this);
	insertHorizontalRule = new QAction("分隔线", this);
	insertLink = new QAction("链接", this);
	insertImage = new QAction("图片", this);
	insertCodeBlock = new QAction("代码块", this);
	insertTable = new QAction("表格", this);
	insertFootnote = new QAction("脚注", this);
	insertDefinitionlist = new QAction("自定义列表", this);
	insertStrikethrough = new QAction("删除线", this);
	insertTasklist = new QAction("任务列表", this);

	Edit->addAction(insertBold);
	Edit->addAction(insertItalic);
	Edit->addAction(insertBlockquote);
	Edit->addAction(insertOrderedList);
	Edit->addAction(insertUnorderedList);
	Edit->addAction(insertCode);
	Edit->addAction(insertHorizontalRule);
	Edit->addAction(insertLink);
	Edit->addAction(insertImage);
	Edit->addAction(insertCodeBlock);
	Edit->addAction(insertTable);
	Edit->addAction(insertFootnote);
	Edit->addAction(insertDefinitionlist);
	Edit->addAction(insertStrikethrough);
	Edit->addAction(insertTasklist);

	minWindow = new QAction("最小", this);
	maxWindow = new QAction("最大", this);
	fullWindow = new QAction("全屏", this);
	quitWindow = new QAction("退出", this);
	Window->addAction(minWindow);
	Window->addAction(maxWindow);
	Window->addAction(fullWindow);
	Window->addSeparator();
	Window->addAction(quitWindow);

	Markdownhelp = new QAction("软件Markdown语法参考", this);
	OnlineMarkDown = new QAction("在线MarkDown语法参考", this);
	CssHelp = new QAction("Css帮助", this);
	Help->addAction(Markdownhelp);
	Help->addAction(OnlineMarkDown);
	Help->addAction(CssHelp);
}

void CoolMarkMenuBar::setconnect() {
	connect(minWindow, &QAction::triggered, this, [this]() {
		parentWindow->showMinimized();
		});

	connect(maxWindow, &QAction::triggered, this, [this]() {
		parentWindow->showMaximized();
		});

	connect(fullWindow, &QAction::triggered, this, [this]() {
		parentWindow->showFullScreen();
		});

	connect(quitWindow, &QAction::triggered, this, [this]() {
		exit(0);
		});

	connect(OpenFile, &QAction::triggered, this, [this]() {
		QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open MarkDown File"), "", tr("MarkDown File(*.md *.markdown *.mdown *.mkd *.mkdn)"));
		if (!filename.isEmpty()) {
			parentLabelFileName->setText(filename);
			parentCoolMarkMarkDownEditer->clear();
			QFile markdownfile(filename);
			if (markdownfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream read_markdown(&markdownfile);
				QString markdown_context = read_markdown.readAll();
				parentCoolMarkMarkDownEditer->insertPlainText(markdown_context);
				markdownfile.close();
			}
			parentLineEditStatus->setText("已打开");
		}
		});

	connect(SaveFile, &QAction::triggered, this, [this]() {
		if (!parentLabelFileName->text().isEmpty()) {
			QString filepath = parentLabelFileName->text();
			QFile markdownfile(filepath);
			if (markdownfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream save_markdown(&markdownfile);
				QString markdown_context = parentCoolMarkMarkDownEditer->toPlainText();
				save_markdown << markdown_context;
				markdownfile.close();
			}
			parentLineEditStatus->setText("已保存");
		}
		else {
			QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save MarkDown File"), "NewMarkDownFile.md", tr("MarkDown File(*.md *.markdown *.mdown *.mkd *.mkdn)"));
			if (!filename.isEmpty()) {
				parentLabelFileName->setText(filename);
				QFile markdownfile(filename);
				if (markdownfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
					QTextStream save_markdown(&markdownfile);
					QString markdown_context = parentCoolMarkMarkDownEditer->toPlainText();
					save_markdown << markdown_context;
					markdownfile.close();
				}
				parentLineEditStatus->setText("已保存");
			}
		}
		});

	connect(SaveAnotherFile, &QAction::triggered, this, [this]() {
		QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save MarkDown File"), "NewMarkDownFile.md", tr("MarkDown File(*.md *.markdown *.mdown *.mkd *.mkdn)"));
		if (!filename.isEmpty()) {
			parentLabelFileName->setText(filename);
			QFile markdownfile(filename);
			if (markdownfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream save_markdown(&markdownfile);
				QString markdown_context = parentCoolMarkMarkDownEditer->toPlainText();
				save_markdown << markdown_context;
				markdownfile.close();
			}
			parentLineEditStatus->setText("已保存");
		}
		});

	connect(insertH1, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("# Level1\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertH2, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("## Level2\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertH3, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("### Level3\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertH4, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("#### Level4\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertH5, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("##### Level5\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertH6, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("###### Level6\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertBold, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("**Bold**\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertItalic, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("*Italic*\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertBlockquote, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("> Blockquote\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertOrderedList, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("1. ListItem\n2. ListItem\n3. ListItem\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertUnorderedList, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("- ListItem\n- ListItem\n- ListItem\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertCode, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("`Code`\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertHorizontalRule, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("---\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertLink, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("[title](https://www.example.com)\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertImage, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("![alt text](Image.jpg)\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertCodeBlock, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("```\nThis is a Fenced Code Block\nYou can input anything you want\n```\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertTable, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("| Col1 | Col2 | Col3 |\n| ---- | ---- | ---- |\n| Row1 | Row1 | Row1 |\n| Row2 | Row2 | Row2 |\n| Row3 | Row3 | Row3 |\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertFootnote, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("Here's a sentence with a footnote[^1]\n[^1]:This is the footnote\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertDefinitionlist, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("Term\n: Definition\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertStrikethrough, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("~~DeleteText~~\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(insertTasklist, &QAction::triggered, this, [this]() {
		QTextCursor cursor = parentCoolMarkMarkDownEditer->textCursor();
		cursor.insertBlock();
		cursor.insertText("- [x] FirstItem\n- [ ] SecondItem\n- [ ] ThirdItem\n");
		parentCoolMarkMarkDownEditer->setTextCursor(cursor);
		});

	connect(Markdownhelp, &QAction::triggered, this, [this]() {
		parentQTabWidget->setCurrentIndex(1);
		});

	connect(OnlineMarkDown, &QAction::triggered, this, [this]() {
		QDesktopServices::openUrl(QUrl("https://markdown.com.cn/"));
		});

	connect(CssHelp, &QAction::triggered, this, [this]() {
		QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath()+"/csss手册4.2.3.chm"));
		});
}
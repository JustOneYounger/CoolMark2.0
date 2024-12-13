#pragma once
#include "CoolMarkInclude.h"
#include "CoolMarkMarkDownEditer.h"

class CoolMarkMenuBar : public QMenuBar {
	Q_OBJECT
public:
	CoolMarkMenuBar(QWidget* parent = nullptr);
	~CoolMarkMenuBar();

	QWidget* parentWindow;

public:
	void initMenu();
	void setconnect();
	CoolMarkMarkDownEditer* parentCoolMarkMarkDownEditer;
	QLabel* parentLabelFileName;
	QLineEdit* parentLineEditStatus;
	QTabWidget* parentQTabWidget;

private:
	QMenu* File;
	QMenu* Edit;
	QMenu* Window;
	QMenu* Help;

	QAction* OpenFile;
	QAction* SaveFile;
	QAction* SaveAnotherFile;

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

	QAction* minWindow;
	QAction* maxWindow;
	QAction* fullWindow;
	QAction* quitWindow;

	QAction* Markdownhelp;
	QAction* OnlineMarkDown;
	QAction* CssHelp;
};
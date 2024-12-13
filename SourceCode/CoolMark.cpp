#include "CoolMark.h"

CoolMark::CoolMark(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CoolMarkClass())
    , markdownediter(new CoolMarkMarkDownEditer(this))
    , cssediter(new CoolMarkCssEditer(this))
{
    ui->setupUi(this);
    BasicWindow();
    this->setWindowIcon(QIcon(MainPath + "/markdown.ico"));
    
    InitWindow();
    ShortcutWindow();
    outputdialog = new OutputPDF(this);
    
    connect(cssediter, &QPlainTextEdit::textChanged, this, [this]() {
        QFile cssFile(MainPath + "/style.css");
        if (cssFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream write_css(&cssFile);
            QString css = cssediter->toPlainText();
            write_css << css;
        }
        cssFile.close();
        });

    connect(markdownediter, &QPlainTextEdit::textChanged, this, [this]() {
        ui->lineEdit_status->setText("未保存");
        });

    connect(ui->lineEdit_status, &QLineEdit::textChanged, this, [this]() {
        if (ui->lineEdit_status->text() == "已导出" || ui->lineEdit_status->text() == "导出取消") {
            isOutput = false;
        }
        });
}

CoolMark::~CoolMark()
{
    QFile TempMarkdownFile(MainPath + "/temp.md");
    QFile TempHtmlFile(MainPath + "/temp.html");
    TempMarkdownFile.remove();
    TempHtmlFile.remove();
    delete converterThread;
    delete combinationThread;
    delete markdownediter;
    delete cssediter;
    delete outputdialog;
    delete ChangeHelp;
    delete ShortcutPreview;
    delete OpenMarkdown;
    delete SaveMarkdown;
    delete Output;
    delete ui;
}

void CoolMark::BasicWindow() {
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->showMaximized();
    connect(ui->pushButton_min, &QPushButton::clicked, this, [this]() {this->showMinimized();});
    connect(ui->pushButton_max, &QPushButton::clicked, this, [this]() {
        if (isMax == false) {
            isMax = true;
            this->showMaximized();
        }
        else {
            this->resize(1200, 700);
            isMax = false;
        }});
    connect(ui->pushButton_quit, &QPushButton::clicked, this, [this]() {this->close();});
}

void CoolMark::mousePressEvent(QMouseEvent* event) {if (ui->widget_headline->rect().contains(event->pos())) {isMove = true;MousePos = event->pos();}}
void CoolMark::mouseMoveEvent(QMouseEvent* event) {if (isMove) {move(pos() + event->pos() - MousePos);}}
void CoolMark::mouseReleaseEvent(QMouseEvent* event) {isMove = false;}

void CoolMark::InitWindow() {
    QWebEngineSettings* settings = ui->webEngineView_preview->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    settings->setAttribute(QWebEngineSettings::TouchIconsEnabled, true);
    settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    settings->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);

    ui->webEngineView_markdownhelp->load(QUrl::fromLocalFile(MainPath + "/MarkdownInformation.html"));

    QSplitter* SplitterEditer = new QSplitter(Qt::Horizontal);
    SplitterEditer->addWidget(ui->widget_markdown_editer);
    SplitterEditer->addWidget(ui->tabWidget);
    
    SplitterEditer->setStyleSheet("QSplitter::handle{background-color:#E6A23C;}");
    SplitterEditer->setHandleWidth(1);
    QList<int> sizes;
    sizes << 650 << 350;
    SplitterEditer->setSizes(sizes);

    QHBoxLayout* layout_splitter = new QHBoxLayout(ui->widget_work);
    layout_splitter->setContentsMargins(0, 0, 0, 0);
    layout_splitter->addWidget(SplitterEditer);

    QVBoxLayout* layout_MarkDownEditer = new QVBoxLayout(ui->widget_markdown_editer);
    layout_MarkDownEditer->setContentsMargins(0, 0, 0, 0);
    layout_MarkDownEditer->addWidget(markdownediter);
    markdownediter->setObjectName("MarkDownEditer");

    QVBoxLayout* layout_StyleEditer = new QVBoxLayout(ui->widget_css_editer);
    layout_StyleEditer->setContentsMargins(0, 0, 0, 0);
    layout_StyleEditer->addWidget(cssediter);

    mymenubar = new CoolMarkMenuBar(this);
    QVBoxLayout* layout_MenuBar = new QVBoxLayout(ui->widget_menubar);
    layout_MenuBar->setMenuBar(mymenubar);
    
    QFile CssFile(MainPath + "/style.css");
    if (CssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream read_css(&CssFile);
        QString css = read_css.readAll();
        cssediter->setPlainText(css);
        CssFile.close();
    }

    QFile TempMarkdownFile(MainPath + "/temp.md");
    if (TempMarkdownFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        TempMarkdownFile.close();
    }

    QFile TempHtmlFile(MainPath + "/temp.html");
    if (TempHtmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream init_html(&TempHtmlFile);
        init_html << "<link rel=\"stylesheet\" href=\"" + MainPath + "/style.css\">";
        TempHtmlFile.close();
        ui->webEngineView_preview->load(QUrl::fromLocalFile(MainPath + "/temp.html"));
    }

    converterThread = new MarkdownConverterThread(python_converter_path, MainPath + "/temp.md", MainPath + "/temp.html");
    combinationThread = new CombinationThread(MainPath + "/temp.html", MainPath + "/style.css");
}

void CoolMark::ShortcutWindow() {
    ChangeHelp = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(ChangeHelp, &QShortcut::activated, this, [this]() {
        ui->tabWidget->setCurrentWidget(ui->tab_help);
        });

    ShortcutPreview = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(ShortcutPreview, &QShortcut::activated, this, &CoolMark::Converter);

    OpenMarkdown = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(OpenMarkdown, &QShortcut::activated, this, &CoolMark::Open);

    SaveMarkdown = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(SaveMarkdown, &QShortcut::activated, this, &CoolMark::Save);

    Output = new QShortcut(QKeySequence("F10"), this);
    connect(Output, &QShortcut::activated, this, [this]() {
        ui->lineEdit_status->setText("写入中");
        isOutput = true;
        Converter();
        connect(combinationThread, &QThread::finished, this, [this]() {
            if (isOutput == true) {
                outputdialog->show();
                ui->lineEdit_status->setText("导出中");
            }
        });
    });
}

void CoolMark::Converter() {
    QFile TempMarkdownFile(MainPath + "/temp.md");
    if (TempMarkdownFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream markdown_write(&TempMarkdownFile);
        markdown_write << markdownediter->toPlainText();
        TempMarkdownFile.close();
    }
    converterThread->start();
    connect(converterThread, &QThread::finished, this, &CoolMark::Preview);
}

void CoolMark::Preview() {
    combinationThread->start();
    connect(combinationThread, &QThread::finished, this, [this]() {
        ui->webEngineView_preview->reload();
        ui->tabWidget->setCurrentWidget(ui->tab_preview);
        });
}

void CoolMark::Open() {
    QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open MarkDown File"), "", tr("MarkDown File(*.md *.markdown *.mdown *.mkd *.mkdn)"));
    if (!filename.isEmpty()) {
        ui->label_filename->setText(filename);
        markdownediter->clear();
        QFile markdownfile(filename);
        if (markdownfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream read_markdown(&markdownfile);
            QString markdown_context = read_markdown.readAll();
            markdownediter->insertPlainText(markdown_context);
            markdownfile.close();
        }
        ui->lineEdit_status->setText("已打开");
    }
}

void CoolMark::Save() {
    if (!ui->label_filename->text().isEmpty()) {
        QString filepath = ui->label_filename->text();
        QFile markdownfile(filepath);
        if (markdownfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream save_markdown(&markdownfile);
            QString markdown_context = markdownediter->toPlainText();
            save_markdown << markdown_context;
            markdownfile.close();
        }
        ui->lineEdit_status->setText("已保存");
    }
    else {
        QString filename = QFileDialog::getSaveFileName(nullptr, tr("Save MarkDown File"), "NewMarkDownFile.md", tr("MarkDown File(*.md *.markdown *.mdown *.mkd *.mkdn)"));
        if (!filename.isEmpty()) {
            ui->label_filename->setText(filename);
            QFile markdownfile(filename);
            if (markdownfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream save_markdown(&markdownfile);
                QString markdown_context = markdownediter->toPlainText();
                save_markdown << markdown_context;
                markdownfile.close();
            }
            ui->lineEdit_status->setText("已保存");
        }
    }
}
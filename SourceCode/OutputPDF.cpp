#include "OutputPDF.h"

OutputPDF::OutputPDF(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::OutputPDFClass())
{
	ui->setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	QPoint parentPos = parentWidget()->pos();
	QSize parentSize = parentWidget()->size();
	QSize childSize = this->size();
	int x = parentPos.x() + (parentSize.width() - childSize.width()) / 2;
	int y = parentPos.y() + (parentSize.height() - childSize.height()) / 2;
	this->move(x, y);

	setpagesizehash();

	parentWebEngineView = this->parentWidget()->findChild<QWebEngineView*>("webEngineView_preview");
	parentStatusLineEdit = this->parentWidget()->findChild<QLineEdit*>("lineEdit_status");

	connect(ui->toolButton_openfolder, &QToolButton::clicked, this, [this]() {
		QString folder=QFileDialog::getExistingDirectory(
			nullptr,
			tr("Save Directory"),
			QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
		);

		ui->lineEdit_savefolder->setText(folder);
	});

	connect(ui->pushButton_output, &QPushButton::clicked, this, &OutputPDF::Output);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, [this]() {
		parentStatusLineEdit->setText("导出取消");
		this->close();
	});
}

OutputPDF::~OutputPDF()
{
	delete ui;
}

void OutputPDF::setpagesizehash() {
	PageSizeHash = {
		{"A0",QPageSize(QPageSize::A0)},
		{"A1",QPageSize(QPageSize::A1)},
		{"A2",QPageSize(QPageSize::A2)},
		{"A3",QPageSize(QPageSize::A3)},
		{"A4",QPageSize(QPageSize::A4)},
		{"A5",QPageSize(QPageSize::A5)},
		{"A6",QPageSize(QPageSize::A6)},
		{"A7",QPageSize(QPageSize::A7)},
		{"A8",QPageSize(QPageSize::A8)},
		{"A9",QPageSize(QPageSize::A9)},
		{"A10",QPageSize(QPageSize::A10)},
		{"A3Extra",QPageSize(QPageSize::A3Extra)},
		{"A4Extra",QPageSize(QPageSize::A4Extra)},
		{"A4Plus",QPageSize(QPageSize::A4Plus)},
		{"A4Small",QPageSize(QPageSize::A4Small)},
		{"A5Extra",QPageSize(QPageSize::A5Extra)},
		{"B0",QPageSize(QPageSize::B0)},
		{"B1",QPageSize(QPageSize::B1)},
		{"B2",QPageSize(QPageSize::B2)},
		{"B3",QPageSize(QPageSize::B3)},
		{"B4",QPageSize(QPageSize::B4)},
		{"B5",QPageSize(QPageSize::B5)},
		{"B6",QPageSize(QPageSize::B6)},
		{"B7",QPageSize(QPageSize::B7)},
		{"B8",QPageSize(QPageSize::B8)},
		{"B9",QPageSize(QPageSize::B9)},
		{"B10",QPageSize(QPageSize::B10)},
		{"B5Extra",QPageSize(QPageSize::B5Extra)},
		{"JisB0",QPageSize(QPageSize::JisB0)},
		{"JisB1",QPageSize(QPageSize::JisB1)},
		{"JisB2",QPageSize(QPageSize::JisB2)},
		{"JisB3",QPageSize(QPageSize::JisB3)},
		{"JisB4",QPageSize(QPageSize::JisB4)},
		{"JisB5",QPageSize(QPageSize::JisB5)},
		{"JisB6",QPageSize(QPageSize::JisB6)},
		{"JisB7",QPageSize(QPageSize::JisB7)},
		{"JisB8",QPageSize(QPageSize::JisB8)},
		{"JisB9",QPageSize(QPageSize::JisB9)},
		{"JisB10",QPageSize(QPageSize::JisB10)}
	};
}

void OutputPDF::Output() {
	QString SaveFolder = ui->lineEdit_savefolder->text();
	
	if (SaveFolder.isEmpty()) {
		QMessageBox::warning(nullptr, "警告", "未指定保存的文件目录", QMessageBox::Ok);
		return;
	}
	QString SaveFileName = ui->lineEdit_filename->text();
	parentStatusLineEdit->setText("获取配置");
	QString PageSize = ui->comboBox_pagesize->currentText();
	QString PageOrientation = ui->comboBox_pageorientation->currentText();
	QString PageMode = ui->comboBox_mode->currentText();
	
	QPageLayout PDFLayout;
	PDFLayout.setPageSize(PageSizeHash.value(PageSize));
	PDFLayout.setTopMargin(0);
	PDFLayout.setBottomMargin(0);
	PDFLayout.setLeftMargin(0);
	PDFLayout.setRightMargin(0);
	if (PageOrientation == "纵向") {
		PDFLayout.setOrientation(QPageLayout::Portrait);
	}
	else if (PageOrientation == "横向") {
		PDFLayout.setOrientation(QPageLayout::Landscape);
	}
	if (PageMode == "FullPageMode") {
		PDFLayout.setMode(QPageLayout::FullPageMode);
	}
	else if (PageMode == "StandardMode") {
		PDFLayout.setMode(QPageLayout::StandardMode);
	}
	parentStatusLineEdit->setText("正在导出");
	ui->pushButton_output->setDisabled(true);
	QDir dir(SaveFolder);
	QString fullPath = dir.filePath(SaveFileName + ".pdf");
	parentWebEngineView->page()->printToPdf(fullPath, PDFLayout);

	connect(parentWebEngineView, &QWebEngineView::pdfPrintingFinished, this, [this]() {
		parentStatusLineEdit->setText("已导出");
		ui->pushButton_output->setDisabled(false);
		this->close();
	});
}
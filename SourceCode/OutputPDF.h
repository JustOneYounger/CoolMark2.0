#pragma once

#include <QMainWindow>
#include "ui_OutputPDF.h"
#include "CoolMarkInclude.h"

QT_BEGIN_NAMESPACE
namespace Ui { class OutputPDFClass; };
QT_END_NAMESPACE

class OutputPDF : public QMainWindow
{
	Q_OBJECT

public:
	OutputPDF(QWidget *parent = nullptr);
	~OutputPDF();
	Ui::OutputPDFClass *ui;
public:
	QWebEngineView* parentWebEngineView;
	QLineEdit* parentStatusLineEdit;
public:
	QHash<QString, QPageSize>PageSizeHash;
	void setpagesizehash();
	void Output();
};

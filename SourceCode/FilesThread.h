#pragma once
#include "CoolMarkInclude.h"

class MarkdownConverterThread : public QThread {
    Q_OBJECT
public:
    QString python_converter_path;
    QString input_file;
    QString output_file;
    MarkdownConverterThread(const QString& python_converter_path, const QString& input_file, const QString& output_file) : python_converter_path(python_converter_path), input_file(input_file), output_file(output_file) {}

protected:
    void run() override {
        QProcess process;
        process.setProgram(python_converter_path);
        process.setArguments({ input_file, output_file });
        process.start();
        process.waitForFinished();
    }
};

class CombinationThread : public QThread {
    Q_OBJECT
public:
    QString converterfile;
    QString cssfile;
    CombinationThread(const QString& converterfile,const QString& cssfile) :converterfile(converterfile),cssfile(cssfile){}

protected:
    void run() override {
        QString Html;
        QFile TempHtmlFile(converterfile);
        if (TempHtmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream ReadHtml(&TempHtmlFile);
            Html = ReadHtml.readAll();
            TempHtmlFile.close();
        }
        if (TempHtmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream Append(&TempHtmlFile);
            QString newHtml=
                "<!DOCTYPE html>\n"
                "<html lang=\"zh-CN\">\n"
                "<head>\n"
                "<meta charset=\"utf-8\">\n"
                "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">\n"
                "<link rel=\"stylesheet\" href=\"" + cssfile + "\">\n"
                "<script type=\"text/x-mathjax-config\">MathJax.Hub.Config({showProcessingMessages: false,messageStyle:\"none\",config:[\"MMLorHTML.js\"],jax:[\"input/TeX\",\"output/HTML-CSS\",\"output/NativeMML\"],extensions:[\"MathMenu.js\",\"MathZoom.js\"]});</script>\n"
                "<script type=\"text/javascript\" src=\"https://cdn.staticfile.org/mathjax/2.7.7/MathJax.js\"></script>\n"
                "</head>\n"
                "<body>\n"
                + Html +
                "</body>\n"
                "</html>\n";
            Append << newHtml;
            TempHtmlFile.close();
        }
    }
};

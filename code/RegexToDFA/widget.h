#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_runButton_clicked();

    void on_displayNFAButton_clicked();

    void on_displayDFAButton_clicked();

    void on_displayMinDFAButton_clicked();

    void on_uploadButton_clicked();

    void on_downloadButton_clicked();

private:
    Ui::Widget* ui;
};
#endif // WIDGET_H

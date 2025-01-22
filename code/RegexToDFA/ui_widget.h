/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *widget;
    QPlainTextEdit *plainTextEdit;
    QTableWidget *tableWidget;
    QPlainTextEdit *plainTextEdit_2;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *uploadButton;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *downloadButton;
    QSpacerItem *horizontalSpacer_6;
    QLabel *label_2;
    QLabel *label_3;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *runButton;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *displayNFAButton;
    QSpacerItem *horizontalSpacer_9;
    QPushButton *displayDFAButton;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *displayMinDFAButton;
    QSpacerItem *horizontalSpacer_10;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1003, 840);
        widget = new QWidget(Widget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(30, 20, 911, 801));
        plainTextEdit = new QPlainTextEdit(widget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 400, 851, 241));
        plainTextEdit->setReadOnly(true);
        tableWidget = new QTableWidget(widget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(0, 310, 911, 381));
        plainTextEdit_2 = new QPlainTextEdit(widget);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(0, 50, 911, 141));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(14);
        plainTextEdit_2->setFont(font);
        widget_5 = new QWidget(widget);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        widget_5->setGeometry(QRect(0, 200, 911, 71));
        horizontalLayout_4 = new QHBoxLayout(widget_5);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        uploadButton = new QPushButton(widget_5);
        uploadButton->setObjectName(QString::fromUtf8("uploadButton"));
        QFont font1;
        font1.setPointSize(15);
        uploadButton->setFont(font1);

        horizontalLayout_4->addWidget(uploadButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_7);

        downloadButton = new QPushButton(widget_5);
        downloadButton->setObjectName(QString::fromUtf8("downloadButton"));
        downloadButton->setFont(font1);

        horizontalLayout_4->addWidget(downloadButton);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_6);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 10, 411, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Agency FB"));
        font2.setPointSize(12);
        label_2->setFont(font2);
        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 280, 411, 31));
        label_3->setFont(font2);
        widget_3 = new QWidget(Widget);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setGeometry(QRect(30, 730, 911, 61));
        horizontalLayout_2 = new QHBoxLayout(widget_3);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        runButton = new QPushButton(widget_3);
        runButton->setObjectName(QString::fromUtf8("runButton"));
        runButton->setFont(font1);

        horizontalLayout_2->addWidget(runButton);

        horizontalSpacer_4 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        displayNFAButton = new QPushButton(widget_3);
        displayNFAButton->setObjectName(QString::fromUtf8("displayNFAButton"));
        displayNFAButton->setFont(font1);

        horizontalLayout_2->addWidget(displayNFAButton);

        horizontalSpacer_9 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_9);

        displayDFAButton = new QPushButton(widget_3);
        displayDFAButton->setObjectName(QString::fromUtf8("displayDFAButton"));
        displayDFAButton->setFont(font1);

        horizontalLayout_2->addWidget(displayDFAButton);

        horizontalSpacer_8 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_8);

        displayMinDFAButton = new QPushButton(widget_3);
        displayMinDFAButton->setObjectName(QString::fromUtf8("displayMinDFAButton"));
        displayMinDFAButton->setFont(font1);

        horizontalLayout_2->addWidget(displayMinDFAButton);

        horizontalSpacer_10 = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_10);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "\345\256\236\351\252\214\344\272\214\357\274\232\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\350\275\254\346\215\242\344\270\272DFA\345\233\276_\350\202\226\347\277\224", nullptr));
        plainTextEdit->setDocumentTitle(QString());
        plainTextEdit->setPlaceholderText(QString());
        plainTextEdit_2->setPlainText(QString());
        uploadButton->setText(QApplication::translate("Widget", "\346\211\223\345\274\200\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217txt", nullptr));
        downloadButton->setText(QApplication::translate("Widget", "\344\270\213\350\275\275\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217txt", nullptr));
        label_2->setText(QApplication::translate("Widget", "\350\257\267\345\234\250\344\273\245\344\270\213\350\276\223\345\205\245\346\241\206\350\276\223\345\205\245\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217\357\274\232", nullptr));
        label_3->setText(QApplication::translate("Widget", "\347\273\223\346\236\234\345\261\225\347\244\272\345\214\272\345\237\237\357\274\232", nullptr));
        runButton->setText(QApplication::translate("Widget", "\350\277\220\350\241\214", nullptr));
        displayNFAButton->setText(QApplication::translate("Widget", "\345\261\225\347\244\272NFA", nullptr));
        displayDFAButton->setText(QApplication::translate("Widget", "\345\261\225\347\244\272DFA", nullptr));
        displayMinDFAButton->setText(QApplication::translate("Widget", "\345\261\225\347\244\272\346\234\200\345\260\217\345\214\226DFA", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H

/********************************************************************************
** Form generated from reading UI file 'editwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITWINDOW_H
#define UI_EDITWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditWindow
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textEdit;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QTextEdit *textEdit_2;
    QPushButton *pushButton;

    void setupUi(QWidget *EditWindow)
    {
        if (EditWindow->objectName().isEmpty())
            EditWindow->setObjectName("EditWindow");
        EditWindow->resize(596, 457);
        verticalLayout = new QVBoxLayout(EditWindow);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(EditWindow);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 572, 194));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setObjectName("verticalLayout_3");
        textEdit = new QTextEdit(scrollAreaWidgetContents);
        textEdit->setObjectName("textEdit");

        verticalLayout_3->addWidget(textEdit);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        scrollArea_2 = new QScrollArea(EditWindow);
        scrollArea_2->setObjectName("scrollArea_2");
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName("scrollAreaWidgetContents_2");
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 572, 194));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(scrollAreaWidgetContents_2);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        textEdit_2 = new QTextEdit(scrollAreaWidgetContents_2);
        textEdit_2->setObjectName("textEdit_2");

        verticalLayout_2->addWidget(textEdit_2);

        scrollArea_2->setWidget(scrollAreaWidgetContents_2);

        verticalLayout->addWidget(scrollArea_2);

        pushButton = new QPushButton(EditWindow);
        pushButton->setObjectName("pushButton");

        verticalLayout->addWidget(pushButton, 0, Qt::AlignmentFlag::AlignRight);


        retranslateUi(EditWindow);

        QMetaObject::connectSlotsByName(EditWindow);
    } // setupUi

    void retranslateUi(QWidget *EditWindow)
    {
        EditWindow->setWindowTitle(QCoreApplication::translate("EditWindow", "Form", nullptr));
        label->setText(QCoreApplication::translate("EditWindow", "Console", nullptr));
        pushButton->setText(QCoreApplication::translate("EditWindow", "Check", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditWindow: public Ui_EditWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITWINDOW_H

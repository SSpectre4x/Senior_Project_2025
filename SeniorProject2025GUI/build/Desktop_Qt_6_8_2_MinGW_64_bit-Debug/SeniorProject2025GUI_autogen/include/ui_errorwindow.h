/********************************************************************************
** Form generated from reading UI file 'errorwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORWINDOW_H
#define UI_ERRORWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ErrorWindow
{
public:
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSave_As;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionHelp;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QToolBox *toolBox;
    QWidget *page;
    QWidget *page_2;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEdit;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QTextEdit *textEdit_2;
    QWidget *tab_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuAbout;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ErrorWindow)
    {
        if (ErrorWindow->objectName().isEmpty())
            ErrorWindow->setObjectName("ErrorWindow");
        ErrorWindow->resize(800, 600);
        actionOpen = new QAction(ErrorWindow);
        actionOpen->setObjectName("actionOpen");
        actionSave = new QAction(ErrorWindow);
        actionSave->setObjectName("actionSave");
        actionSave_As = new QAction(ErrorWindow);
        actionSave_As->setObjectName("actionSave_As");
        actionUndo = new QAction(ErrorWindow);
        actionUndo->setObjectName("actionUndo");
        actionRedo = new QAction(ErrorWindow);
        actionRedo->setObjectName("actionRedo");
        actionHelp = new QAction(ErrorWindow);
        actionHelp->setObjectName("actionHelp");
        centralwidget = new QWidget(ErrorWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        toolBox = new QToolBox(centralwidget);
        toolBox->setObjectName("toolBox");
        page = new QWidget();
        page->setObjectName("page");
        page->setGeometry(QRect(0, 0, 83, 458));
        toolBox->addItem(page, QString::fromUtf8("Page 1"));
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        page_2->setGeometry(QRect(0, 0, 83, 458));
        toolBox->addItem(page_2, QString::fromUtf8("Page 2"));

        horizontalLayout->addWidget(toolBox, 0, Qt::AlignmentFlag::AlignLeft);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setTabPosition(QTabWidget::TabPosition::North);
        tabWidget->setTabShape(QTabWidget::TabShape::Triangular);
        tab = new QWidget();
        tab->setObjectName("tab");
        verticalLayout = new QVBoxLayout(tab);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(tab);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 660, 244));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        textEdit = new QTextEdit(scrollAreaWidgetContents);
        textEdit->setObjectName("textEdit");
        QFont font;
        font.setPointSize(12);
        textEdit->setFont(font);

        verticalLayout_2->addWidget(textEdit);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        scrollArea_2 = new QScrollArea(tab);
        scrollArea_2->setObjectName("scrollArea_2");
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName("scrollAreaWidgetContents_2");
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 660, 214));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents_2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        label = new QLabel(scrollAreaWidgetContents_2);
        label->setObjectName("label");
        QFont font1;
        font1.setPointSize(10);
        label->setFont(font1);

        verticalLayout_3->addWidget(label, 0, Qt::AlignmentFlag::AlignTop);

        textEdit_2 = new QTextEdit(scrollAreaWidgetContents_2);
        textEdit_2->setObjectName("textEdit_2");
        textEdit_2->setFont(font);

        verticalLayout_3->addWidget(textEdit_2, 0, Qt::AlignmentFlag::AlignBottom);

        scrollArea_2->setWidget(scrollAreaWidgetContents_2);

        verticalLayout->addWidget(scrollArea_2, 0, Qt::AlignmentFlag::AlignBottom);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        tabWidget->addTab(tab_2, QString());

        horizontalLayout->addWidget(tabWidget);

        ErrorWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ErrorWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        menuAbout = new QMenu(menubar);
        menuAbout->setObjectName("menuAbout");
        ErrorWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ErrorWindow);
        statusbar->setObjectName("statusbar");
        ErrorWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_As);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuAbout->addAction(actionHelp);

        retranslateUi(ErrorWindow);

        toolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ErrorWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ErrorWindow)
    {
        ErrorWindow->setWindowTitle(QCoreApplication::translate("ErrorWindow", "MainWindow", nullptr));
        actionOpen->setText(QCoreApplication::translate("ErrorWindow", "Open", nullptr));
        actionSave->setText(QCoreApplication::translate("ErrorWindow", "Save", nullptr));
        actionSave_As->setText(QCoreApplication::translate("ErrorWindow", "Save As", nullptr));
        actionUndo->setText(QCoreApplication::translate("ErrorWindow", "Undo", nullptr));
        actionRedo->setText(QCoreApplication::translate("ErrorWindow", "Redo", nullptr));
        actionHelp->setText(QCoreApplication::translate("ErrorWindow", "Help", nullptr));
        toolBox->setItemText(toolBox->indexOf(page), QCoreApplication::translate("ErrorWindow", "Page 1", nullptr));
        toolBox->setItemText(toolBox->indexOf(page_2), QCoreApplication::translate("ErrorWindow", "Page 2", nullptr));
        label->setText(QCoreApplication::translate("ErrorWindow", "Output", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("ErrorWindow", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("ErrorWindow", "Tab 2", nullptr));
        menuFile->setTitle(QCoreApplication::translate("ErrorWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("ErrorWindow", "Edit", nullptr));
        menuAbout->setTitle(QCoreApplication::translate("ErrorWindow", "About", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ErrorWindow: public Ui_ErrorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERRORWINDOW_H

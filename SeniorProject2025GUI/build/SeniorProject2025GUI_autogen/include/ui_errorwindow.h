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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
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
    QTabWidget *tabWidget;
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
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setTabPosition(QTabWidget::TabPosition::North);
        tabWidget->setTabShape(QTabWidget::TabShape::Triangular);

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

        tabWidget->setCurrentIndex(-1);


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

#include "errorwindow.h"
#include "ui_errorwindow.h"
#include "editwindow.h"

#include <QTabBar>
#include <QFileDialog>

ErrorWindow::ErrorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);

    connect(ui->tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &ErrorWindow::closeTab);
    connect(ui->actionOpen, &QAction::triggered, this, &ErrorWindow::openFile);
}

ErrorWindow::~ErrorWindow()
{
    delete ui;
}

// Function to add a tab with the new file
void ErrorWindow::addTabWithFile(const QString &fileName){

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, "Error", "Cannot open file: " + fileName);
        return;
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    // Create a new text widget to display the content
    EditWindow* textWidget = new EditWindow(this, fileName.toStdString());
    textWidget->setText(fileContent);

    // Use the file name as the tab title
    QString tabTitle = QFileInfo(fileName).fileName();  // Just the filename with extension
    int ind = ui->tabWidget->addTab(textWidget, tabTitle);

    // Switch to new tab on add
    ui->tabWidget->setCurrentIndex(ind);

}

void ErrorWindow::openFile() {
    // Get file dialog and open new tab from selected file.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Source File"), QDir::homePath(), tr("Source Files (*.s)"));
    if (!fileName.isEmpty())
        addTabWithFile(fileName);
}

void ErrorWindow::closeTab(int index)
{
    QWidget* tab = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);

    delete tab;
}
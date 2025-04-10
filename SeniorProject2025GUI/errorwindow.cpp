#include "errorwindow.h"
#include "ui_errorwindow.h"
#include "editwindow.h"

ErrorWindow::ErrorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
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
    EditWindow *textWidget = new EditWindow(this);
    textWidget->setText(fileContent);

    // Use the file name as the tab title
    QString tabTitle = QFileInfo(fileName).fileName();  // Just the filename with extension
    ui->tabWidget->addTab(textWidget, tabTitle);

}

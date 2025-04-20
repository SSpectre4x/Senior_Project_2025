#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "errorwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , errorWindow(nullptr)
{
    ui->setupUi(this);

    // Set text line placeholder
    ui->lineEdit->setPlaceholderText("Enter file name...");

    // Connect button click to open error window
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openErrorWindow);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::getFileDialog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openErrorWindow() {

    // Get file name from the text box
    QString fileName = ui->lineEdit->text();

    if (fileName.isEmpty()){
        QMessageBox::warning(this, "Error", "Please enter a file name");
        return;
    }

    // Create the second window if it doesn't exist
    if (!errorWindow) {
        errorWindow = new ErrorWindow(this);
        errorWindow->setWindowModality(Qt::ApplicationModal);  // Make it modal
        errorWindow->show();  // Enter modal loop
    }

    else errorWindow->show();

    // Open a new tab on the error window of the file
    errorWindow->addTabWithFile(fileName);

}

void MainWindow::getFileDialog() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Source File"), QDir::homePath(), tr("Source Files (*.s)"));
    ui->lineEdit->setText(fileName);
}

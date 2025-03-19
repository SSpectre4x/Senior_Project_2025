#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect button click to open second window
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openErrorWindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::openErrorWindow() {
    ErrorWindow* errorWindow = new ErrorWindow(this);  // Set parent for automatic deletion
    errorWindow->setWindowModality(Qt::ApplicationModal);  // Make it modal
    errorWindow->show();  // Enter modal loop

}


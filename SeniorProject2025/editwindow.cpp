#include "editwindow.h"
#include "ui_editwindow.h"
#include "main.h"

#include <QBuffer>

EditWindow::EditWindow(QWidget *parent, std::string fileName)
    : QWidget(parent)
    , ui(new Ui::EditWindow)
{
    this->fileName = fileName;
    ui->setupUi(this);

    // Connect button click to open error window
    connect(ui->pushButton, &QPushButton::clicked, this, &EditWindow::populateConsole);
}

EditWindow::~EditWindow()
{
    delete ui;
}

// Function to set text of the text box
void EditWindow::setText(const QString &text){
    ui->textEdit->setPlainText(text);
}

// Function to set text of the console text box
void EditWindow::populateConsole() {
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QTextStream out(&buffer);
    readFile(this->fileName, out);
    buffer.open(QIODevice::ReadOnly);
    QTextStream in(&buffer);
    ui->textEdit_2->setPlainText(in.readAll());
}
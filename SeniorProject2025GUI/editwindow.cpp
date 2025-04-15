#include "editwindow.h"
#include "ui_editwindow.h"

EditWindow::EditWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditWindow)
{
    ui->setupUi(this);
}

EditWindow::~EditWindow()
{
    delete ui;
}

// Function to set text of the text box
void EditWindow::setText(const QString &text){
    ui->textEdit->setPlainText(text);
}

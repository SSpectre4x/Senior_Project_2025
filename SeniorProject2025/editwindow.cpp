#include "editwindow.h"
#include "ui_editwindow.h"

#include <QBuffer>
#include <QMessageBox>
#include <QTextBlock>

#include "main.h"
#include "Error.h"

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
    ui->plainTextEdit->setPlainText(text);
}

// Function to set text of the console text box, set error tooltips and highlighting for code window
void EditWindow::populateConsole() {
    ui->plainTextEdit->lineErrors.clear();
    QTextDocument* doc = ui->plainTextEdit->document();

    // Create a buffer and a QTextStream to read data into the buffer.
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QTextStream out(&buffer);
    
    // First attempt to assemble and link the file, if error code
    if (assembleAndLink(this->fileName, out) != 0)
    {
        QMessageBox::warning(this, "Error", "File failed to link and assemble, error checker will not run. Please fix the file and try again.");

        buffer.open(QIODevice::ReadOnly);
        QTextStream in(&buffer);
        ui->plainTextEdit_2->setPlainText(in.readAll());
        return;
    }

    std::vector<std::vector<Error::Error>> error_vectors = readFile(this->fileName, false, true, true, true, &out);
    out << Qt::endl;

    for (std::vector<Error::Error> vector : error_vectors)
    {
        for (Error::Error error : vector)
        {
            // Push error back to code editor map of line to errors for tooltip display.
            ui->plainTextEdit->lineErrors[error.lineNumber].push_back(error);

            // Push text output to console.
            out << QString::fromStdString(Error::to_string(error)) << Qt::endl;

            // Set up line highlighting for block associated with error line number.
            QTextBlock tb = doc->findBlockByLineNumber(error.lineNumber - 1);
            QTextCursor cursor(tb);

            QTextBlockFormat fm = cursor.blockFormat();
            if (error.isWarning && fm.background() != QColor(Qt::red).lighter(120))
                fm.setBackground(QColor(Qt::yellow).lighter(150));
            else
                fm.setBackground(QColor(Qt::red).lighter(120));
            cursor.setBlockFormat(fm);
        }
    }

    // Switch the buffer to read out stored text and send output to console window.
    buffer.open(QIODevice::ReadOnly);
    QTextStream in(&buffer);
    ui->plainTextEdit_2->setPlainText(in.readAll());
}
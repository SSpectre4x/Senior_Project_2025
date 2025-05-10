#pragma once
#ifndef ERRORWINDOW_H
#define ERRORWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>

namespace Ui {
class ErrorWindow;
}

class ErrorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ErrorWindow(QWidget *parent = nullptr);
    void openFile();
    void addTabWithFile(const QString &fileName);
    void closeTab(int index);
    ~ErrorWindow();

private:
    Ui::ErrorWindow *ui;
};

#endif // ERRORWINDOW_H

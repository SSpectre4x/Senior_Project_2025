#pragma once
#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QWidget>

namespace Ui {
class EditWindow;
}

class EditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr, std::string fileName = "");
    void setText(const QString &text);
    void populateConsole();
    ~EditWindow();

private:
    Ui::EditWindow *ui;
    std::string fileName;
};

#endif // EDITWINDOW_H

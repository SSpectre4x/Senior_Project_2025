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
    explicit EditWindow(QWidget *parent = nullptr);
    void setText(const QString &text);
    ~EditWindow();

private:
    Ui::EditWindow *ui;
};

#endif // EDITWINDOW_H

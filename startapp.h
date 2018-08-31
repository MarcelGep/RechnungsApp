#ifndef STARTAPP_H
#define STARTAPP_H

#include <QDialog>

namespace Ui {
class StartApp;
}

class StartApp : public QDialog
{
    Q_OBJECT

public:
    explicit StartApp(QWidget *parent = nullptr);
    ~StartApp();

private:
    Ui::StartApp *ui;
};

#endif // STARTAPP_H

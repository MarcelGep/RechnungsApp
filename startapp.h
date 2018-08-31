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

private slots:
    void on_btnExit_clicked();

private slots:
    void on_btnCreateDatabase_clicked();
    void on_btnOpenDatabase_clicked();
    void on_btnOpenExistDatabase_clicked();
    
private:
    Ui::StartApp *ui;
};

#endif // STARTAPP_H

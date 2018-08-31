#include "startapp.h"
#include "ui_startapp.h"

StartApp::StartApp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartApp)
{
    ui->setupUi(this);
}

StartApp::~StartApp()
{
    delete ui;
}

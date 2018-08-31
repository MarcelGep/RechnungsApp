#include "startapp.h"
#include "ui_startapp.h"

#include "mainwindow.h"

#include <dbmanager.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

QString path_database;

StartApp::StartApp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartApp)
{
    ui->setupUi(this);
    
    QSettings settings("data/config.ini", QSettings::IniFormat);
    QString search_file = settings.value("path_database", "").toString();

    if(search_file.isEmpty())
    {
        ui->lbExistDatabase->setText("Es steht keine Voreinstellung zur Verfügung!");
        ui->btnOpenExistDatabase->setEnabled(false);
        return;
    }
    else
    {
        QFileInfo fi(search_file);
        QDir directory(fi.dir());
        QString filename = fi.baseName() + ".db";
        QStringList Filter;
        Filter << "*.db";
        QStringList data = directory.entryList(Filter);
        foreach(QString file, data)
        {
            if(file == filename)
            {
                ui->lbExistDatabase->setStyleSheet("Color : rgb(0, 150, 0)");
                ui->lbExistDatabase->setText(settings.value("path_database", "").toString());
                ui->btnOpenExistDatabase->setEnabled(true);
                return;
            }
        }
        ui->lbExistDatabase->setStyleSheet("Color : rgb(190, 0, 0)");
        ui->lbExistDatabase->setText("Voreinstellung kann nicht mehr gefunden werden!");
        ui->btnOpenExistDatabase->setEnabled(false);
    }
}

StartApp::~StartApp()
{
    delete ui;
}

void StartApp::on_btnCreateDatabase_clicked()
{
    bool copyState = QFile::copy(PATH_DATABASE_CLEAN, PATH_DATABASE);

    if(copyState)
    {
        QSettings settings("data/config.ini", QSettings::IniFormat);
        settings.setValue("path_database", PATH_DATABASE);

        accept();
        QMessageBox::information(this, "Neue Datenbank", "Die Datenbank wurde erfolgreich erstellt!", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, "Neue Datenbank", "Fehler beim erstellen der Datenbank!", QMessageBox::Ok);
    }
}

void StartApp::on_btnOpenDatabase_clicked()
{
    path_database = QFileDialog::getOpenFileName(this, "Datenbank auswählen", "database/", "Data Base File (*.db)");
    if(path_database.isEmpty())
        return;
    else
    {
        accept();
    }

    QSettings settings("data/config.ini", QSettings::IniFormat);
    settings.setValue("path_database", path_database);
}

void StartApp::on_btnOpenExistDatabase_clicked()
{
    path_database = ui->lbExistDatabase->text();
    accept();    
}

void StartApp::on_btnExit_clicked()
{
    QMessageBox msg;
    msg.setText("Möchten Sie das Programm wirklich beenden?");
    msg.setWindowTitle("Verlassen");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setButtonText(QMessageBox::Yes, "Ja");
    msg.setButtonText(QMessageBox::No, "Nein");
    msg.setDefaultButton(QMessageBox::No);
    msg.setIcon(QMessageBox::Question);

    if(msg.exec() == QMessageBox::Yes)
    {
        exit(0);
    }
}

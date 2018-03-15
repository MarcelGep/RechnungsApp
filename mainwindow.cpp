#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_dbManager = new DBManager(PATH_DATABASE);

    ui->tabMain->setCurrentIndex(CustomersTab);
    ui->tabWidKunden->setCurrentIndex(OverviewTab);

    printAllCustomers();
}

MainWindow::~MainWindow()
{
    if (m_dbManager->isOpen())
    {
        m_dbManager->closeDatabase();
    }
    delete m_dbManager;
    delete ui;
}

void MainWindow::on_btnSaveCustomer_clicked()
{
    int kdnr = ui->leKdNr->text().toInt();
    QString firma = ui->leFirma->text();
    QString name1 = ui->leName1->text();
    QString name2 = ui->leName2->text();
    QString strasse = ui->leStrasse->text();

    int plz = 0;
    QString plz_str = ui->lePlz->text();
    for (int i =0;i<plz_str.size();i++)
    {
        if (!(plz_str[i].isDigit()))
        {
            QMessageBox::warning(this, "Eingabefehler", "Keine gültige PLZ!");
            return;
        }
    }
    plz = ui->lePlz->text().toInt();

    QString ort = ui->leOrt->text();
    QString land = ui->leLand->text();
    QString telefon = ui->leTelefon->text();
    QString telefax = ui->leTelefax->text();
    QString email = ui->leEmail->text();
    QString website = ui->leWebsite->text();
    double rabatt = ui->sbRabatt->value();
    double kontostand = ui->sbKontostand->value();
    QString info = ui->ptCustomerInfo->toPlainText();

    Customers customer(kdnr,
                       firma,
                       name1,
                       name2,
                       strasse,
                       plz,
                       ort,
                       land,
                       telefon,
                       telefax,
                       email,
                       website,
                       rabatt,
                       kontostand,
                       info);

    if (m_dbManager->dbEntryExist(KUNDEN, QString::number(kdnr)))
    {
        if (m_dbManager->editCustomer(QString::number(kdnr), customer))
        {
            printAllCustomers();
            QMessageBox::information(this, "Info", "Der Kunde wurde erfolgreich bearbeitet!", QMessageBox::Ok);
            clearCustomerEdits();
            ui->tabWidKunden->setCurrentIndex(OverviewTab);
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim bearbeiten des Kunden!", QMessageBox::Ok);
    }
    else
    {
        if (m_dbManager->addCustomer(customer))
        {
            printAllCustomers();
            QMessageBox::information(this, "Info", "Der Kunde wurde erfolgreich angelegt!", QMessageBox::Ok);
            clearCustomerEdits();
            ui->tabWidKunden->setCurrentIndex(OverviewTab);
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim erstellen des Kunden!", QMessageBox::Ok);
    }
}

void MainWindow::on_btnDeleteCustomer_clicked()
{
    if(ui->tvCustomers->selectionModel()->hasSelection())
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie den Kunden wirklich löschen?");
        msg.setWindowTitle("Kunden löschen");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            int row = ui->tvCustomers->selectionModel()->selectedRows().at(0).row();
            QString id = ui->tvCustomers->model()->index(row, KdNr).data().toString();

            if(m_dbManager->removeDbEntry(KUNDEN, id))
            {
                printAllCustomers();
                QMessageBox::information(this, "Info", "Der ausgewählte Kunde wurde erfolgreich gelöscht!", QMessageBox::Ok);
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Es wurde kein Kunde zum löschen ausgewählt!", QMessageBox::Ok);
    }
}

void MainWindow::on_btnNewCustomer_clicked()
{
    clearCustomerEdits();
    ui->tabWidKunden->setCurrentIndex(EditTab);

    int lastID = getLastID(KUNDEN);
    ui->leKdNr->setText(QString::number(lastID + 1));

    ui->tvCustomers->clearSelection();
}

void MainWindow::on_btnEditCustomer_clicked()
{
    // Alle felder im Kundenbereich ausfüllen
    if(printCustomer())
    {
        // Zum Kunden Anlegen/Editieren Tab wechseln
        ui->tabWidKunden->setCurrentIndex(EditTab);
    }
    else
    {
        QMessageBox::warning(this, "Keine Auswahl", "Es wurde kein Kunde ausgwählt!");
    }
}

void MainWindow::on_btnCancelCustomer_clicked()
{
    ui->tabWidKunden->setCurrentIndex(OverviewTab);
    clearCustomerEdits();
}

bool MainWindow::printCustomer()
{  
    if(ui->tvCustomers->selectionModel()->hasSelection())
    {
        int row = ui->tvCustomers->selectionModel()->selectedRows().at(0).row();
        ui->leKdNr->setText(ui->tvCustomers->model()->index(row, KdNr).data().toString());
        ui->leFirma->setText(ui->tvCustomers->model()->index(row, Firma).data().toString());
        ui->leName1->setText(ui->tvCustomers->model()->index(row, Name1).data().toString());
        ui->leName2->setText(ui->tvCustomers->model()->index(row, Name2).data().toString());
        ui->leStrasse->setText(ui->tvCustomers->model()->index(row, Strasse).data().toString());
        ui->lePlz->setText(ui->tvCustomers->model()->index(row, Plz).data().toString());
        ui->leOrt->setText(ui->tvCustomers->model()->index(row, Ort).data().toString());
        ui->leLand->setText(ui->tvCustomers->model()->index(row, Land).data().toString());
        ui->leTelefon->setText(ui->tvCustomers->model()->index(row, Telefon).data().toString());
        ui->leTelefax->setText(ui->tvCustomers->model()->index(row, Telefax).data().toString());
        ui->leEmail->setText(ui->tvCustomers->model()->index(row, Email).data().toString());
        ui->leWebsite->setText(ui->tvCustomers->model()->index(row, Website).data().toString());
        ui->sbRabatt->setValue(ui->tvCustomers->model()->index(row, Rabatt).data().toDouble());
        ui->sbKontostand->setValue(ui->tvCustomers->model()->index(row, Kontostand).data().toDouble());
        ui->ptCustomerInfo->setPlainText(ui->tvCustomers->model()->index(row, Info).data().toString());

        return true;
    }

    return false;
}

void MainWindow::printAllAricles() const
{
    ui->tvArtList->setModel(m_dbManager->readDbData(ARTIKEL));
    qDebug() << DEBUG_TAG << ": Print all articles successfull!";
}

void MainWindow::printAllCustomers() const
{
    ui->tvCustomers->setModel(m_dbManager->readDbData(KUNDEN));
    qDebug() << DEBUG_TAG << ": Print all customers successfull!";
}

void MainWindow::clearArticleEdits()
{
    ui->leArtName->clear();
    ui->leArtPrice->clear();
    ui->leArtUnit->clear();
    ui->ptArtDescription->clear();
    ui->leArtNr->setText(QString::number(getLastID(ARTIKEL) + 1));
}

void MainWindow::clearCustomerEdits() const
{
    ui->leKdNr->clear();
    ui->leFirma->clear();
    ui->leName1->clear();
    ui->leName2->clear();
    ui->leStrasse->clear();
    ui->lePlz->clear();
    ui->leOrt->clear();
    ui->leLand->clear();
    ui->leTelefon->clear();
    ui->leTelefax->clear();
    ui->leEmail->clear();
    ui->leWebsite->clear();
    ui->sbRabatt->setValue(0);
    ui->sbKontostand->setValue(0);
    ui->ptCustomerInfo->clear();
}

int MainWindow::getLastID(QString table)
{
    int lastID = -1;

    if (m_dbManager->isOpen())
    {
        lastID = m_dbManager->readLastID(table);
    }
    else
    {
        qDebug() << "Database is not open!";
    }

    return lastID;
}


void MainWindow::on_tabWidKunden_tabBarClicked(int index)
{
    switch(index)
    {
        case EditTab:
            if(!printCustomer())
            {
                int lastID = getLastID(KUNDEN);
                ui->leKdNr->setText(QString::number(lastID + 1));
            }
            break;

        case OverviewTab:
            break;

        default:
    break;
    }
}


void MainWindow::on_tabMain_currentChanged(int index)
{
    switch(index)
    {
        case EvidenceTab:
            break;

        case CustomersTab:
            break;

        case ArticlesTab:
        {
            clearArticleEdits();
            printAllAricles();
        }
        break;

        case CalculationsTab:
        {
            QList<QString> entries;
            std::vector<Customers> customerList;
            m_dbManager->readCustomers(customerList);
            for(std::vector<Customers>::iterator it = customerList.begin(); it != customerList.end(); ++it)
            {
                QString tempEntry;
                tempEntry += QString::number(it->getKdnr()) + " - ";
                tempEntry += it->getFirma().isEmpty() ? "" : it->getFirma() + ", ";
                tempEntry += it->getName1() + " " + it->getName2() + ", ";
                tempEntry += it->getStrasse() + ", ";
                tempEntry += QString::number(it->getPlz()) + " " + it->getOrt();
;
                entries.push_back(tempEntry);
            }

            ui->cbRgCustomer->clear();
            ui->cbRgCustomer->addItems(entries);
            ui->leRgSubject->setText("Rechnung XXX");
            QDate date;
            ui->deRgDate->setDate(date.currentDate());
        }
        break;

        case SettingsTab:
            break;

        default:
            break;
    }
}

void MainWindow::on_btnRgRechnung_clicked()
{
    ui->leRgSubject->setText("Rechnung XXX");
    ui->btnRgRechnung->setChecked(true);
    ui->btnRgAngebot->setChecked(false);
    ui->btnRgGutschrift->setChecked(false);
}

void MainWindow::on_btnRgAngebot_clicked()
{
    ui->leRgSubject->setText("Angebot XXX");
    ui->btnRgRechnung->setChecked(false);
    ui->btnRgAngebot->setChecked(true);
    ui->btnRgGutschrift->setChecked(false);
}

void MainWindow::on_btnRgGutschrift_clicked()
{
    ui->leRgSubject->setText("Gutschrift XXX");
    ui->btnRgRechnung->setChecked(false);
    ui->btnRgAngebot->setChecked(false);
    ui->btnRgGutschrift->setChecked(true);
}

void MainWindow::on_btnArtSave_clicked()
{
    int artnr = ui->leArtNr->text().toInt();
    QString unit = ui->leArtUnit->text();
    QString name = ui->leArtName->text();
    double price = ui->leArtPrice->text().toDouble();
    QString description = ui->ptArtDescription->toPlainText();

    Articles article(artnr, name, description, unit, price);

    if (m_dbManager->dbEntryExist(ARTIKEL, QString::number(artnr)))
    {
        // edit data entry
//        if (m_dbManager->editCustomer(QString::number(artnr), customer))
//        {
//            printAllCustomers();
//            QMessageBox::information(this, "Info", "Der Kunde wurde erfolgreich bearbeitet!", QMessageBox::Ok);
//            clearCustomerEdits();
//            ui->tabWidKunden->setCurrentIndex(OverviewTab);
//        }
//        else
//            QMessageBox::critical(this, "Error", "Fehler beim bearbeiten des Kunden!", QMessageBox::Ok);
    }
    else
    {
        // add data entry
        if (m_dbManager->addArticle(article))
        {
            printAllAricles();
            clearArticleEdits();
            QMessageBox::information(this, "Info", "Der Artikel wurde erfolgreich angelegt!", QMessageBox::Ok);
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim erstellen des Artikels!", QMessageBox::Ok);
    }
}

void MainWindow::on_tvArtList_clicked(const QModelIndex &index)
{
    int row = index.row();
    QString artnr = ui->tvArtList->model()->index(row, 0).data().toString();
    QString unit = ui->tvArtList->model()->index(row, 1).data().toString();
    QString name = ui->tvArtList->model()->index(row, 2).data().toString();
    QString price = ui->tvArtList->model()->index(row, 3).data().toString();
    QString description = ui->tvArtList->model()->index(row, 4).data().toString();

    ui->leArtNr->setText(artnr);
    ui->leArtUnit->setText(unit);
    ui->leArtName->setText(name);
    ui->leArtPrice->setText(price);
    ui->ptArtDescription->setPlainText(description);
}

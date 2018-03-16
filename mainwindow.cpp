#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_dbManager = new DBManager(PATH_DATABASE);

    // Read table fields from DB
    m_customerFields = m_dbManager->readFieldNames(KUNDEN);
    m_articleFields = m_dbManager->readFieldNames(ARTIKEL);
    m_calculationFields = m_dbManager->readFieldNames(RECHNUNG);
    m_positionFields = m_dbManager->readFieldNames(POSITIONEN);

    // Set active tabs
    ui->tabMain->setCurrentIndex(EvidenceTab);
    ui->tabWidKunden->setCurrentIndex(OverviewTab);
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
    if(ui->twCustomers->selectedItems().count() >  0)
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
            QString id = ui->twCustomers->item(ui->twCustomers->currentRow(), 0)->text();
            m_dbManager->removeDbEntry(KUNDEN, id);
            printAllCustomers();
            QMessageBox::information(this, "Info", "Der ausgewählte Kunde wurde erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        qDebug() << "No customer selected!";
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
    if(ui->twCustomers->selectedItems().count() >  0)
        {
            ui->tabWidKunden->setCurrentIndex(EditTab);
            ui->leKdNr->clear();

            QString kdnr = ui->twCustomers->item(ui->twCustomers->currentRow(), KdNr)->text();
            Customers customer = m_dbManager->readCustomer(kdnr);

            printCustomer(customer);
        }
        else
        {
            qDebug() << "No customer selected!";
            QMessageBox::warning(this, "Warnung", "Es wurde kein Kunde zum bearbeiten ausgewählt!", QMessageBox::Ok);
        }
}

void MainWindow::on_btnCancelCustomer_clicked()
{
    ui->tabWidKunden->setCurrentIndex(OverviewTab);
    clearCustomerEdits();
}

void MainWindow::printCustomer(Customers customer)
{  
    ui->leKdNr->setText(QString::number(customer.getKdnr()));
    ui->leFirma->setText(customer.getFirma());
    ui->leName1->setText(customer.getName1());
    ui->leName2->setText(customer.getName2());
    ui->leStrasse->setText(customer.getStrasse());
    ui->lePlz->setText(QString::number(customer.getPlz()));
    ui->leOrt->setText(customer.getOrt());
    ui->leLand->setText(customer.getLand());
    ui->leTelefon->setText(customer.getTelefon());
    ui->leTelefax->setText(customer.getTelefax());
    ui->leEmail->setText(customer.getEmail());
    ui->leWebsite->setText(customer.getWebsite());
    ui->sbRabatt->setValue(customer.getRabatt());
    ui->sbKontostand->setValue(customer.getKontostand());
    ui->ptCustomerInfo->setPlainText(customer.getInfo());
}

void MainWindow::printAllAricles() const
{
    ui->tvArtList->setModel(m_dbManager->readDbData(ARTIKEL));
    qDebug() << DEBUG_TAG << ": Print all articles successfull!";
}

void MainWindow::clearTWCustomers() const
{
    while (ui->twCustomers->rowCount() > 0)
    {
        ui->twCustomers->removeRow(0);
    }
}

void MainWindow::printAllCustomers()
{
    if(ui->twCustomers->rowCount() > 0)
           clearTWCustomers();

    // set columns
    ui->twCustomers->setColumnCount(m_customerFields.size() - 1);
    for ( int i = 0; i < m_customerFields.size(); i++)
    {
        ui->twCustomers->setHorizontalHeaderItem(i, new QTableWidgetItem(m_customerFields[i]));
    }

    std::vector<Customers> customers;
    if (!m_dbManager->readCustomers(customers))
    {
       qDebug() << DEBUG_TAG << ": Error read customers!";
       return;
    }

    for (std::vector<Customers>::iterator it = customers.begin(); it != customers.end(); ++it)
    {
       int row = ui->twCustomers->rowCount();
       ui->twCustomers->insertRow(row);
       //ui->twCustomers->setRowHeight(row, CUSTOMER_ROW_HEIGHT);
       ui->twCustomers->setItem(row, KdNr, new QTableWidgetItem(QString::number(it->getKdnr())));
       ui->twCustomers->setItem(row, Firma, new QTableWidgetItem(it->getFirma()));
       ui->twCustomers->setItem(row, Name1, new QTableWidgetItem(it->getName1()));
       ui->twCustomers->setItem(row, Name2, new QTableWidgetItem(it->getName2()));
       ui->twCustomers->setItem(row, Strasse, new QTableWidgetItem(it->getStrasse()));
       ui->twCustomers->setItem(row, Plz, new QTableWidgetItem(QString::number(it->getPlz())));
       ui->twCustomers->setItem(row, Ort, new QTableWidgetItem(it->getOrt()));
       ui->twCustomers->setItem(row, Land, new QTableWidgetItem(it->getLand()));
       ui->twCustomers->setItem(row, Telefon, new QTableWidgetItem(it->getTelefon()));
       ui->twCustomers->setItem(row, Telefax, new QTableWidgetItem(it->getTelefax()));
       ui->twCustomers->setItem(row, Email, new QTableWidgetItem(it->getEmail()));
       ui->twCustomers->setItem(row, Website, new QTableWidgetItem(it->getWebsite()));
       ui->twCustomers->setItem(row, Rabatt, new QTableWidgetItem(QString::number(it->getRabatt()) + " %"));
       ui->twCustomers->item(row, Rabatt)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twCustomers->setItem(row, Kontostand, new QTableWidgetItem(QString::number(it->getKontostand()) + " €"));
       ui->twCustomers->item(row, Kontostand)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }

    ui->twCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
   //ui->twCustomers->resizeColumnsToContents();

   /* Set customer table column width offset */
    for (int i = 0; i < ui->twCustomers->columnCount(); i++)
    {
        int tempWidth = ui->twCustomers->columnWidth(i);
        ui->twCustomers->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }

    qDebug() << DEBUG_TAG << ": Print all customers!";
}

void MainWindow::setCustomerColumnsWidth() const
{
    ui->tvCustomers->resizeColumnsToContents();
    for(int i = 0; i < columnsCount; i++)
    {
        int colWidth = ui->tvCustomers->columnWidth(i);
        ui->tvCustomers->setColumnWidth(i, colWidth + CUSTOMER_COLUMN_OFFSET);
    }
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
    if (index == EditTab)
    {
        if(ui->twCustomers->selectedItems().count() >  0)
        {
            QString kdnr = ui->twCustomers->item(ui->twCustomers->currentRow(), KdNr)->text();
            Customers customer = m_dbManager->readCustomer(kdnr);

            printCustomer(customer);
        }
        else
        {
            int lastID = getLastID("Kunden");
            ui->leKdNr->setText(QString::number(lastID + 1));
        }
    }
    else if(index == OverviewTab)
    {
    //        if(ui->twCustomers->selectedItems().count() >  0)
    //        {
    //            clearCustomerEdits();
    //            ui->leKdNr->clear();
    //            ui->twCustomers->clearSelection();
    //            printAllCustomers();
    //        }
    }
}


void MainWindow::on_tabMain_currentChanged(int index)
{
    switch(index)
    {
        case EvidenceTab:
            break;

        case CustomersTab:
        {
            // Setup customer list
            QFont font("MS Shell Dlg 2", 8, QFont::Bold);
            ui->twCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            ui->twCustomers->horizontalHeader()->setFont(font);
            printAllCustomers();
        }
        break;

        case ArticlesTab:
        {
            // Setup article list
            ui->tvArtList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
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

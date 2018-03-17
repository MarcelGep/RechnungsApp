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

    // Setup customer list
    ui->twCustomers->setColumnCount(m_customerFields.size() - 1);
    for ( int i = 0; i < m_customerFields.size(); i++)
    {
        ui->twCustomers->setHorizontalHeaderItem(i, new QTableWidgetItem(m_customerFields[i]));
    }
    QFont fontCustomer("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twCustomers->horizontalHeader()->setFont(fontCustomer);
    ui->twCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Setup article list
    ui->twArticles->setColumnCount(m_articleFields.size());
    for ( int i = 0; i < m_articleFields.size(); i++)
    {
        ui->twArticles->setHorizontalHeaderItem(i, new QTableWidgetItem(m_articleFields[i]));
    }
    QFont fontArticles("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twArticles->horizontalHeader()->setFont(fontArticles);
    ui->twArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twArticles->horizontalHeader()->setSectionResizeMode(Beschreibung, QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    m_dbManager->closeDatabase();

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
            QMessageBox::information(this, "Info", "Der Kunde wurde erfolgreich angelegt!", QMessageBox::Ok);
            clearCustomerEdits();
            printAllCustomers();
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

    int lastID = m_dbManager->readLastID(KUNDEN);
    ui->leKdNr->setText(QString::number(lastID + 1));

    ui->tabWidKunden->setCurrentIndex(EditTab);

    ui->twCustomers->clearSelection();
}

void MainWindow::on_btnEditCustomer_clicked()
{
    if(ui->twCustomers->selectedItems().count() >  0)
        {
            ui->tabWidKunden->setCurrentIndex(EditTab);
            ui->leKdNr->clear();

            QString kdnr = ui->twCustomers->item(ui->twCustomers->currentRow(), KdNr)->text();
            Customers customer;
            m_dbManager->readCustomer(kdnr, customer);

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
    ui->twCustomers->clearSelection();
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

void MainWindow::printAllArticles()
{
    // clear article list
    if(ui->twArticles->rowCount() > 0)
        clearArticles();

    // read articles from database
    if (!m_dbManager->readArticles(m_articles))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customers!";
       return;
    }

    // print all exist articles to list
    for (std::vector<Articles>::iterator it = m_articles.begin(); it != m_articles.end(); ++it)
    {
       int row = ui->twArticles->rowCount();
       ui->twArticles->insertRow(row);
       ui->twArticles->setRowHeight(row, ARTICLE_ROW_HEIGHT);
       ui->twArticles->setItem(row, ArtNr, new QTableWidgetItem(QString::number(it->getArtNr())));
       ui->twArticles->setItem(row, Einheit, new QTableWidgetItem(it->getUnit()));
       ui->twArticles->setItem(row, Bezeichnung, new QTableWidgetItem(it->getName()));
       ui->twArticles->setItem(row, Beschreibung, new QTableWidgetItem(it->getDescription()));
       ui->twArticles->setItem(row, Preis, new QTableWidgetItem(QString::number(it->getPrice(), 'f', 2) + " €"));
       ui->twArticles->item(row, Preis)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    // Set articles table column width offset
    setArticleColumnsWidth();

    qDebug() << DEBUG_TAG_MAIN << ": Print all articles successfull!";
}

void MainWindow::clearArticles()
{
    while (ui->twArticles->rowCount() > 0)
    {
        ui->twArticles->removeRow(0);
    }
    m_articles.clear();
}

void MainWindow::clearCustomers()
{
    while (ui->twCustomers->rowCount() > 0)
    {
        ui->twCustomers->removeRow(0);
    }
    m_customers.clear();
}

void MainWindow::printAllCustomers()
{
    // clear customer list
    if(ui->twCustomers->rowCount() > 0)
        clearCustomers();

    // read customers from database
    if (!m_dbManager->readCustomers(m_customers))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customers!";
       return;
    }

    // print all exist customers to list
    for (std::vector<Customers>::iterator it = m_customers.begin(); it != m_customers.end(); ++it)
    {
       int row = ui->twCustomers->rowCount();
       ui->twCustomers->insertRow(row);
       ui->twCustomers->setRowHeight(row, CUSTOMER_ROW_HEIGHT);
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
       ui->twCustomers->setItem(row, Rabatt, new QTableWidgetItem(QString::number(it->getRabatt(), 'f', 2) + " %"));
       ui->twCustomers->item(row, Rabatt)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twCustomers->setItem(row, Kontostand, new QTableWidgetItem(QString::number(it->getKontostand(), 'f', 2) + " €"));
       ui->twCustomers->item(row, Kontostand)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }

    // Set customer table column width offset
    setCustomerColumnsWidth();

    qDebug() << DEBUG_TAG_MAIN << ": Print all customers!";
}

void MainWindow::setCustomerColumnsWidth() const
{
    ui->twCustomers->resizeColumnsToContents();

    for (int i = 0; i < ui->twCustomers->columnCount(); i++)
    {
        int tempWidth = ui->twCustomers->columnWidth(i);
        ui->twCustomers->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }
}

void MainWindow::setArticleColumnsWidth() const
{
    ui->twArticles->resizeColumnsToContents();

    for (int i = 0; i < ui->twArticles->columnCount(); i++)
    {
        int tempWidth = ui->twArticles->columnWidth(i);
        ui->twArticles->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }
}

void MainWindow::clearArticleEdits()
{
    ui->leArtName->clear();
    ui->leArtPrice->clear();
    ui->leArtUnit->clear();
    ui->ptArtDescription->clear();
    ui->leArtNr->setText(QString::number(m_dbManager->readLastID(ARTIKEL) + 1));
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

void MainWindow::on_tabWidKunden_tabBarClicked(int index)
{
    if (index == EditTab)
    {
        if(ui->twCustomers->selectedItems().count() >  0)
        {
            QString kdnr = ui->twCustomers->item(ui->twCustomers->currentRow(), KdNr)->text();
            Customers customer;
            m_dbManager->readCustomer(kdnr, customer);

            printCustomer(customer);
        }
        else
        {
            int lastID = m_dbManager->readLastID(KUNDEN);
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
            printAllCustomers();
        }
        break;

        case ArticlesTab:
        {
            // Setup article list
            clearArticleEdits();
            printAllArticles();
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
            printAllArticles();
            clearArticleEdits();
            QMessageBox::information(this, "Info", "Der Artikel wurde erfolgreich angelegt!", QMessageBox::Ok);
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim erstellen des Artikels!", QMessageBox::Ok);
    }
}

void MainWindow::on_twArticles_itemClicked(QTableWidgetItem *item)
{
    // read customer from database
    QString artNr = ui->twArticles->item(item->row(), ArtNr)->text();
    Articles article;
    if (!m_dbManager->readArticle(artNr, article))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customer!";
       return;
    }

    // print all customer to edits
    ui->leArtNr->setText(QString::number(article.getArtNr()));
    ui->leArtUnit->setText(article.getUnit());
    ui->leArtName->setText(article.getName());
    ui->leArtPrice->setText(QString::number(article.getPrice()));
    ui->ptArtDescription->setPlainText(article.getDescription());
}

void MainWindow::on_btnArtNew_clicked()
{
    ui->twArticles->clearSelection();
    clearArticleEdits();
}

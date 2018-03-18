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

    // Setup article positions
    ui->twRgArticles->setColumnCount(m_positionFields.size() - 1);
    for ( int i = 1; i < m_positionFields.size(); i++)
    {
        ui->twRgArticles->setHorizontalHeaderItem(i-1, new QTableWidgetItem(m_positionFields[i]));
    }
    QFont fontArticlesPos("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twRgArticles->horizontalHeader()->setFont(fontArticlesPos);
    ui->twRgArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twRgArticles->horizontalHeader()->setSectionResizeMode(BeschreibungPos, QHeaderView::Stretch);
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
    if (ui->twCustomers->selectedItems().count() >  0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie den Kunden wirklich löschen?");
        msg.setWindowTitle("Warnung");
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

            if (ui->tabWidKunden->currentIndex() == EditTab)
            {
                ui->btnDeleteCustomer->setEnabled(false);
                ui->btnEditCustomer->setEnabled(false);
                ui->tabWidKunden->setCurrentIndex(OverviewTab);
                clearCustomerEdits();
            }

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
    //ui->twCustomers->clearSelection();
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

void MainWindow::setArticlePosColumnsWidth() const
{
    ui->twRgArticles->resizeColumnsToContents();

    for (int i = 0; i < ui->twRgArticles->columnCount(); i++)
    {
        int tempWidth = ui->twRgArticles->columnWidth(i);
        ui->twRgArticles->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }
}

void MainWindow::clearArticleEdits()
{
    ui->leArtNr->clear();
    ui->leArtName->clear();
    ui->leArtPrice->clear();
    ui->leArtUnit->clear();
    ui->ptArtDescription->clear();
    ui->Artikel_Erfassen->setEnabled(false);
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
            ui->btnCustomerBill->setEnabled(false);
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
            // Fill customers combobox
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

            // Fill subject line
            int lastRgNr = m_dbManager->readLastID(RECHNUNG);
            ui->leRgSubject->setText("Rechnung " + QString::number(lastRgNr));

            // Fill date
            QDate date;
            ui->deRgDate->setDate(date.currentDate());

            // Fill articles combobox
            // clear articles
            if(ui->cbRgArtikel->count() > 0)
                ui->cbRgArtikel->clear();

            // read articles from database
            std::vector<Articles> articles;
            if (!m_dbManager->readArticles(articles))
            {
               qDebug() << DEBUG_TAG_MAIN << ": Error read customers!";
               return;
            }

            // save all exist articles to combobox
            for (std::vector<Articles>::iterator it = articles.begin(); it != articles.end(); ++it)
            {
               QString article = QString::number(it->getArtNr())+ " - " + it->getName();
               ui->cbRgArtikel->addItem(article);
            }

            clearBillEdits();
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
    int lastRgNr = m_dbManager->readLastID(RECHNUNG);

    ui->leRgSubject->setText("Rechnung " + QString::number(lastRgNr));
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
        if (m_dbManager->editArticle(QString::number(artnr), article))
        {
            printAllArticles();
            QMessageBox::information(this, "Info", "Der Artikel wurde erfolgreich bearbeitet!", QMessageBox::Ok);
            clearArticleEdits();
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim bearbeiten des Artikels!", QMessageBox::Ok);
    }
    else
    {
        // add data entry
        if (m_dbManager->addArticle(article))
        {
            printAllArticles();
            clearArticleEdits();
            QMessageBox::information(this, "Info", "Der Artikel wurde erfolgreich angelegt!", QMessageBox::Ok);
            ui->Artikel_Erfassen->setEnabled(false);
        }
        else
            QMessageBox::critical(this, "Error", "Fehler beim erstellen des Artikels!", QMessageBox::Ok);
    }
}

void MainWindow::on_twArticles_itemClicked(QTableWidgetItem *item)
{
    // read article from database
    QString artNr = ui->twArticles->item(item->row(), ArtNr)->text();
    Articles article;
    if (!m_dbManager->readArticle(artNr, article))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customer!";
       return;
    }

    // print all customer to edits
    ui->Artikel_Erfassen->setEnabled(true);

    ui->leArtNr->setText(QString::number(article.getArtNr()));
    ui->leArtUnit->setText(article.getUnit());
    ui->leArtName->setText(article.getName());
    ui->leArtPrice->setText(QString::number(article.getPrice()));
    ui->ptArtDescription->setPlainText(article.getDescription());
}

void MainWindow::on_btnArtNew_clicked()
{
    clearArticleEdits();
    ui->Artikel_Erfassen->setEnabled(true);
    ui->twArticles->clearSelection();
    ui->leArtNr->setText(QString::number(m_dbManager->readLastID(ARTIKEL) + 1));
}

void MainWindow::on_btnArtCancel_clicked()
{
    ui->twArticles->clearSelection();
    clearArticleEdits();
    ui->Artikel_Erfassen->setEnabled(false);
}

void MainWindow::on_btnArtDelete_clicked()
{
    if(ui->twArticles->selectedItems().count() >  0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie den Artikel wirklich löschen?");
        msg.setWindowTitle("Warnung");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            QString id = ui->twArticles->item(ui->twArticles->currentRow(), 0)->text();
            m_dbManager->removeDbEntry(ARTIKEL, id);
            printAllArticles();
            QMessageBox::information(this, "Info", "Der ausgewählte Artikel wurde erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        qDebug() << "No article selected!";
        QMessageBox::warning(this, "Warnung", "Es wurde kein Artikel zum löschen ausgewählt!", QMessageBox::Ok);
    }
}

void MainWindow::on_btnArtDelAll_clicked()
{
    if (ui->twArticles->rowCount() > 0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie ALLE Artikel wirklich löschen?");
        msg.setWindowTitle("Warnung");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            m_dbManager->removeDBList(ARTIKEL);
            printAllArticles();
            QMessageBox::information(this, "Info", "Alle Artikel wurden erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Keine Artikel zum löschen verfügbar!", QMessageBox::Ok);
    }
}

void MainWindow::on_twCustomers_itemClicked(QTableWidgetItem *item)
{
    ui->btnEditCustomer->setEnabled(true);
    ui->btnDeleteCustomer->setEnabled(true);
    ui->btnCustomerBill->setEnabled(true);
}

void MainWindow::on_btnRgClear_clicked()
{
    clearBillEdits();
    ui->btnRgClear->setEnabled(false);
    ui->btnRgAddArticle->setEnabled(false);
}

void MainWindow::clearBillEdits()
{
    ui->cbRgArtikel->setCurrentIndex(-1);
    ui->leRgArtNr->clear();
    ui->leRgName->clear();
    ui->sbRgCount->setValue(0);
    ui->sbRgSinglePrice->setValue(0);
    ui->sbRgTotalPrice->setValue(0);
    ui->leRgUnit->setText("Stück");
}

void MainWindow::on_cbRgArtikel_currentTextChanged(const QString &name)
{
    QString artNr = name.split(" ").value(0);

    if (!artNr.isEmpty())
    {
        Articles article;
        m_dbManager->readArticle(artNr, article);
        ui->leRgArtNr->setText(artNr);
        ui->leRgName->setText(article.getName());
        ui->leRgUnit->setText(article.getUnit());
        ui->sbRgCount->setValue(1);
        ui->sbRgSinglePrice->setValue(article.getPrice());
        ui->sbRgTotalPrice->setValue(article.getPrice() * ui->sbRgCount->value());
    }
}

void MainWindow::on_sbRgCount_valueChanged(int value)
{
    double totalPriceTemp = ui->sbRgSinglePrice->value() * value;
    ui->sbRgTotalPrice->setValue(totalPriceTemp);
}

void MainWindow::on_btnRgAddArticle_clicked()
{
    QString artNr = ui->leRgArtNr->text();
    QString name = ui->leRgName->text();
    QString count = QString::number(ui->sbRgCount->value());
    QString unit = ui->leRgUnit->text();
    QString sPrice = QString::number(ui->sbRgSinglePrice->value(), 'f', 2) + " €";
    QString tPrice = QString::number(ui->sbRgTotalPrice->value(), 'f', 2) + " €";

    int row = ui->twRgArticles->rowCount();

    ui->twRgArticles->insertRow(row);
    ui->twRgArticles->setRowHeight(row, ARTICLEPOS_ROW_HEIGHT);
    ui->twRgArticles->setItem(row, ArtNrPos, new QTableWidgetItem(artNr));
    ui->twRgArticles->setItem(row, BeschreibungPos, new QTableWidgetItem(name));
    ui->twRgArticles->setItem(row, Anzahl, new QTableWidgetItem(count));
    ui->twRgArticles->setItem(row, EinheitPos, new QTableWidgetItem(unit));
    ui->twRgArticles->setItem(row, EinzelPreis, new QTableWidgetItem(sPrice));
    ui->twRgArticles->item(row, EinzelPreis)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->twRgArticles->setItem(row, Summe, new QTableWidgetItem(tPrice));
    ui->twRgArticles->item(row, Summe)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

    // Set customer table column width offset
    setArticlePosColumnsWidth();

    // Update total price
    updateTotalPrice();

    // Clear bill edits
    clearBillEdits();
    ui->btnRgClear->setEnabled(false);
    ui->btnRgAddArticle->setEnabled(false);
}

void MainWindow::updateTotalPrice()
{
    double summe = 0;
    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        summe += ui->twRgArticles->item(i, Summe)->text().split(" ").value(0).toDouble();
    }
    ui->sbRgSumme->setValue(summe);
}

void MainWindow::on_sbRgSinglePrice_valueChanged(double value)
{
    ui->sbRgTotalPrice->setValue(value * ui->sbRgCount->value());
}

void MainWindow::on_btnRgDeteleAllArticle_clicked()
{
    QMessageBox msg;
    msg.setWindowIcon(QPixmap("logo.png"));
    msg.setText("Möchten Sie ALLE Positionen wirklich löschen?");
    msg.setWindowTitle("Warnung");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setButtonText(QMessageBox::Yes, "Ja");
    msg.setButtonText(QMessageBox::No, "Nein");
    msg.setDefaultButton(QMessageBox::No);
    msg.setIcon(QMessageBox::Question);

    if(msg.exec() == QMessageBox::Yes)
    {
        int rowCount = ui->twRgArticles->rowCount();
        for (int i = 0; i < rowCount; i++)
        {
            ui->twRgArticles->removeRow(0);
        }

        updateTotalPrice();
        QMessageBox::information(this, "Info", "Alle Positionen wurden erfolgreich gelöscht!", QMessageBox::Ok);
    }
}

void MainWindow::on_btnRgDeleteArticle_clicked()
{
    if(ui->twRgArticles->selectedItems().count() >  0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie die Position wirklich löschen?");
        msg.setWindowTitle("Warnung");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            ui->twRgArticles->removeRow(ui->twRgArticles->currentRow());
            ui->twRgArticles->clearSelection();
            updateTotalPrice();
            QMessageBox::information(this, "Info", "Der ausgewählte Artikel wurde erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        qDebug() << "No position selected!";
        QMessageBox::warning(this, "Warnung", "Es wurde keine Position zum löschen ausgewählt!", QMessageBox::Ok);
    }
}

void MainWindow::on_twRgArticles_itemSelectionChanged()
{
    if (ui->twRgArticles->selectedItems().count() > 0)
    {
        ui->btnRgDeleteArticle->setEnabled(true);
    }
    else
    {
        ui->btnRgDeleteArticle->setEnabled(false);
    }
}

void MainWindow::on_cbRgArtikel_activated(int index)
{
    ui->btnRgClear->setEnabled(true);
    ui->btnRgAddArticle->setEnabled(true);
    ui->twRgArticles->clearSelection();
}

void MainWindow::on_leRgName_textChanged(const QString &text)
{
    if (!text.isEmpty())
    {
        ui->btnRgAddArticle->setEnabled(true);
        ui->sbRgCount->setValue(1);
    }
    else
    {
        ui->btnRgAddArticle->setEnabled(false);
        ui->sbRgCount->setValue(0);
    }
}

void MainWindow::on_twRgArticles_itemClicked(QTableWidgetItem *item)
{
    int curRow = item->row();
    ui->leRgArtNr->setText(ui->twRgArticles->item(curRow, ArtNrPos)->text());
    ui->leRgName->setText(ui->twRgArticles->item(curRow, BeschreibungPos)->text());
    ui->sbRgCount->setValue(ui->twRgArticles->item(curRow, Anzahl)->text().toDouble());
    ui->leRgUnit->setText(ui->twRgArticles->item(curRow, EinheitPos)->text());
    ui->sbRgSinglePrice->setValue(ui->twRgArticles->item(curRow, EinzelPreis)->text().split(" ").value(0).toDouble());
    ui->sbRgTotalPrice->setValue(ui->twRgArticles->item(curRow, Summe)->text().split(" ").value(0).toDouble());

}

void MainWindow::on_btnCustomerBill_clicked()
{
    int curRow = ui->twCustomers->currentRow();
    int kdNr =  ui->twCustomers->item(curRow, KdNr)->text().toInt();
    ui->tabMain->setCurrentIndex(CalculationsTab);

    for (int i = 0; i < ui->cbRgCustomer->count(); i++)
    {
        int cbnr = ui->cbRgCustomer->itemText(i).split(" ").value(0).toInt();
        if (kdNr == cbnr)
        {
            ui->cbRgCustomer->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::on_twCustomers_itemSelectionChanged()
{
    ui->btnEditCustomer->setEnabled(false);
    ui->btnDeleteCustomer->setEnabled(false);
    ui->btnCustomerBill->setEnabled(false);
}

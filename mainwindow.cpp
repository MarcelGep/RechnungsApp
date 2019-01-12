
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QLocale>

//#define BORDER_ACTIVE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_posNr(0),
    m_statusLabel(new QLabel())
{
    ui->setupUi(this);

//    StartApp start;
//    int res = start.exec();
//    if(res == QDialog::Rejected)
//        return;
//    else if(res == QDialog::Accepted)
//    {
//        m_dbManager = new DBManager(PATH_DATABASE);
//        QMessageBox::information(this, "Info", "Die Datenbank wurde erfolgreich geladen!", QMessageBox::Ok);
//    }

    // Init and open database
    m_dbManager = new DBManager();
    if(m_dbManager->openDatabase(PATH_DATABASE))
    {
        // Set default tabs
        ui->tabWidgetMain->setCurrentIndex(CustomersTab);
        selectMainTabIndex( CustomersTab );

        ui->tabWidKunden->setCurrentIndex(OverviewTab);

        m_statusLabel->setText(DB_CONNECTED_MESSAGE);
        m_statusLabel->setStyleSheet("QLabel { color : green; }");
    }
    else
    {
        QMessageBox errorMsg;
        errorMsg.setText("Es konnte keine Verbindung zur Datenbank hergestellt werden!");
        errorMsg.setStandardButtons(QMessageBox::Ok);
        errorMsg.setWindowTitle("Fehler Datenkbank");
        errorMsg.setIcon(QMessageBox::Warning);
        errorMsg.exec();

        m_statusLabel->setText(DB_NOTCONNECTED_MESSAGE);
        m_statusLabel->setStyleSheet("QLabel { color : red; }");
    }

    m_statusLabel->setAlignment(Qt::AlignRight);
    ui->statusBar->addPermanentWidget(m_statusLabel, 1);

//    QDoubleValidator* spValidator= new QDoubleValidator(0.0, 9999999999.0, 2, ui->leRgSinglePrice);
//    spValidator->setLocale(QLocale("de_DE"));
//    spValidator->setNotation(QDoubleValidator::StandardNotation);
//    ui->leRgSinglePrice->setValidator(spValidator);
}

MainWindow::~MainWindow()
{
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
    for (int i = 0; i< plz_str.size(); i++)
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

    if (m_dbManager->dbEntryExist(KUNDE, QString::number(kdnr)))
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
            m_dbManager->removeDbEntry(KUNDE, id);
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

    int lastID = m_dbManager->readLastID(KUNDE);
    ui->leKdNr->setText(QString::number(lastID + 1));

    ui->tabWidKunden->setCurrentIndex(EditTab);

    QPixmap pixmap = QPixmap(":/Icons/icons/customer_add.png");
    ui->lb_logoAddEdit->setPixmap(pixmap);

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

        QPixmap pixmap = QPixmap(":/Icons/icons/customer_edit.png");
        ui->lb_logoAddEdit->setPixmap(pixmap);
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
    ui->twCustomers->clearSelection();
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
       ui->twArticles->setItem(row, Article_ArtNr, new QTableWidgetItem(QString::number(it->getArtNr())));
       ui->twArticles->setItem(row, Article_Einheit, new QTableWidgetItem(it->getUnit()));
       ui->twArticles->setItem(row, Article_Beschreibung, new QTableWidgetItem(it->getDescription()));
       ui->twArticles->setItem(row, Article_Preis, new QTableWidgetItem(QLocale().toCurrencyString(it->getPrice())));
       ui->twArticles->item(row, Article_Preis)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
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

void MainWindow::clearInvoices()
{
    while (ui->twRgList->rowCount() > 0)
    {
        ui->twRgList->removeRow(0);
    }
    m_invoices.clear();
}

void MainWindow::clearCustomers()
{
    while (ui->twCustomers->rowCount() > 0)
    {
        ui->twCustomers->removeRow(0);
    }
    m_customers.clear();
}

void MainWindow::printAllInvoices()
{
    // clear invoice list
    if(ui->twRgList->rowCount() > 0)
        clearInvoices();

    // read invoices from database
    if (!m_dbManager->readInvoices(m_invoices))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read invoices!";
       return;
    }

    // print all exist invoices to list
    for (std::vector<Invoices>::iterator it = m_invoices.begin(); it != m_invoices.end(); ++it)
    {
       int row = ui->twRgList->rowCount();
       ui->twRgList->insertRow(row);
       ui->twRgList->setRowHeight(row, INVOICE_ROW_HEIGHT);
       ui->twRgList->setItem(row, Invoice_RgNr, new QTableWidgetItem(QString::number(it->rgnr())));
       ui->twRgList->setItem(row, Invoice_KdNr, new QTableWidgetItem(QString::number(it->kdnr())));
       ui->twRgList->setItem(row, Invoice_RgDate, new QTableWidgetItem(it->rgdate()));
       ui->twRgList->setItem(row, Invoice_Amount, new QTableWidgetItem(QLocale().toCurrencyString(it->amount())));
       ui->twRgList->item(row, Invoice_Amount)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twRgList->setItem(row, Invoice_USt, new QTableWidgetItem(QString::number(it->ust(), 'f', 1) + " %"));
       ui->twRgList->item(row, Invoice_USt)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twRgList->setItem(row, Invoice_Skonto, new QTableWidgetItem(QString::number(it->skonto(), 'f', 1) + " %"));
       ui->twRgList->item(row, Invoice_Skonto)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twRgList->setItem(row, Invoice_Currency, new QTableWidgetItem(it->currency()));
       ui->twRgList->item(row, Invoice_Currency)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    }

    // Set customer table column width offset
    setInvoicesColumnsWidth();

    qDebug() << DEBUG_TAG_MAIN << ": Print all invoices!";

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

void MainWindow::setInvoicesColumnsWidth() const
{
    ui->twRgList->resizeColumnsToContents();

    for (int i = 0; i < ui->twRgList->columnCount(); i++)
    {
        int tempWidth = ui->twRgList->columnWidth(i);
        ui->twRgList->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }
}

void MainWindow::setCustomerColumnsWidth() const
{
    ui->twCustomers->resizeColumnsToContents();

    for (int i = 0; i < ui->twCustomers->columnCount(); i++)
    {
        int tempWidth = ui->twCustomers->columnWidth(i);
        ui->twCustomers->setColumnWidth(i, tempWidth + CUSTOMER_COLUMN_OFFSET);
    }

    int width = ui->twCustomers->horizontalHeader()->length()+ui->twCustomers->verticalHeader()->width();

    ui->twCustomers->setMinimumWidth(width);
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

            QPixmap pixmap = QPixmap(":/Icons/icons/customer_edit.png");
            ui->lb_logoAddEdit->setPixmap(pixmap);
        }
        else
        {
            int lastID = m_dbManager->readLastID(KUNDE);
            ui->leKdNr->setText(QString::number(lastID + 1));

            QPixmap pixmap = QPixmap(":/Icons/icons/customer_add.png");
            ui->lb_logoAddEdit->setPixmap(pixmap);
        }
    }
    else if(index == OverviewTab)
    {

    }
}

void MainWindow::selectMainTabIndex(int index)
{
    switch(index)
    {
        case CustomersTab:
        {
            // Setup customer list
            setupCustomerList();

            // Setup customer list
            printAllCustomers();
            ui->btnCustomerBill->setEnabled(false);
        }
        break;

        case ArticlesTab:
        {
            // Setup article list
            setupArticleList();

            printAllArticles();
        }
        break;

        case CalculationsTab:
        {
            // Setup article position list
            setupArticlePositionList();

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

            // Fill invoice number
            int lastRgNr = m_dbManager->readLastID(RECHNUNG);
            ui->leRgNr->setText(QString::number(lastRgNr + 1));

            // Fill subject line
            ui->leRgSubject->setText("Rechnung");

            // Fill date
            QDate date;
            ui->deRgDate->setDate(date.currentDate());

            // Fill delivery date
            ui->deLiefDate->setDate(date.currentDate());

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
               QString article = QString::number(it->getArtNr())+ " - " + it->getDescription();
               ui->cbRgArtikel->addItem(article);
            }

            clearBillEdits();

            ui->tabWidgetInvoice->setCurrentIndex(0);

            // clear customer
            ui->cbRgCustomer->setCurrentIndex(-1);
        }
        break;

        case SettingsTab:
        {
            // Fill pdf settings list
            ui->lwSetPdfPhrases->clear();

            m_dbManager->getSettingsFields();
            for (int i = 0; i < m_dbManager->getSettingsFields().size(); i++)
            {
                QListWidgetItem *item = new QListWidgetItem();
                item->setSizeHint(QSize(0, 30));
                item->setText(m_dbManager->getSettingsFields().value(i));
                ui->lwSetPdfPhrases->addItem(item);
            }

            ui->lwSetPdfPhrases->setCurrentRow(0);
            ui->lwSetPdfPhrases->setFocus();

            QString data;
            m_dbManager->readSetting(ui->lwSetPdfPhrases->item(0)->text(), data);
            ui->teSetContent->setText(data);
        }
        break;

        default:
            break;
    }
}

void MainWindow::on_tabWidgetMain_currentChanged(int index)
{
    selectMainTabIndex(index);
}

void MainWindow::on_btnRgRechnung_clicked()
{
    ui->leRgSubject->setText("Rechnung");
    ui->btnRgRechnung->setChecked(true);
    ui->btnRgAngebot->setChecked(false);
    ui->btnRgGutschrift->setChecked(false);
}

void MainWindow::on_btnRgAngebot_clicked()
{
    ui->leRgSubject->setText("Angebot");
    ui->btnRgRechnung->setChecked(false);
    ui->btnRgAngebot->setChecked(true);
    ui->btnRgGutschrift->setChecked(false);
}

void MainWindow::on_btnRgGutschrift_clicked()
{
    ui->leRgSubject->setText("Gutschrift");
    ui->btnRgRechnung->setChecked(false);
    ui->btnRgAngebot->setChecked(false);
    ui->btnRgGutschrift->setChecked(true);
}

void MainWindow::on_btnArtNew_clicked()
{
    articleAdd();
}

void MainWindow::on_btnArtDelete_clicked()
{
    articleDelete();
}

void MainWindow::on_btnArtDelAll_clicked()
{
    if (ui->twArticles->rowCount() > 0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Möchten Sie wirklich ALLE Artikel löschen?");
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

void MainWindow::on_btnRgClear_clicked()
{
    clearBillEdits();
    ui->btnRgClear->setEnabled(false);
    ui->btnRgAddArticle->setEnabled(false);
    ui->leRgArtNr->setFocus();
}

void MainWindow::clearBillEdits()
{
    ui->cbRgArtikel->setCurrentIndex(-1);
    ui->leRgArtNr->clear();
    ui->leRgName->clear();
    ui->sbRgCount->setValue(1);
    ui->leRgSinglePrice->clear();
    ui->leRgTotalPrice->clear();
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
        ui->leRgName->setText(article.getDescription());
        ui->leRgUnit->setText(article.getUnit());
        ui->leRgSinglePrice->setText(QLocale().toCurrencyString(article.getPrice()).split(" ").value(0));
        ui->leRgTotalPrice->setText(QLocale().toCurrencyString(article.getPrice() * ui->sbRgCount->value()).split(" ").value(0));
    }
}

void MainWindow::on_sbRgCount_valueChanged(int value)
{
    QString singlePrice = ui->leRgSinglePrice->text().split(" ").value(0);
    double totalPriceTemp = singlePrice.replace(".", "").replace(",",".").toDouble() * value;
    ui->leRgTotalPrice->setText(QLocale().toCurrencyString(totalPriceTemp).split(" ").value(0));
}

void MainWindow::on_btnRgAddArticle_clicked()
{
    QString artNr = ui->leRgArtNr->text();
    QString name = ui->leRgName->text();
    QString count = QString::number(ui->sbRgCount->value());
    QString unit = ui->leRgUnit->text();
    QString sPrice = ui->leRgSinglePrice->text().replace(".", "").replace(",",".");
    QString tPrice = ui->leRgTotalPrice->text().replace(".", "").replace(",",".");

    // search for exist entries
    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        if (name == ui->twRgArticles->item(i, BeschreibungPos)->text())
        {
            int ret = QMessageBox::question(this,
                                            "Vorsicht",
                                            "Der Artikel befindet sich bereits in der Liste!\n\n"
                                            "Soll die Stückzahl addiert werden?",
                                            QMessageBox::Yes | QMessageBox:: No,
                                            QMessageBox::No);

            if (ret == QMessageBox::Yes)
            {
                int count = ui->twRgArticles->item(i, AnzahlPos)->text().toInt();
                ui->twRgArticles->item(i, AnzahlPos)->setText(QString::number(count + ui->sbRgCount->value()));
            }

            clearBillEdits();
            return;
        }
    }

    int row = ui->twRgArticles->rowCount();
    m_posNr = row;

    ui->twRgArticles->insertRow(row);
    ui->twRgArticles->setRowHeight(row, ARTICLEPOS_ROW_HEIGHT);
    ui->twRgArticles->setItem(row, PosNr, new QTableWidgetItem(QString::number(++m_posNr)));
    ui->twRgArticles->setItem(row, ArtNrPos, new QTableWidgetItem(artNr));
    ui->twRgArticles->setItem(row, BeschreibungPos, new QTableWidgetItem(name));
    ui->twRgArticles->setItem(row, AnzahlPos, new QTableWidgetItem(count));
    ui->twRgArticles->setItem(row, EinheitPos, new QTableWidgetItem(unit));
    ui->twRgArticles->setItem(row, EinzelPreisPos, new QTableWidgetItem(QLocale().toCurrencyString(sPrice.toDouble())));
    ui->twRgArticles->item(row, EinzelPreisPos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->twRgArticles->setItem(row, SummePos, new QTableWidgetItem(QLocale().toCurrencyString(tPrice.toDouble())));
    ui->twRgArticles->item(row, SummePos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
    int rowCount = ui->twRgArticles->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        QString sumStr = ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0);
        sumStr = sumStr.replace(".", "");
        sumStr = sumStr.replace(",", ".");

        double sum = sumStr.toDouble();
        summe += sum;
    }
    ui->leRgSumme->setText(QLocale().toCurrencyString(summe));
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
        ui->btnRgDeteleAllArticle->setEnabled(false);
        ui->btnRgCreate->setEnabled(false);
        m_posNr = 0;
//        QMessageBox::information(this, "Info", "Alle Positionen wurden erfolgreich gelöscht!", QMessageBox::Ok);
    }
}

void MainWindow::on_btnRgDeleteArticle_clicked()
{
    if(ui->twRgArticles->selectedItems().count() >  0)
    {
        QString currRow = QString::number(ui->twRgArticles->currentRow() + 1);

        QMessageBox msg;
        msg.setWindowIcon(QPixmap("logo.png"));
        msg.setText("Position " + currRow + " wirklich löschen?");
        msg.setWindowTitle("Warnung");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            int currRow = ui->twRgArticles->currentRow();
            ui->twRgArticles->removeRow(currRow);
            ui->twRgArticles->clearSelection();
            updateTotalPrice();

            for( int i = currRow; i < ui->twRgArticles->rowCount(); i++)
            {
                ui->twRgArticles->setItem(i, PosNr, new QTableWidgetItem(QString::number(++currRow)));
            }
//            QMessageBox::information(this, "Info", "Der ausgewählte Artikel wurde erfolgreich gelöscht!", QMessageBox::Ok);

            if(ui->twRgArticles->rowCount() == 0)
            {
                ui->btnRgDeteleAllArticle->setEnabled(false);
            }
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

void MainWindow::on_cbRgArtikel_activated(int /*index*/)
{
    ui->btnRgClear->setEnabled(true);
    ui->btnRgAddArticle->setEnabled(true);
    ui->twRgArticles->clearSelection();
}

void MainWindow::on_leRgName_textChanged(const QString &text)
{
    if (!text.isEmpty() && !ui->leRgArtNr->text().isEmpty())
    {
        ui->btnRgAddArticle->setEnabled(true);
        ui->btnRgClear->setEnabled(true);
    }
    else
    {
        ui->btnRgAddArticle->setEnabled(false);
        ui->btnRgClear->setEnabled(false);
    }
}

void MainWindow::on_btnCustomerBill_clicked()
{
    int curRow = ui->twCustomers->currentRow();
    int kdNr =  ui->twCustomers->item(curRow, KdNr)->text().toInt();
    ui->tabWidgetMain->setCurrentIndex(CalculationsTab);
    ui->tabWidgetInvoice->setCurrentIndex(0);

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
    if (ui->twCustomers->selectedItems().count() > 0)
    {
        ui->btnEditCustomer->setEnabled(true);
        ui->btnDeleteCustomer->setEnabled(true);
        ui->btnCustomerBill->setEnabled(true);
    }
    else
    {
        ui->btnEditCustomer->setEnabled(false);
        ui->btnDeleteCustomer->setEnabled(false);
        ui->btnCustomerBill->setEnabled(false);
    }
}

void MainWindow::on_leRgArtNr_returnPressed()
{
    QString artNr = ui->leRgArtNr->text();

    Articles article;
    if (m_dbManager->readArticle(artNr, article))
    {
        ui->leRgName->setText(article.getDescription());
        ui->leRgUnit->setText(article.getUnit());
        ui->leRgSinglePrice->setText(QLocale().toCurrencyString(article.getPrice()).split(" ").value(0));
        ui->leRgTotalPrice->setText(QLocale().toCurrencyString(article.getPrice() * 1).split(" ").value(0));

        ui->sbRgCount->setFocus();
        ui->sbRgCount->selectAll();
        ui->btnRgClear->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Es wurde kein Aritkel gefunden!", QMessageBox::Ok);
        ui->leRgArtNr->clear();
    }
}

void MainWindow::on_sbRgCount_editingFinished()
{
    ui->leRgUnit->setFocus();
    ui->leRgUnit->selectAll();
}

void MainWindow::on_leRgUnit_returnPressed()
{
    ui->leRgSinglePrice->setFocus();
    ui->leRgSinglePrice->selectAll();
}

void MainWindow::on_twRgArticles_itemChanged(QTableWidgetItem *item)
{
    if (ui->twRgArticles->rowCount() > 0)
    {
        ui->btnRgDeteleAllArticle->setEnabled(true);
        ui->btnRgCreate->setEnabled(true);

        QString value = item->text();

        if(item->column() == EinzelPreisPos)
        {
            double price = 0;

            if(value.contains("€"))
            {
                value = value.split(" ").value(0);
            }

            if(value.contains("."))
            {
                price = value.toDouble();
            }
            else if(value.contains(","))
            {
                price = value.replace(",", ".").toDouble();
            }
            else
            {
                price = value.toDouble();
            }

            double summe = price * ui->twRgArticles->item(item->row(), AnzahlPos)->text().toDouble();
            ui->twRgArticles->setItem(item->row(), SummePos, new QTableWidgetItem(QLocale().toCurrencyString(summe).split(" ").value(0)));
//            ui->twRgArticles->setItem(item->row(), EinzelPreisPos, new QTableWidgetItem(QLocale().toCurrencyString(price)));
        }
        else if(item->column() == AnzahlPos)
        {
            int anzahl = value.toInt();
//            double price = ui->twRgArticles->item(item->row(), EinzelPreisPos)->text().split(" ").value(0).replace(",", ".").toDouble();
//            ui->twRgArticles->setItem(item->row(), SummePos, new QTableWidgetItem(QLocale().toCurrencyString(price * anzahl)));
        }
    }
}

void MainWindow::on_btnRgCreate_clicked()
{
    // check all inputs
    if( ui->cbRgCustomer->currentIndex() == -1)
    {
        QMessageBox::warning(this, "Eingabefehler", "Es wurde kein Kunde angegeben!", QMessageBox::Ok);
        return;
    }

    if( ui->twRgArticles->rowCount() <= 0)
    {
        QMessageBox::warning(this, "Eingabefehler", "Es wurde kein Artikel erfasst!", QMessageBox::Ok);
        return;
    }

    // Add positions to DB
    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        int pos = ui->twRgArticles->item(i, PosNr)->text().toInt();
        int rgnr = ui->leRgNr->text().toInt();
        int artnr = ui->twRgArticles->item(i, ArtNrPos)->text().toInt();
        int menge = ui->twRgArticles->item(i, AnzahlPos)->text().toInt();
        QString beschreibung = ui->twRgArticles->item(i, BeschreibungPos)->text();
        QString einheit = ui->twRgArticles->item(i, EinheitPos)->text();
        double preis = ui->twRgArticles->item(i, EinzelPreisPos)->text().split(" ").value(0).replace(".", "").replace(",", ".").toDouble();
        double total = ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0).replace(".", "").replace(",", ".").toDouble();

        Positions position(pos, rgnr, artnr, beschreibung, einheit, menge, preis, total);

        m_dbManager->addPosition(position);
    }

    // Add invoice to DB
    int kdnr = ui->cbRgCustomer->currentText().split(" ").value(0).toInt();
    QString rgdate = ui->deRgDate->text();
    QString amount_str = ui->leRgSumme->text().split(" ").value(0);
    double amount = amount_str.replace(".", "").replace(",", ".").toDouble();
    int ust = ui->sbRgUst->value();
    int skonto = ui->sbRgSkonto->value();
    QString currency = ui->leRgCurrency->text();

    Invoices invoice(0, kdnr, rgdate, amount, ust, skonto, currency);

    m_dbManager->addInvoice(invoice);

    // Create the invoice to PDF
    QStringList dateList = ui->deRgDate->text().split(".");
    QString date = dateList[dateList.size()-1] + dateList[dateList.size()-2] + dateList[dateList.size()-3];

    QString kdNr = ui->cbRgCustomer->currentText().split(" ").value(0);
    QString subject = ui->leRgSubject->text();
    QString rgNr = ui->leRgNr->text();

    std::vector<Positions> positions;

    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        Positions position;
        position.setPos(ui->twRgArticles->item(i, PosNr)->text().toInt());
        position.setArtnr(ui->twRgArticles->item(i, ArtNrPos)->text().toInt());
        position.setBeschreibung(ui->twRgArticles->item(i, BeschreibungPos)->text());
        position.setMenge(ui->twRgArticles->item(i, AnzahlPos)->text().toInt());
        position.setEinheit(ui->twRgArticles->item(i, EinheitPos)->text());
        position.setPrice(ui->twRgArticles->item(i, EinzelPreisPos)->text().split(" ").value(0).replace(".", "").replace(",", ".").toDouble());
        position.setTotal(ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0).replace(".", "").replace(",", ".").toDouble());

        positions.push_back(position);
    }

    QString summe = ui->leRgSumme->text();

    createInvoice(rgNr, kdNr, date, subject, positions, summe, CreateInvoice);

    // Clean article positions table
    int rowCount = ui->twRgArticles->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        ui->twRgArticles->removeRow(0);
    }

    updateTotalPrice();
    ui->btnRgDeteleAllArticle->setEnabled(false);
//    ui->btnRgCreate->setEnabled(false);
    m_posNr = 0;

    // Fill invoice number
    int lastRgNr = m_dbManager->readLastID(RECHNUNG);
    ui->leRgNr->setText(QString::number(lastRgNr + 1));

    // Clear customer
    ui->cbRgCustomer->setCurrentIndex(-1);

    //QMessageBox::information(this, "Info", "Die Rechnung wurde erstellt! ", QMessageBox::Ok | QMessageBox::Close);
    ui->tabWidgetInvoice->setCurrentIndex(InvoicesTab);
}

void MainWindow::fillSettingsEdit()
{

}

void MainWindow::readSettingsEdit()
{

}

void MainWindow::createInvoice(QString rgNr, QString kdNr, QString date, QString subjectLine, std::vector<Positions> positions, QString summe, InvoiceTyp typ)
{
    QString outPath = PATH_PDF;
    if(!QDir(outPath).exists())
    {
        QDir().mkdir(outPath);
    }

    QString outputName = outPath + "Rechnung_" + rgNr + "_" + date + ".pdf";

    QPainter* m_painter = new QPainter();

    QPrinter* m_pdfPrinter = new QPrinter(QPrinter::HighResolution);
    m_pdfPrinter->setOutputFormat(QPrinter::PdfFormat);
    m_pdfPrinter->setOutputFileName(outputName);
    m_pdfPrinter->setPaperSize(QPrinter::A4);
    m_pdfPrinter->setPageMargins(QMarginsF(30, 30, 30, 30));

    QString receiver;
    Customers customer;
    m_dbManager->readCustomer(kdNr, customer);
    receiver += customer.getFirma() + "\n";
    receiver += customer.getName1() + " " + customer.getName2() + "\n";
    receiver += customer.getStrasse() + "\n";
    receiver += QString::number(customer.getPlz()) + " " + customer.getOrt();

    QString invoiceNrLabel("Rechnungs-Nr.:");
    QString invoiceDateLabel("Rechnungsdatum:");
    QString deliveryDateLabel("Lieferdatum:");
    QString deliveryNrLabel("Lieferschein-Nr.:");
    QString customerNrLabel("Kunden-Nr.:");

    // Header fonts
    QFont font("Times");
    font.setPixelSize(DEFAULT_FONT_SIZE);

    QFont senderSmallFont("Times");
    senderSmallFont.setPixelSize(SENDER_SMALL_FONT_SIZE);

    QFont subjectFont("Times");
    subjectFont.setPixelSize(SUBJECT_FONT_SIZE);
    subjectFont.setBold(true);

    QFont posHeaderFont("Times");
    posHeaderFont.setPixelSize(DEFAULT_FONT_SIZE);
    posHeaderFont.setBold(true);

    QFont ustFont("Times");
    ustFont.setPixelSize(DEFAULT_FONT_SIZE);
    ustFont.setBold(true);

    QFont accountFont("Times");
    accountFont.setPixelSize(ACCOUNT_FONT_SIZE);

    // Start m_painter
    if (!m_painter->begin(m_pdfPrinter))
    {
        qWarning("failed to open file, is it writeable?");
        QMessageBox warningBox;
        warningBox.setText("Fehler beim öffnen der PDF Datei! Schließen Sie bitte eine bereits geöffnete Datei!");
        warningBox.setWindowTitle("Schreibfehler");
        warningBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        warningBox.setButtonText(QMessageBox::Retry, "Wiederholen");
        warningBox.setButtonText(QMessageBox::Cancel, "Abbrechen");
        warningBox.setIcon(QMessageBox::Warning);

        if(warningBox.exec() == QMessageBox::Retry)
        {
            createInvoice(rgNr, kdNr, date, subjectLine, positions, summe, typ);
        }

        return;
    }

    // Pens
    QPen penDefault = m_painter->pen();

    QPen penLine;
    penLine.setColor(Qt::black);
    penLine.setWidth(12);

    QPen penLineThin;
    penLineThin.setColor(Qt::black);
    penLineThin.setWidth(8);

    // Create company Logo
    QImage logo(":/Images/logo/tpt_logo.png");
    QImage scaledLogo = logo.scaled(static_cast<int>(logo.width() * 1.5), static_cast<int>(logo.height() * 1.5), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Define coordinates
    int w = m_pdfPrinter->pageRect().width();
    int h = m_pdfPrinter->pageRect().height();
    int x = 0;
    int y = 0;
    int lMargin = static_cast<int>(w * 0.05);
    int rMargin = static_cast<int>(w * 0.04);
    int tMargin = 0;
    int bMargin = 0;
    int spaceData = 100;
    int tabWidth = 170;

    int x_posLeft = x + lMargin;
    int x_posRight = w - rMargin;
    int x_posLogo = (w - scaledLogo.width() - rMargin);
    int x_posSender = x_posLogo + (scaledLogo.width() / 2) / 3;
    int y_posTop = tMargin;
    int y_posLine = y_posTop + scaledLogo.height() + 150;
    int y_posSender = y_posLine + 300;
    int y_posSenderSmall = y_posLine + 300;
    int y_posReceiver = y_posSenderSmall + 300;
    int y_posSubject = y_posReceiver + (w / 5) + 500;
    int pageContent = x_posRight - x_posLeft;

    // Set Font Metrics
    QFontMetrics metricFont(font);
    QFontMetrics metricSmallFont(senderSmallFont);
    QFontMetrics metricSubjectFont(subjectFont);
    QFontMetrics metricPosHeaderFont(posHeaderFont);
    QFontMetrics metricAccountFont(accountFont);

    m_painter->setFont(senderSmallFont);

    // Draw Company Logo
    QRect rectLogo(x_posLogo, y_posTop, scaledLogo.width(), scaledLogo.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectLogo);
    #endif
    m_painter->drawImage(rectLogo, scaledLogo);

    // Separation Line
    m_painter->setPen(penLineThin);
    m_painter->drawLine(x_posLeft, y_posLine, x_posRight, y_posLine);
    m_painter->setPen(penDefault);

    // Small Sender
    QRect rectSenderSmall(x_posLeft, y_posSenderSmall, metricSmallFont.width(getSettings(Anschrift)), metricSmallFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSenderSmall);
    #endif
    m_painter->drawText(rectSenderSmall, getSettings(Anschrift));

    m_painter->setFont(subjectFont);

    // Subject Line
    QRect rectSubject(x_posLeft, y_posSubject, w - rMargin - lMargin, metricSubjectFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSubject);
    #endif
    m_painter->drawText(rectSubject, Qt::AlignTop | Qt::AlignLeft, subjectLine);

    m_painter->setFont(font);

    // Sender
    QRect rectSender(x_posSender, y_posSender, (x_posRight - x_posSender), metricFont.height() * 10);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSender);
    #endif
    m_painter->drawText(rectSender, Qt::AlignTop | Qt::AlignLeft, getSettings(Kontakt));

    // Receiver
    QRect rectReceiver(x_posLeft, y_posReceiver, w / 3, metricFont.height() * 8);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectReceiver);
    #endif
    m_painter->drawText(rectReceiver, Qt::AlignTop | Qt::AlignLeft, receiver);

    y = y_posSubject;
    y += 500;

    // Invoice & Delivery No.
    QRect rectRgLfNrLabel(x_posLeft, y, metricFont.width(deliveryNrLabel), metricFont.height() * 2);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfNrLabel);
    #endif
    m_painter->drawText(rectRgLfNrLabel, Qt::AlignLeft | Qt::AlignTop, invoiceNrLabel);

    if( !ui->leLiefNr->text().isEmpty())
    {
        m_painter->drawText(rectRgLfNrLabel, Qt::AlignLeft | Qt::AlignBottom, deliveryNrLabel);
    }

    QRect rectRgLfNrData(x_posLeft + metricFont.width(deliveryNrLabel) + spaceData, y, 1400, metricFont.height() * 2);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfNrData);
    #endif
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignTop, rgNr);
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignBottom, ui->leLiefNr->text());

    // Invoice & Delivery Dates & Customer Nr.
    QRect rectRgLfDateLabel(x_posSender, y, metricFont.width(invoiceDateLabel), metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfDateLabel);
    #endif
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignTop, invoiceDateLabel);
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignVCenter, deliveryDateLabel);
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignBottom, customerNrLabel);

    QRect rectRgLfDateData(x_posSender + metricFont.width(invoiceDateLabel) + spaceData, y, (x_posRight - x_posSender) - metricFont.width(invoiceDateLabel) - 100, metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfDateData);
    #endif
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignTop, m_settings.getDateStr());
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignVCenter, ui->deLiefDate->text());
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignBottom, kdNr);

    y += 1100;

    // Free Text
    QRect rectFreeText(x_posLeft, y, pageContent, 500);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectFreeText);
    #endif
    m_painter->drawText(rectFreeText, getSettings(FreeText));

    // Position List
    /* Header */
    x = x_posLeft;
    y += 700;

    m_painter->setPen(penLine);
    m_painter->drawLine(x, y, x_posRight, y);

    int y_posHeader = y + 150;
    int y_posSecLine = y_posHeader + metricPosHeaderFont.height() + 150;

    m_painter->drawLine(x, y_posSecLine, x_posRight, y_posSecLine);

    y = y_posSecLine;

    m_painter->setPen(penDefault);

    y += 200;

    /* Data */
    int maxLenPosNr = metricFont.width("0000");
    int maxLenArtNr = metricFont.width("00000000");
    int maxLenCount = metricFont.width("000000000");
    int maxLenUnit = metricFont.width("00000000");
    int maxLenPrice = metricFont.width("0000000000000");
    int maxLenSumme = metricFont.width("0000000000000");
    int dataLenght = maxLenPosNr + maxLenArtNr + maxLenCount + maxLenUnit + maxLenPrice + maxLenSumme + (6 * tabWidth);
    int maxLenDescription = pageContent - dataLenght;

    // Position Label
    int x_position = x;
    QRect rectPosLabel(x_position, y_posHeader, maxLenPosNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPosLabel);
    #endif
    m_painter->drawText(rectPosLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(Position_PosNr));

    x += rectPosLabel.width() + tabWidth;

    // ArtNr Label
    int x_artNr = x;
    QRect rectArtNrLabel(x_artNr, y_posHeader, maxLenArtNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectArtNrLabel);
    #endif
    m_painter->drawText(rectArtNrLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(Position_ArtNr));

    x += rectArtNrLabel.width() + tabWidth;

    // Beschreibung Label
    int x_description = x;
    QRect rectDescriptionLabel(x_description, y_posHeader, maxLenDescription, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectDescriptionLabel);
    #endif
    m_painter->drawText(rectDescriptionLabel, Qt::AlignLeft, m_dbManager->getArticleFields().value(Article_Beschreibung));

    x += rectDescriptionLabel.width() + tabWidth;

    // Anzahl Label
    int x_count = x;
    QRect rectCountLabel(x_count, y_posHeader, maxLenCount, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectCountLabel);
    #endif
    m_painter->drawText(rectCountLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(Position_Menge));

    int x_totalLabel = x;
    x += rectCountLabel.width() + tabWidth;

    // Einheit Label
    int x_unit = x;
    QRect rectUnitLabel(x_unit, y_posHeader, maxLenUnit, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectUnitLabel);
    #endif
    m_painter->drawText(rectUnitLabel, Qt::AlignHCenter, m_dbManager->getArticleFields().value(Article_Einheit));

    x += rectUnitLabel.width() + tabWidth;

    // Preis Label
    int x_price = x;
    QRect rectPriceLabel(x_price, y_posHeader, maxLenPrice, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPriceLabel);
    #endif
    m_painter->drawText(rectPriceLabel, Qt::AlignRight, m_dbManager->getArticleFields().value(Article_Preis));

    x += rectPriceLabel.width() + tabWidth;

    // Summe Label
    int x_summe = x;
    QRect rectSummeLabel(x_summe, y_posHeader, maxLenSumme, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSummeLabel);
    #endif
    m_painter->drawText(rectSummeLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(Position_Gesamtpreis));

    y = y_posSecLine + 200;

    for (uint i = 0; i < positions.size(); i++)
    {
        QString posNrStr = QString::number(positions[i].getPos());
        QString artNrStr = QString::number(positions[i].getArtnr());
        QString labelStr = positions[i].getBeschreibung();
        QString countStr = QString::number(positions[i].getMenge());
        QString unitStr = positions[i].getEinheit();
        QString priceStr = QLocale().toCurrencyString(positions[i].getPrice());
        QString summeStr = QLocale().toCurrencyString(positions[i].getTotal());

        int rows = qRound((double(metricFont.width(labelStr)) / double(maxLenDescription)) + 0.49);

        // Position
        QRect rectPosNr(x_position, y, maxLenPosNr, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectPosNr);
        #endif
        m_painter->drawText(rectPosNr, Qt::AlignLeft | Qt::TextWordWrap, posNrStr);

        // ArtNr
        QRect rectArtNr(x_artNr, y, maxLenArtNr, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectArtNr);
        #endif
        m_painter->drawText(rectArtNr, Qt::AlignLeft | Qt::TextWordWrap, artNrStr);

        // Beschreibung
        QRect rectDescription(x_description, y, maxLenDescription, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectDescription);
        #endif
        m_painter->drawText(rectDescription, Qt::AlignLeft | Qt::TextWordWrap, labelStr);

        // Anzahl
        QRect rectCount(x_count, y, maxLenCount, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectCount);
        #endif
        m_painter->drawText(rectCount, Qt::AlignRight | Qt::TextWordWrap, countStr);

        // Einheit
        QRect rectUnit(x_unit, y, maxLenUnit, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectUnit);
        #endif
        m_painter->drawText(rectUnit, Qt::AlignHCenter | Qt::TextWordWrap, unitStr);

        // Preis
        QRect rectPrice(x_price, y, maxLenPrice, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectPrice);
        #endif
        m_painter->drawText(rectPrice, Qt::AlignRight | Qt::TextWordWrap, priceStr);

        // Summe
        QRect rectSumme(x_summe, y, maxLenSumme, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectSumme);
        #endif
        m_painter->drawText(rectSumme, Qt::AlignRight | Qt::TextWordWrap, summeStr);

        y += metricFont.height() * rows + 200;
    }

    y += 100;

    // Trennlinie
    m_painter->setPen(penLine);
    m_painter->drawLine(x_totalLabel, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    // Gesamtsumme
    QString totalStr = summe;
    QRect rectTotal(x_totalLabel, y, x_posRight - x_totalLabel, metricPosHeaderFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectTotal);
    #endif
    m_painter->setFont(posHeaderFont);
    m_painter->drawText(rectTotal, Qt::AlignLeft, "Rechungsbetrag: ");
    m_painter->drawText(rectTotal, Qt::AlignRight, totalStr);
    m_painter->setFont(font);

    // Conclusion part
    y = h - bMargin - (metricFont.height() * 3) - (metricAccountFont.height()) - 250;

    /* USt & Thx */
    QRect rectConclusion(x_posLeft, y, pageContent, metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectConclusion);
    #endif
    m_painter->setFont(ustFont);
    m_painter->drawText(rectConclusion, Qt::AlignTop | Qt::AlignLeft, getSettings(USt));
    m_painter->setFont(font);
    m_painter->drawText(rectConclusion, Qt::AlignBottom | Qt::AlignLeft, getSettings(Thx));

    y += metricFont.height() + 600;

    m_painter->setPen(penLineThin);
    m_painter->drawLine(x_posLeft, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    /* Steuernummer */
    QRect rectSteuerNr(x_posLeft, y, pageContent, metricAccountFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSteuerNr);
    #endif
    m_painter->setFont(accountFont);
    m_painter->drawText(rectSteuerNr, Qt::AlignBottom | Qt::AlignHCenter, getSettings(Steuernummer));

    y += metricFont.height() + 10;

    /* Kontoinformationen */
    QRect rectAccountInfo(x_posLeft, y, pageContent, metricAccountFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectAccountInfo);
    #endif
    m_painter->setFont(accountFont);
    m_painter->drawText(rectAccountInfo, Qt::AlignBottom | Qt::AlignHCenter, getSettings(Konto));

    m_painter->end();

    QMessageBox msgBox;
//    QPushButton *showButton = msgBox.addButton(tr("PDF "), QMessageBox::ActionRole);
//    msgBox.addButton(QMessageBox::Close);
    msgBox.setStandardButtons(QMessageBox::Ok);
//    msgBox.setButtonText(QMessageBox::Close, "Schließen");
//    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setIcon(QMessageBox::Information);

    if( typ == CreateInvoice )
    {
        msgBox.setWindowTitle("Erstellung PDF");
        msgBox.setText("Die Rechnung wurde erstellt!");
    }
    else if( typ == EditInvoice )
    {
        msgBox.setWindowTitle("Bearbeitung PDF");
        msgBox.setText("Die Rechnung wurde geändert!");
    }

    msgBox.exec();

//    if (msgBox.clickedButton() == showButton)
//    {
//        QPdfDocument *document = new QPdfDocument;
//        document->load("/invoice/Rechnung_1107_20180831.pdf");

//        QPdfView *view = new QPdfView;
//        view->setDocument(document);
//        view->show();

//        QPrinter printer;
//        QPrintDialog printDialog(&printer, this);
//        if (printDialog.exec() == QDialog::Accepted)
//        {
//            // print ...
//        }
//    }

    delete m_pdfPrinter;
    delete m_painter;

}

void MainWindow::createInvoice()
{
    QString outPath = PATH_PDF;

    if(!QDir(PATH_PDF).exists())
    {
        QDir().mkdir(PATH_PDF);
    }

    QStringList dateList = ui->deRgDate->text().split(".");
    QString date = dateList[dateList.size()-1] + dateList[dateList.size()-2] + dateList[dateList.size()-3];
    QString outputName = outPath + "Rechnung_" + ui->leRgNr->text() + "_" + date + ".pdf";

    QPainter* m_painter = new QPainter();

    QPrinter* m_pdfPrinter = new QPrinter(QPrinter::HighResolution);
    m_pdfPrinter->setOutputFormat(QPrinter::PdfFormat);
    m_pdfPrinter->setOutputFileName(outputName);
    m_pdfPrinter->setPaperSize(QPrinter::A4);
    m_pdfPrinter->setPageMargins(QMarginsF(30, 30, 30, 30));

    QString receiver;
    QString KdNr = ui->cbRgCustomer->currentText().split(" ").value(0);
    Customers customer;
    m_dbManager->readCustomer(KdNr, customer);
    receiver += customer.getFirma() + "\n";
    receiver += customer.getName1() + " " + customer.getName2() + "\n";
    receiver += customer.getStrasse() + "\n";
    receiver += QString::number(customer.getPlz()) + " " + customer.getOrt();

    QString invoiceNrLabel("Rechnungs-Nr.:");
    QString invoiceDateLabel("Rechnungsdatum:");
    QString deliveryDateLabel("Lieferdatum:");
    QString deliveryNrLabel("Lieferschein-Nr.:");
    QString customerNrLabel("Kunden-Nr.:");

    // Header fonts
    QFont font("Times");
    font.setPixelSize(DEFAULT_FONT_SIZE);

    QFont senderSmallFont("Times");
    senderSmallFont.setPixelSize(SENDER_SMALL_FONT_SIZE);

    QFont subjectFont("Times");
    subjectFont.setPixelSize(SUBJECT_FONT_SIZE);
    subjectFont.setBold(true);

    QFont posHeaderFont("Times");
    posHeaderFont.setPixelSize(DEFAULT_FONT_SIZE);
    posHeaderFont.setBold(true);

    QFont ustFont("Times");
    ustFont.setPixelSize(DEFAULT_FONT_SIZE);
    ustFont.setBold(true);

    QFont accountFont("Times");
    accountFont.setPixelSize(ACCOUNT_FONT_SIZE);

    // Start m_painter
    if (!m_painter->begin(m_pdfPrinter))
    {
        qWarning("failed to open file, is it writeable?");
        QMessageBox warningBox;
        warningBox.setText("Fehler beim öffnen der PDF Datei! Schließen Sie bitte eine schon geöffnete Datei!");
        warningBox.setWindowTitle("Schreibfehler");
        warningBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        warningBox.setIcon(QMessageBox::Warning);

        if(warningBox.exec() == QMessageBox::Retry)
        {


        }
        else
        {
            return;
        }
    }

    // Pens
    QPen penDefault = m_painter->pen();

    QPen penLine;
    penLine.setColor(Qt::black);
    penLine.setWidth(12);

    QPen penLineThin;
    penLineThin.setColor(Qt::black);
    penLineThin.setWidth(8);

    // Create company Logo
    QImage logo(":/Images/logo/tpt_logo.png");
    QImage scaledLogo = logo.scaled(static_cast<int>(logo.width() * 1.5), static_cast<int>(logo.height() * 1.5), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Define coordinates
    int w = m_pdfPrinter->pageRect().width();
    int h = m_pdfPrinter->pageRect().height();
    int x = 0;
    int y = 0;
    int lMargin = static_cast<int>(w * 0.05);
    int rMargin = static_cast<int>(w * 0.04);
    int tMargin = 0;
    int bMargin = 0;
    int spaceData = 100;
    int tabWidth = 170;

    int x_posLeft = x + lMargin;
    int x_posRight = w - rMargin;
    int x_posLogo = (w - scaledLogo.width() - rMargin);
    int x_posSender = x_posLogo + (scaledLogo.width() / 2) / 3;
    int y_posTop = tMargin;
    int y_posLine = y_posTop + scaledLogo.height() + 150;
    int y_posSender = y_posLine + 300;
    int y_posSenderSmall = y_posLine + 300;
    int y_posReceiver = y_posSenderSmall + 300;
    int y_posSubject = y_posReceiver + (w / 5) + 500;
    int pageContent = x_posRight - x_posLeft;

    // Set Font Metrics
    QFontMetrics metricFont(font);
    QFontMetrics metricSmallFont(senderSmallFont);
    QFontMetrics metricSubjectFont(subjectFont);
    QFontMetrics metricPosHeaderFont(posHeaderFont);
    QFontMetrics metricAccountFont(accountFont);

    m_painter->setFont(senderSmallFont);

    // Draw Company Logo
    QRect rectLogo(x_posLogo, y_posTop, scaledLogo.width(), scaledLogo.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectLogo);
    #endif
    m_painter->drawImage(rectLogo, scaledLogo);

    // Separation Line
    m_painter->setPen(penLineThin);
    m_painter->drawLine(x_posLeft, y_posLine, x_posRight, y_posLine);
    m_painter->setPen(penDefault);

    // Small Sender
    QRect rectSenderSmall(x_posLeft, y_posSenderSmall, metricSmallFont.width(getSettings(Anschrift)), metricSmallFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSenderSmall);
    #endif
    m_painter->drawText(rectSenderSmall, getSettings(Anschrift));

    m_painter->setFont(subjectFont);

    // Subject Line
    QRect rectSubject(x_posLeft, y_posSubject, w - rMargin - lMargin, metricSubjectFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSubject);
    #endif
    m_painter->drawText(rectSubject, Qt::AlignTop | Qt::AlignLeft, ui->leRgSubject->text());

    m_painter->setFont(font);

    // Sender
    QRect rectSender(x_posSender, y_posSender, (x_posRight - x_posSender), metricFont.height() * 10);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSender);
    #endif
    m_painter->drawText(rectSender, Qt::AlignTop | Qt::AlignLeft, getSettings(Kontakt));

    // Receiver
    QRect rectReceiver(x_posLeft, y_posReceiver, w / 3, metricFont.height() * 8);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectReceiver);
    #endif
    m_painter->drawText(rectReceiver, Qt::AlignTop | Qt::AlignLeft, receiver);

    y = y_posSubject;
    y += 500;

    // Invoice & Delivery No.
    QRect rectRgLfNrLabel(x_posLeft, y, metricFont.width(deliveryNrLabel), metricFont.height() * 2);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfNrLabel);
    #endif
    m_painter->drawText(rectRgLfNrLabel, Qt::AlignLeft | Qt::AlignTop, invoiceNrLabel);

    if( !ui->leLiefNr->text().isEmpty())
    {
        m_painter->drawText(rectRgLfNrLabel, Qt::AlignLeft | Qt::AlignBottom, deliveryNrLabel);
    }

    QRect rectRgLfNrData(x_posLeft + metricFont.width(deliveryNrLabel) + spaceData, y, 1400, metricFont.height() * 2);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfNrData);
    #endif
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignTop, ui->leRgNr->text());
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignBottom, ui->leLiefNr->text());

    // Invoice & Delivery Dates & Customer Nr.
    QRect rectRgLfDateLabel(x_posSender, y, metricFont.width(invoiceDateLabel), metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfDateLabel);
    #endif
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignTop, invoiceDateLabel);
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignVCenter, deliveryDateLabel);
    m_painter->drawText(rectRgLfDateLabel, Qt::AlignLeft | Qt::AlignBottom, customerNrLabel);

    QRect rectRgLfDateData(x_posSender + metricFont.width(invoiceDateLabel) + spaceData, y, (x_posRight - x_posSender) - metricFont.width(invoiceDateLabel) - 100, metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfDateData);
    #endif
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignTop, m_settings.getDateStr());
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignVCenter, ui->deLiefDate->text());
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignBottom, ui->cbRgCustomer->currentText().split(" ").value(0));

    y += 1100;

    // Free Text
    QRect rectFreeText(x_posLeft, y, pageContent, 500);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectFreeText);
    #endif
    m_painter->drawText(rectFreeText, getSettings(FreeText));

    // Position List
    /* Header */
    x = x_posLeft;
    y += 700;

    m_painter->setPen(penLine);
    m_painter->drawLine(x, y, x_posRight, y);

    int y_posHeader = y + 150;
    int y_posSecLine = y_posHeader + metricPosHeaderFont.height() + 150;

    m_painter->drawLine(x, y_posSecLine, x_posRight, y_posSecLine);

    y = y_posSecLine;

    m_painter->setPen(penDefault);

    y += 200;

    /* Data */
    int maxLenPosNr = metricFont.width("0000");
    int maxLenArtNr = metricFont.width("00000000");
    int maxLenCount = metricFont.width("000000000");
    int maxLenUnit = metricFont.width("00000000");
    int maxLenPrice = metricFont.width("0000000000");
    int maxLenSumme = metricFont.width("00000000000");
    int dataLenght = maxLenPosNr + maxLenArtNr + maxLenCount + maxLenUnit + maxLenPrice + maxLenSumme + (6 * tabWidth);
    int maxLenDescription = pageContent - dataLenght;

    // Position Label
    int x_position = x;
    QRect rectPosLabel(x_position, y_posHeader, maxLenPosNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPosLabel);
    #endif
    m_painter->drawText(rectPosLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(Position_PosNr));

    x += rectPosLabel.width() + tabWidth;

    // ArtNr Label
    int x_artNr = x;
    QRect rectArtNrLabel(x_artNr, y_posHeader, maxLenArtNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectArtNrLabel);
    #endif
    m_painter->drawText(rectArtNrLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(Position_ArtNr));

    x += rectArtNrLabel.width() + tabWidth;

    // Beschreibung Label
    int x_description = x;
    QRect rectDescriptionLabel(x_description, y_posHeader, maxLenDescription, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectDescriptionLabel);
    #endif
    m_painter->drawText(rectDescriptionLabel, Qt::AlignLeft, m_dbManager->getArticleFields().value(Article_Beschreibung));

    x += rectDescriptionLabel.width() + tabWidth;

    // Anzahl Label
    int x_count = x;
    QRect rectCountLabel(x_count, y_posHeader, maxLenCount, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectCountLabel);
    #endif
    m_painter->drawText(rectCountLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(Position_Menge));

    int x_totalLabel = x;
    x += rectCountLabel.width() + tabWidth;

    // Einheit Label
    int x_unit = x;
    QRect rectUnitLabel(x_unit, y_posHeader, maxLenUnit, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectUnitLabel);
    #endif
    m_painter->drawText(rectUnitLabel, Qt::AlignHCenter, m_dbManager->getArticleFields().value(Article_Einheit));

    x += rectUnitLabel.width() + tabWidth;

    // Preis Label
    int x_price = x;
    QRect rectPriceLabel(x_price, y_posHeader, maxLenPrice, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPriceLabel);
    #endif
    m_painter->drawText(rectPriceLabel, Qt::AlignRight, m_dbManager->getArticleFields().value(Article_Preis));

    x += rectPriceLabel.width() + tabWidth;

    // Summe Label
    int x_summe = x;
    QRect rectSummeLabel(x_summe, y_posHeader, maxLenSumme, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSummeLabel);
    #endif
    m_painter->drawText(rectSummeLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(Position_Gesamtpreis));

    y = y_posSecLine + 200;

    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        QString posNrStr = ui->twRgArticles->item(i, PosNr)->text();
        QString artNrStr = ui->twRgArticles->item(i, ArtNrPos)->text();
        QString labelStr = ui->twRgArticles->item(i, BeschreibungPos)->text();
        QString countStr = QString::number(ui->twRgArticles->item(i, AnzahlPos)->text().toDouble(), 'f', 2).replace(".", ",");
        QString unitStr = ui->twRgArticles->item(i, EinheitPos)->text();
        QString priceStr = ui->twRgArticles->item(i, EinzelPreisPos)->text();
        QString summeStr = ui->twRgArticles->item(i, SummePos)->text();

        int rows = qRound((double(metricFont.width(labelStr)) / double(maxLenDescription)) + 0.49);

        // Position
        QRect rectPosNr(x_position, y, maxLenPosNr, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectPosNr);
        #endif
        m_painter->drawText(rectPosNr, Qt::AlignLeft | Qt::TextWordWrap, posNrStr);

        // ArtNr
        QRect rectArtNr(x_artNr, y, maxLenArtNr, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectArtNr);
        #endif
        m_painter->drawText(rectArtNr, Qt::AlignLeft | Qt::TextWordWrap, artNrStr);

        // Beschreibung
        QRect rectDescription(x_description, y, maxLenDescription, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectDescription);
        #endif
        m_painter->drawText(rectDescription, Qt::AlignLeft | Qt::TextWordWrap, labelStr);

        // Anzahl
        QRect rectCount(x_count, y, maxLenCount, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectCount);
        #endif
        m_painter->drawText(rectCount, Qt::AlignRight | Qt::TextWordWrap, countStr);

        // Einheit
        QRect rectUnit(x_unit, y, maxLenUnit, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectUnit);
        #endif
        m_painter->drawText(rectUnit, Qt::AlignHCenter | Qt::TextWordWrap, unitStr);

        // Preis
        QRect rectPrice(x_price, y, maxLenPrice, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectPrice);
        #endif
        m_painter->drawText(rectPrice, Qt::AlignRight | Qt::TextWordWrap, priceStr);

        // Summe
        QRect rectSumme(x_summe, y, maxLenSumme, metricFont.height() * rows);
        m_painter->setFont(font);
        #ifdef BORDER_ACTIVE
            m_painter->drawRect(rectSumme);
        #endif
        m_painter->drawText(rectSumme, Qt::AlignRight | Qt::TextWordWrap, summeStr);

        y += metricFont.height() * rows + 200;
    }

    y += 100;

    // Trennlinie
    m_painter->setPen(penLine);
    m_painter->drawLine(x_totalLabel, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    // Gesamtsumme
    QString totalStr = ui->leRgSumme->text();
    QRect rectTotal(x_totalLabel, y, x_posRight - x_totalLabel, metricPosHeaderFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectTotal);
    #endif
    m_painter->setFont(posHeaderFont);
    m_painter->drawText(rectTotal, Qt::AlignLeft, "Rechungsbetrag: ");
    m_painter->drawText(rectTotal, Qt::AlignRight, totalStr);
    m_painter->setFont(font);

    // Conclusion part
    y = h - bMargin - (metricFont.height() * 3) - (metricAccountFont.height()) - 250;

    /* USt & Thx */
    QRect rectConclusion(x_posLeft, y, pageContent, metricFont.height() * 3);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectConclusion);
    #endif
    m_painter->setFont(ustFont);
    m_painter->drawText(rectConclusion, Qt::AlignTop | Qt::AlignLeft, getSettings(USt));
    m_painter->setFont(font);
    m_painter->drawText(rectConclusion, Qt::AlignBottom | Qt::AlignLeft, getSettings(Thx));

    y += metricFont.height() + 600;

    m_painter->setPen(penLineThin);
    m_painter->drawLine(x_posLeft, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    /* Steuernummer */
    QRect rectSteuerNr(x_posLeft, y, pageContent, metricAccountFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSteuerNr);
    #endif
    m_painter->setFont(accountFont);
    m_painter->drawText(rectSteuerNr, Qt::AlignBottom | Qt::AlignHCenter, getSettings(Steuernummer));

    y += metricFont.height() + 10;

    /* Kontoinformationen */
    QRect rectAccountInfo(x_posLeft, y, pageContent, metricAccountFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectAccountInfo);
    #endif
    m_painter->setFont(accountFont);
    m_painter->drawText(rectAccountInfo, Qt::AlignBottom | Qt::AlignHCenter, getSettings(Konto));

    m_painter->end();

    QMessageBox msgBox;
    //QPushButton *showButton = msgBox.addButton(tr("PDF "), QMessageBox::ActionRole);
    msgBox.setText("Die Rechnung wurde erstellt!");
    msgBox.setWindowTitle("Erstellung PDF");
    msgBox.setButtonText(QMessageBox::Close, "Schließen");
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton(QMessageBox::Close);

    msgBox.exec();

//    if (msgBox.clickedButton() == showButton)
//    {
//        QPdfDocument *document = new QPdfDocument;
//        document->load("/invoice/Rechnung_1107_20180831.pdf");

//        QPdfView *view = new QPdfView;
//        view->setDocument(document);
//        view->show();

//        QPrinter printer;
//        QPrintDialog printDialog(&printer, this);
//        if (printDialog.exec() == QDialog::Accepted)
//        {
//            // print ...
//        }
//    }

    delete m_pdfPrinter;
    delete m_painter;
}

QString MainWindow::getSettings(SettingsColumns typ)
{
    QString data;
    m_dbManager->readSetting(m_dbManager->getSettingsFields().value(typ), data);
    return data;
}

void MainWindow::on_btnSetCancel_clicked()
{
    ui->btnSetSave->setEnabled(false);
    ui->btnSetCancel->setEnabled(false);

    QString typ = ui->lwSetPdfPhrases->currentItem()->text();
    QString data;
    m_dbManager->readSetting(typ, data);
    ui->teSetContent->setText(data);
}

void MainWindow::on_btnSetSave_clicked()
{
    ui->btnSetSave->setEnabled(false);
    ui->btnSetCancel->setEnabled(false);

    QString typ = ui->lwSetPdfPhrases->currentItem()->text();
    m_dbManager->editSetting(typ ,ui->teSetContent->toPlainText());
}

void MainWindow::on_lwSetPdfPhrases_itemClicked(QListWidgetItem *item)
{
    ui->btnSetSave->setEnabled(false);
    ui->btnSetCancel->setEnabled(false);

    QString data;
    m_dbManager->readSetting(item->text(), data);
    ui->teSetContent->setText(data);
}

void MainWindow::on_teSetContent_selectionChanged()
{
    ui->btnSetSave->setEnabled(true);
    ui->btnSetCancel->setEnabled(true);
}

void MainWindow::on_deRgDate_dateChanged(const QDate &date)
{
    m_settings.setDate(date);
}

void MainWindow::on_tabWidgetInvoice_currentChanged(int index)
{
    switch(index)
    {
        case CreateInvoiceTab:
            break;

        case InvoicesTab:
            // Setup invoice list
            setupInvoiceList();

            // Read invoices
            printAllInvoices();

            ui->btnRgDetails->setEnabled(false);
            ui->btnRgDelete->setEnabled(false);

            if( ui->twRgList->rowCount() > 0 )
                ui->btnRgDeleteAll->setEnabled(true);
            else
                ui->btnRgDeleteAll->setEnabled(false);

            break;
    }
}

void MainWindow::on_twRgList_itemClicked(QTableWidgetItem *item)
{
    if (item != nullptr)
    {
        ui->btnRgDetails->setEnabled(true);
        ui->btnRgDelete->setEnabled(true);
    }
    else
    {
        ui->btnRgDetails->setEnabled(false);
        ui->btnRgDelete->setEnabled(false);
    }
}

void MainWindow::on_btnRgDelete_clicked()
{
    QString rechnungsNr = ui->twRgList->item(ui->twRgList->currentRow(), Invoice_RgNr)->text();
    QString dateUnformated = ui->twRgList->item(ui->twRgList->currentRow(), Invoice_RgDate)->text();
    QStringList dateList = dateUnformated.split(".");
    QString date = dateList[dateList.size()-1] + dateList[dateList.size()-2] + dateList[dateList.size()-3];
    QString outPath = PATH_PDF;
    QString invoiceName = outPath + "Rechnung_" + rechnungsNr + "_" + date + ".pdf";

    QMessageBox msg;
    msg.setWindowIcon(QPixmap("logo.png"));
    msg.setText("Möchten Sie die Rechnung mit der Nr. " + rechnungsNr + " wirklich löschen?");
    msg.setWindowTitle("Warnung");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setButtonText(QMessageBox::Yes, "Ja");
    msg.setButtonText(QMessageBox::No, "Nein");
    msg.setDefaultButton(QMessageBox::No);
    msg.setIcon(QMessageBox::Question);

    if(msg.exec() == QMessageBox::Yes)
    {
        QTableWidgetItem* item = ui->twRgList->selectedItems().value(0);
        QString rgnr = ui->twRgList->item(item->row(), 0)->text();

        m_dbManager->removeDbEntry(RECHNUNG, rgnr);
        m_dbManager->removeDbEntries(POSITION, m_dbManager->getInvoiceFields()[Invoice_RgNr], rgnr);

        // delete invoice pdf
        QFile invoice(invoiceName);
        invoice.remove();

        printAllInvoices();

        ui->btnRgDelete->setEnabled(false);
        ui->btnRgDetails->setEnabled(false);
    }
}

void MainWindow::on_twRgList_itemDoubleClicked(QTableWidgetItem */*item*/)
{
    on_btnRgDetails_clicked();
}

void MainWindow::on_twCustomers_itemDoubleClicked(QTableWidgetItem */*item*/)
{
    on_btnEditCustomer_clicked();
}

void MainWindow::on_twArticles_itemDoubleClicked(QTableWidgetItem *item)
{
    articleEdit(item);
}

void MainWindow::on_btnArtEdit_clicked()
{
    if (ui->twArticles->selectedItems().size() > 0 )
    {
        articleEdit(ui->twArticles->currentItem());
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Es wurde kein Artikel ausgewählt!", QMessageBox::Ok);
    }
}

void MainWindow::on_twArticles_itemClicked(QTableWidgetItem */*item*/)
{
    ui->btnArtEdit->setEnabled(true);
    ui->btnArtDelete->setEnabled(true);
}

void MainWindow::articleAdd()
{
    int artNr = m_dbManager->readLastID(ARTIKEL) + 1;
    Articles article;
    article.setArtNr(artNr);

    WindowArticle wa(this, &article, m_dbManager);
    wa.setWindowTitle("Neuer Artikel");

    if(wa.exec() == QDialog::Accepted)
    {
        m_dbManager->addArticle(wa.getArticle());
        printAllArticles();
    }
}

void MainWindow::articleEdit(QTableWidgetItem* item)
{
    int selectedRow = item->row();

    // read article from database
    QString artNr = ui->twArticles->item(item->row(), Article_ArtNr)->text();
    Articles article;
    if (!m_dbManager->readArticle(artNr, article))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customer!";
       return;
    }

    WindowArticle wa(this, &article, m_dbManager);
    wa.setWindowTitle("Artikel bearbeiten");

    if(wa.exec() == QDialog::Accepted)
    {
        printAllArticles();
        ui->twArticles->selectRow(selectedRow);
    }

    ui->twArticles->setFocus();
}

void MainWindow::articleDelete()
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
//            QMessageBox::information(this, "Info", "Der ausgewählte Artikel wurde erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Es wurde kein Artikel zum löschen ausgewählt!", QMessageBox::Ok);
    }
}

void MainWindow::on_cbRgArtikel_activated(const QString &/*arg1*/)
{
    ui->sbRgCount->setFocus();
    ui->sbRgCount->selectAll();
    ui->btnRgClear->setEnabled(true);
}

void MainWindow::on_leRgSinglePrice_returnPressed()
{
    this->on_btnRgAddArticle_clicked();
    ui->leRgArtNr->setFocus();
}

void MainWindow::on_cbLiefDate_stateChanged()
{
    if(ui->cbLiefDate->isChecked())
    {
        ui->deLiefDate->setEnabled( false );
    }
    else
    {
        ui->deLiefDate->setEnabled( true );
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionDbSave_triggered()
{
    QString dbDir = QFileDialog::getExistingDirectory(this, tr("Verzeichnis auswählen"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dbDir.isEmpty())
    {
        QString dbPath = PATH_DATABASE;
        QString newPath = dbDir + "/mainDatabase.db";

        if (QFile::exists(newPath))
        {
            QFile::remove(newPath);
        }

        bool copyState = QFile::copy(PATH_DATABASE, newPath);

        if(copyState)
        {
            QMessageBox::information(this, "Datenbank Sicherung", "Die Datenbank wurde erfolgreich gesichert!", QMessageBox::Ok);

        }
        else
        {
            QMessageBox::warning(this, "Datenbank Sicherung", "Fehler beim sichern der Datenbank!", QMessageBox::Ok);
        }
    }
}

void MainWindow::on_actionDbRestore_triggered()
{
    QStringList filters;
    filters << "Datenbankdateien (*.db)"
            << "Alle Dateien (*)";

    QFileDialog dialog(this);
    dialog.setNameFilters(filters);
    dialog.setWindowTitle("Datenbankdatei auswählen");

    if(dialog.exec())
    {
        QString newDbFile = dialog.selectedFiles().first();

        if (QFile::exists(PATH_DATABASE))
        {
            m_dbManager->closeDatabase();
            QFile::remove(PATH_DATABASE);
        }

        bool copyState = QFile::copy(newDbFile, PATH_DATABASE);

        if(copyState)
        {
            if(m_dbManager->openDatabase(PATH_DATABASE))
            {
                selectMainTabIndex(CustomersTab);
                QMessageBox::information(this, "Datenbank Wiederherstellung", "Die Datenbank wurde erfolgreich wiederhergestellt!", QMessageBox::Ok);

                ui->statusBar->showMessage(DB_CONNECTED_MESSAGE);
            }
            else
            {
                ui->statusBar->showMessage(DB_NOTCONNECTED_MESSAGE);
            }
        }
        else
        {
            QMessageBox::warning(this, "Datenbank Wiederherstellung", "Fehler beim wiederherstellen der Datenbank!", QMessageBox::Ok);
        }
    }
}

void MainWindow::on_leRgSinglePrice_textChanged(const QString &arg1)
{
    QString valueStr = arg1;
    valueStr = valueStr.replace(".","").replace(",", ".");
    QString currencyString = QLocale().toCurrencyString(valueStr.toDouble() * ui->sbRgCount->value());
    ui->leRgTotalPrice->setText( currencyString.split(" ").first());
}

void MainWindow::on_actionUeber_triggered()
{
    QMessageBox::information(this, "Über RechnungsApp", "Author: Marcel Geprägs \n"
                                                        "Version: 1.0.0.1", QMessageBox::Ok);
}

void MainWindow::on_btnRgDetails_clicked()
{
    int selectedRow = ui->twRgList->currentRow();
    QString rgNr = ui->twRgList->item(selectedRow, 0)->text();

    QString dateUnformated = ui->twRgList->item(ui->twRgList->currentRow(), Invoice_RgDate)->text();
    QStringList dateList = dateUnformated.split(".");
    QString date = dateList[dateList.size()-1] + dateList[dateList.size()-2] + dateList[dateList.size()-3];
    QString outPath = PATH_PDF;
    QString invoiceName = outPath + "Rechnung_" + rgNr + "_" + date + ".pdf";

    WindowPositions wp(this, rgNr, m_dbManager);
    wp.setWindowTitle("Detailansicht der Rechnung " + rgNr);

    if(wp.exec() == QDialog::Accepted)
    {
        if(wp.deleteInvoice())
        {
            m_dbManager->removeDbEntry(RECHNUNG, rgNr);
            printAllInvoices();

            // delete invoice pdf
            QFile invoice(invoiceName);
            invoice.remove();

            wp.setDeleteInvoice(false);
            return;
        }

        // delete positions in database
        if(wp.positionDeleted() & !wp.deletedPos().isEmpty())
        {
            m_dbManager->deletePosition(rgNr, wp.deletedPos());
            wp.setPositionDeleted(false);
            wp.clearDeletedPos();

            for(uint i = 0; i < wp.positions().size(); i++)
            {
                m_dbManager->editPosNr(wp.positions()[i]);
            }
        }

        // edit positions in database
        for(uint i = 0; i < wp.positions().size(); i++)
        {
            m_dbManager->editPosition(wp.positions()[i]);
        }

        // edit invoice amount in database
        m_dbManager->editInvoiceSumme(rgNr, wp.summe());

        // read all invoices from database and print out
        printAllInvoices();

        // edit invoice pdf
        Invoices invoice;
        m_dbManager->readInvoice(invoice, rgNr);

        std::vector<Positions> positions;
        m_dbManager->readPositions(positions, rgNr);

        QStringList dateList = invoice.rgdate().split(".");
        QString date = dateList[dateList.size()-1] + dateList[dateList.size()-2] + dateList[dateList.size()-3];

        createInvoice(rgNr, QString::number(invoice.kdnr()), date, "Rechnung", positions, QLocale().toCurrencyString(invoice.amount()), EditInvoice);
    }

    ui->twRgList->selectRow(selectedRow);
    ui->twRgList->setFocus();
}

void MainWindow::on_btnRgDeleteAll_clicked()
{
    if (ui->twRgList->rowCount() > 0)
    {
        QMessageBox msg;
        msg.setText("Möchten Sie wirklich ALLE Rechnungen löschen?");
        msg.setWindowTitle("Warnung");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(msg.exec() == QMessageBox::Yes)
        {
            m_dbManager->removeDBList(RECHNUNG);
            m_dbManager->removeDBList(POSITION);
            printAllInvoices();

            ui->btnRgDeleteAll->setEnabled(false);
//            QMessageBox::information(this, "Info", "Alle Artikel wurden erfolgreich gelöscht!", QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "Warnung", "Keine Artikel zum löschen verfügbar!", QMessageBox::Ok);
    }
}

void MainWindow::setupInvoiceList()
{
    if( ui->twRgList->columnCount() <= 0 )
    {
        ui->twRgList->setColumnCount(InvoiceColumsCount);
        for ( int i = 0; i < m_dbManager->getInvoiceFields().size(); i++)
        {
            ui->twRgList->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getInvoiceFields()[i]));
        }
        QFont fontInvoices("MS Shell Dlg 2", 8, QFont::Bold);
        ui->twRgList->horizontalHeader()->setFont(fontInvoices);
        ui->twRgList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->twRgList->horizontalHeader()->setSectionResizeMode(Article_Beschreibung, QHeaderView::Stretch);
    }
}

void MainWindow::setupArticleList()
{
    if(ui->twArticles->columnCount() <= 0)
    {
        ui->twArticles->setColumnCount(ArticleColumnsCount);

        for ( int i = 0; i < m_dbManager->getArticleFields().size(); i++)
        {
            ui->twArticles->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getArticleFields()[i]));
        }
        QFont fontArticles("MS Shell Dlg 2", 8, QFont::Bold);
        ui->twArticles->horizontalHeader()->setFont(fontArticles);
        ui->twArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->twArticles->horizontalHeader()->setSectionResizeMode(Article_Beschreibung, QHeaderView::Stretch);
    }
}

void MainWindow::setupArticlePositionList()
{
    if(ui->twRgArticles->columnCount() <= 0)
    {
        ui->twRgArticles->setColumnCount(ArtPosColumnsCount);

        ui->twRgArticles->setHorizontalHeaderItem(PosNr, new QTableWidgetItem("Pos."));
        ui->twRgArticles->setHorizontalHeaderItem(ArtNrPos, new QTableWidgetItem("Art-Nr."));
        ui->twRgArticles->setHorizontalHeaderItem(BeschreibungPos, new QTableWidgetItem("Beschreibung"));
        ui->twRgArticles->setHorizontalHeaderItem(AnzahlPos, new QTableWidgetItem("Menge"));
        ui->twRgArticles->setHorizontalHeaderItem(EinheitPos, new QTableWidgetItem("Einheit"));
        ui->twRgArticles->setHorizontalHeaderItem(EinzelPreisPos, new QTableWidgetItem("E-Preis"));
        ui->twRgArticles->setHorizontalHeaderItem(SummePos, new QTableWidgetItem("Gesamt"));

        QFont fontArticlesPos("MS Shell Dlg 2", 8, QFont::Bold);
        ui->twRgArticles->horizontalHeader()->setFont(fontArticlesPos);
        ui->twRgArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        ui->twRgArticles->horizontalHeader()->setSectionResizeMode(BeschreibungPos, QHeaderView::Stretch);
    }
}

void MainWindow::setupCustomerList()
{
    if(ui->twCustomers->columnCount() <= 0)
    {
        ui->twCustomers->setColumnCount(m_dbManager->getCustomerFields().size() - 1);
        for ( int i = 0; i < m_dbManager->getCustomerFields().size(); i++)
        {
            ui->twCustomers->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getCustomerFields()[i]));
        }
        QFont fontCustomer("MS Shell Dlg 2", 8, QFont::Bold);
        ui->twCustomers->horizontalHeader()->setFont(fontCustomer);
        ui->twCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    }
}

void MainWindow::on_twRgArticles_itemDoubleClicked(QTableWidgetItem *item)
{
    int curRow = item->row();
    ui->leRgArtNr->setText(ui->twRgArticles->item(curRow, ArtNrPos)->text());
    ui->leRgName->setText(ui->twRgArticles->item(curRow, BeschreibungPos)->text());
    ui->sbRgCount->setValue(ui->twRgArticles->item(curRow, AnzahlPos)->text().toInt());
    ui->leRgUnit->setText(ui->twRgArticles->item(curRow, EinheitPos)->text());
    ui->leRgSinglePrice->setText(ui->twRgArticles->item(curRow, EinzelPreisPos)->text().split(" ").value(0));
    ui->leRgTotalPrice->setText(ui->twRgArticles->item(curRow, SummePos)->text().split(" ").value(0));
}

void MainWindow::on_leRgArtNr_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->leRgName->text().isEmpty())
    {
        ui->btnRgAddArticle->setEnabled(true);
        ui->btnRgClear->setEnabled(true);
    }
    else
    {
        ui->btnRgAddArticle->setEnabled(false);
        ui->btnRgClear->setEnabled(false);
    }
}

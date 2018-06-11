
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include <QStandardItemModel>
#include <QStandardItem>

#include <QPrintDialog>
#include <QPrintPreviewDialog>

//#define BORDER_ACTIVE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pdfPrinter(new QPrinter(QPrinter::HighResolution)),
    m_painter(new QPainter()),
    m_posNr(0)
{
    ui->setupUi(this);

    m_pdfPrinter->setOutputFormat(QPrinter::PdfFormat);
    m_pdfPrinter->setOutputFileName(PATH_PDF);
    m_pdfPrinter->setPaperSize(QPrinter::A4);
    m_pdfPrinter->setPageMargins(QMarginsF(30, 30, 30, 30));

    m_dbManager = new DBManager(PATH_DATABASE);

    // Read settings from DB
    //m_dbManager->readSettings(m_settings);

    // Setup customer list
    ui->twCustomers->setColumnCount(m_dbManager->getCustomerFields().size() - 1);
    for ( int i = 0; i < m_dbManager->getCustomerFields().size(); i++)
    {
        ui->twCustomers->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getCustomerFields()[i]));
    }
    QFont fontCustomer("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twCustomers->horizontalHeader()->setFont(fontCustomer);
    ui->twCustomers->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Setup article list
    ui->twArticles->setColumnCount(m_dbManager->getArticleFields().size());
    for ( int i = 0; i < m_dbManager->getArticleFields().size(); i++)
    {
        ui->twArticles->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getArticleFields()[i]));
    }
    QFont fontArticles("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twArticles->horizontalHeader()->setFont(fontArticles);
    ui->twArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twArticles->horizontalHeader()->setSectionResizeMode(Beschreibung, QHeaderView::Stretch);

    // Setup invoice list
    ui->twRgList->setColumnCount(m_dbManager->getInvoiceFields().size());
    for ( int i = 0; i < m_dbManager->getInvoiceFields().size(); i++)
    {
        ui->twRgList->setHorizontalHeaderItem(i, new QTableWidgetItem(m_dbManager->getInvoiceFields()[i]));
    }
    QFont fontInvoices("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twRgList->horizontalHeader()->setFont(fontInvoices);
    ui->twRgList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twRgList->horizontalHeader()->setSectionResizeMode(Beschreibung, QHeaderView::Stretch);

    // Setup article positions
    ui->twRgArticles->setColumnCount(ArtPosColumnsCount);

    ui->twRgArticles->setHorizontalHeaderItem(0, new QTableWidgetItem("Pos."));
    ui->twRgArticles->setHorizontalHeaderItem(1, new QTableWidgetItem("Art-Nr."));
    ui->twRgArticles->setHorizontalHeaderItem(2, new QTableWidgetItem("Beschreibung"));
    ui->twRgArticles->setHorizontalHeaderItem(3, new QTableWidgetItem("Menge"));
    ui->twRgArticles->setHorizontalHeaderItem(4, new QTableWidgetItem("Einheit"));
    ui->twRgArticles->setHorizontalHeaderItem(5, new QTableWidgetItem("E-Preis"));
    ui->twRgArticles->setHorizontalHeaderItem(6, new QTableWidgetItem("Gesamt"));

    QFont fontArticlesPos("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twRgArticles->horizontalHeader()->setFont(fontArticlesPos);
    ui->twRgArticles->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twRgArticles->horizontalHeader()->setSectionResizeMode(BeschreibungPos, QHeaderView::Stretch);

    // Fill pdf settings list
    m_dbManager->getSettingsFields();
    for (int i = 0; i < m_dbManager->getSettingsFields().size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 30));
        item->setText(m_dbManager->getSettingsFields().value(i));
        ui->lwSetPdfPhrases->addItem(item);

    }

    // Set CustomersTab as default tab
    ui->tabWidgetMain->setCurrentIndex(CustomersTab);
    printAllCustomers();

    ui->tabWidKunden->setCurrentIndex(OverviewTab);
}

MainWindow::~MainWindow()
{
    m_dbManager->closeDatabase();

    delete m_dbManager;
    delete ui;
    delete m_pdfPrinter;
    delete m_painter;
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

    // print all exist customers to list
    for (std::vector<Invoices>::iterator it = m_invoices.begin(); it != m_invoices.end(); ++it)
    {
       int row = ui->twRgList->rowCount();
       ui->twRgList->insertRow(row);
       ui->twRgList->setRowHeight(row, INVOICE_ROW_HEIGHT);
       ui->twRgList->setItem(row, Invoice_RgNr, new QTableWidgetItem(QString::number(it->rgnr())));
       ui->twRgList->setItem(row, Invoice_KdNr, new QTableWidgetItem(QString::number(it->kdnr())));
       ui->twRgList->setItem(row, Invoice_RgDate, new QTableWidgetItem(it->rgdate()));
       ui->twRgList->setItem(row, Invoice_Amount, new QTableWidgetItem(QString::number(it->amount(), 'f', 2) + " €"));
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
            int lastID = m_dbManager->readLastID(KUNDEN);
            ui->leKdNr->setText(QString::number(lastID + 1));

            QPixmap pixmap = QPixmap(":/Icons/icons/customer_add.png");
            ui->lb_logoAddEdit->setPixmap(pixmap);
        }
    }
    else if(index == OverviewTab)
    {

    }
}

void MainWindow::on_tabWidgetMain_currentChanged(int index)
{
    switch(index)
    {
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

            // Fill invoice number
            int lastRgNr = m_dbManager->readLastID(RECHNUNGEN);
            ui->leRgNr->setText(QString::number(lastRgNr + 1));

            // Fill subject line
            ui->leRgSubject->setText("Rechnung");

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

            if (ui->tabWidgetInvoice->currentIndex() == 1)
            {
                printAllInvoices();
            }
        }
        break;

        case SettingsTab:
            fillSettingsEdit();
            break;

        default:
            break;
    }
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
    ui->twArticles->clearSelection();
}

void MainWindow::on_btnArtCancel_clicked()
{
    ui->twArticles->clearSelection();
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

    ui->twRgArticles->insertRow(row);
    ui->twRgArticles->setRowHeight(row, ARTICLEPOS_ROW_HEIGHT);
    ui->twRgArticles->setItem(row, PosNr, new QTableWidgetItem(QString::number(++m_posNr)));
    ui->twRgArticles->setItem(row, ArtNrPos, new QTableWidgetItem(artNr));
    ui->twRgArticles->setItem(row, BeschreibungPos, new QTableWidgetItem(name));
    ui->twRgArticles->setItem(row, AnzahlPos, new QTableWidgetItem(count));
    ui->twRgArticles->setItem(row, EinheitPos, new QTableWidgetItem(unit));
    ui->twRgArticles->setItem(row, EinzelPreisPos, new QTableWidgetItem(sPrice));
    //ui->twRgArticles->item(row, EinzelPreisPos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    ui->twRgArticles->setItem(row, SummePos, new QTableWidgetItem(tPrice));
    //ui->twRgArticles->item(row, SummePos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

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
        double sum = ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0).toDouble();
        summe += sum;
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
        ui->btnRgDeteleAllArticle->setEnabled(false);
        ui->btnRgCreate->setEnabled(false);
        m_posNr = 0;
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
        ui->btnRgClear->setEnabled(true);
        ui->sbRgCount->setValue(1);
    }
    else
    {
        ui->btnRgAddArticle->setEnabled(false);
        ui->btnRgClear->setEnabled(false);
        ui->sbRgCount->setValue(0);
    }
}

void MainWindow::on_twRgArticles_itemClicked(QTableWidgetItem *item)
{
    int curRow = item->row();
    ui->leRgArtNr->setText(ui->twRgArticles->item(curRow, ArtNrPos)->text());
    ui->leRgName->setText(ui->twRgArticles->item(curRow, BeschreibungPos)->text());
    ui->sbRgCount->setValue(ui->twRgArticles->item(curRow, AnzahlPos)->text().toDouble());
    ui->leRgUnit->setText(ui->twRgArticles->item(curRow, EinheitPos)->text());
    ui->sbRgSinglePrice->setValue(ui->twRgArticles->item(curRow, EinzelPreisPos)->text().split(" ").value(0).toDouble());
    ui->sbRgTotalPrice->setValue(ui->twRgArticles->item(curRow, SummePos)->text().split(" ").value(0).toDouble());
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
        ui->leRgName->setText(article.getName());
        ui->leRgUnit->setText(article.getUnit());
        ui->sbRgCount->setValue(1);
        ui->sbRgSinglePrice->setValue(article.getPrice());
        ui->sbRgTotalPrice->setValue(article.getPrice() * 1);

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
    this->on_btnRgAddArticle_clicked();
    ui->leRgArtNr->setFocus();
}

void MainWindow::on_twRgArticles_itemChanged(QTableWidgetItem *item)
{
    if (ui->twRgArticles->rowCount() > 0)
    {
        ui->btnRgDeteleAllArticle->setEnabled(true);
        ui->btnRgCreate->setEnabled(true);
    }
}

void MainWindow::on_btnRgCreate_clicked()
{
    // Add positions to DB
    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        int pos = ui->twRgArticles->item(i, PosNr)->text().toInt();
        int rgnr = ui->leRgNr->text().toInt();
        int artnr = ui->twRgArticles->item(i, ArtNrPos)->text().toInt();
        int menge = ui->twRgArticles->item(i, AnzahlPos)->text().toInt();
        double total = ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0).toDouble();

        Positions position(pos, rgnr, artnr, menge, total);

        m_dbManager->addPosition(position);
    }

    // Add invoice to DB
    int kdnr = ui->cbRgCustomer->currentText().split(" ").value(0).toInt();
    QString rgdate = ui->deRgDate->text();
    double amount = ui->sbRgSumme->value();
    int ust = ui->sbRgUst->value();
    int skonto = ui->sbRgSkonto->value();
    QString currency = ui->leRgCurrency->text();

    Invoices invoice(0, kdnr, rgdate, amount, ust, skonto, currency);

    m_dbManager->addInvoice(invoice);

    // Create the invoice to PDF
    createInvoice();

    // Clean article positions table
    int rowCount = ui->twRgArticles->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        ui->twRgArticles->removeRow(0);
    }

    updateTotalPrice();
    ui->btnRgDeteleAllArticle->setEnabled(false);
    ui->btnRgCreate->setEnabled(false);
    m_posNr = 0;
    QMessageBox::information(this, "Info", "Rechnung wurde erstellt!", QMessageBox::Ok);
}

void MainWindow::fillSettingsEdit()
{

}

void MainWindow::readSettingsEdit()
{

}

void MainWindow::createInvoice()
{
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
    QImage scaledLogo = logo.scaled(logo.width() * 1.5, logo.height() * 1.5, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Define coordinates
    int w = m_pdfPrinter->pageRect().width();
    int h = m_pdfPrinter->pageRect().height();
    int x = 0;
    int y = 0;
    int lMargin = w * 0.05;
    int rMargin = w * 0.04;
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
    m_painter->drawText(rectRgLfNrLabel, Qt::AlignLeft | Qt::AlignBottom, deliveryNrLabel);

    QRect rectRgLfNrData(x_posLeft + metricFont.width(deliveryNrLabel) + spaceData, y, 1400, metricFont.height() * 2);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectRgLfNrData);
    #endif
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignTop, ui->leRgNr->text());
    m_painter->drawText(rectRgLfNrData, Qt::AlignLeft | Qt::AlignBottom, "2345678");

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
    m_painter->drawText(rectRgLfDateData, Qt::AlignLeft | Qt::AlignVCenter, "01.01.2001");
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
    y = y_posHeader + metricPosHeaderFont.height() + 150;

    m_painter->drawLine(x, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 200;

    /* Data */
    QStringList slPosNr;
    QStringList slArtNr;
    QStringList slLabel;
    //QStringList slDescription;
    QStringList slCount;
    QStringList slUnit;
    QStringList slPrice;
    QStringList slSumme;

    int maxLenPosNr = metricFont.width("0000");
    int maxLenArtNr = metricFont.width("00000000");
    int maxLenCount = metricFont.width("000000000");
    int maxLenUnit = metricFont.width("00000000");
    int maxLenPrice = metricFont.width("0000000000");
    int maxLenSumme = metricFont.width("00000000000");
    int dataLenght = maxLenPosNr + maxLenArtNr + maxLenCount + maxLenUnit + maxLenPrice + maxLenSumme + (6 * tabWidth);
    int maxLenDescription = pageContent - dataLenght;

    for (int i = 0; i < ui->twRgArticles->rowCount(); i++)
    {
        slPosNr.append(ui->twRgArticles->item(i, PosNr)->text());
        slArtNr.append(ui->twRgArticles->item(i, ArtNrPos)->text());
        slLabel.append(ui->twRgArticles->item(i, BeschreibungPos)->text());
        //slDescription.append(ui->twRgArticles->item(i, BeschreibungPos)->text());
        slCount.append(QString::number(ui->twRgArticles->item(i, AnzahlPos)->text().toDouble(), 'f', 2).replace(".", ","));
        slUnit.append(ui->twRgArticles->item(i, EinheitPos)->text());
        slPrice.append(QLocale().toCurrencyString(ui->twRgArticles->item(i, EinzelPreisPos)->text().split(" ").value(0).toDouble()));
        slSumme.append(QLocale().toCurrencyString(ui->twRgArticles->item(i, SummePos)->text().split(" ").value(0).toDouble()));
    }

    QString posNrStr = slPosNr.join("\n\n");
    QString artNrStr = slArtNr.join("\n\n");
    QString labelStr = slLabel.join("\n\n");
    //QString descriptionStr = slDescription.join("\n\n");
    QString countStr = slCount.join("\n\n");
    QString unitStr = slUnit.join("\n\n");
    QString priceStr = slPrice.join("\n\n");
    QString summeStr = slSumme.join("\n\n");

    QRect rectPosLabel(x, y_posHeader, maxLenPosNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPosLabel);
    #endif
    m_painter->drawText(rectPosLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(PosNr));
    QRect rectPosNr(x, y, maxLenPosNr, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPosNr);
    #endif
    m_painter->drawText(rectPosNr, Qt::AlignLeft | Qt::TextWordWrap, posNrStr);

    x += rectPosNr.width() + tabWidth;

    QRect rectArtNrLabel(x, y_posHeader, maxLenArtNr, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectArtNrLabel);
    #endif
    m_painter->drawText(rectArtNrLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(ArtNrPos));
    QRect rectArtNr(x, y, maxLenArtNr, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectArtNr);
    #endif
    m_painter->drawText(rectArtNr, Qt::AlignLeft | Qt::TextWordWrap, artNrStr);

    x += rectArtNr.width() + tabWidth;

    QRect rectDescriptionLabel(x, y_posHeader, maxLenDescription, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectDescriptionLabel);
    #endif
    m_painter->drawText(rectDescriptionLabel, Qt::AlignLeft, m_dbManager->getPositionFields().value(BeschreibungPos));
    QRect rectDescription(x, y, maxLenDescription, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectDescription);
    #endif
    m_painter->drawText(rectDescription, Qt::AlignLeft | Qt::TextWordWrap, labelStr);

    x += rectDescription.width() + tabWidth;

    QRect rectCountLabel(x, y_posHeader, maxLenCount, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectCountLabel);
    #endif
    m_painter->drawText(rectCountLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(AnzahlPos));
    QRect rectCount(x, y, maxLenCount, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectCount);
    #endif
    m_painter->drawText(rectCount, Qt::AlignRight | Qt::TextWordWrap, countStr);

    int x_totalLabel = x;
    x += rectCount.width() + tabWidth;

    QRect rectUnitLabel(x, y_posHeader, maxLenUnit, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectUnitLabel);
    #endif
    m_painter->drawText(rectUnitLabel, Qt::AlignHCenter, m_dbManager->getPositionFields().value(EinheitPos));
    QRect rectUnit(x, y, maxLenUnit, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectUnit);
    #endif
    m_painter->drawText(rectUnit, Qt::AlignHCenter | Qt::TextWordWrap, unitStr);

    x += rectUnit.width() + tabWidth;

    QRect rectPriceLabel(x, y_posHeader, maxLenPrice, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPriceLabel);
    #endif
    m_painter->drawText(rectPriceLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(EinzelPreisPos));
    QRect rectPrice(x, y, maxLenPrice, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectPrice);
    #endif
    m_painter->drawText(rectPrice, Qt::AlignRight | Qt::TextWordWrap, priceStr);

    x += rectPrice.width() + tabWidth;

    QRect rectSummeLabel(x, y_posHeader, maxLenSumme, metricPosHeaderFont.height());
    m_painter->setFont(posHeaderFont);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSummeLabel);
    #endif
    m_painter->drawText(rectSummeLabel, Qt::AlignRight, m_dbManager->getPositionFields().value(SummePos));
    QRect rectSumme(x, y, maxLenSumme, metricFont.height() * (ui->twRgArticles->rowCount() * 2));
    m_painter->setFont(font);
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectSumme);
    #endif
    m_painter->drawText(rectSumme, Qt::AlignRight | Qt::TextWordWrap, summeStr);

    y += rectSumme.height() + 200;

    m_painter->setPen(penLine);
    m_painter->drawLine(x_totalLabel, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    // Summe
    QString totalStr = QLocale().toCurrencyString(ui->sbRgSumme->value());
    //QString totalStr = "€ " + QString::number(ui->sbRgSumme->value(), 'f', 2).replace(".", ",");
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

    y += (metricFont.height() * 3) + 400;

    m_painter->setPen(penLineThin);
    m_painter->drawLine(x_posLeft, y, x_posRight, y);
    m_painter->setPen(penDefault);

    y += 100;

    /* Account informations */
    QRect rectAccountInfo(x_posLeft, y, pageContent, metricAccountFont.height());
    #ifdef BORDER_ACTIVE
        m_painter->drawRect(rectAccountInfo);
    #endif
    m_painter->setFont(accountFont);
    m_painter->drawText(rectAccountInfo, Qt::AlignBottom | Qt::AlignHCenter, getSettings(Konto));

    m_painter->end();
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
            // Read invoices
            printAllInvoices();

            ui->btnRgDetails->setEnabled(false);
            ui->btnRgDelete->setEnabled(false);
            break;
    }
}

void MainWindow::on_btnRgDetails_clicked()
{
    QTableWidgetItem* item = ui->twRgList->selectedItems().value(0);
    QString rgnr = ui->twRgList->item(item->row(), 0)->text();

    m_positions.clear();

    // read invoice positions from database
    if (!m_dbManager->readPositions(m_positions, rgnr))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read invoice positions!";
       return;
    }

    WindowPositions wp(this, m_positions);
    wp.setWindowTitle("Detailansicht der Rechnung " + rgnr);
    if(wp.exec() == QDialog::Rejected)
        return;
}

void MainWindow::on_twRgList_itemClicked(QTableWidgetItem *item)
{
    if (item != NULL)
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
    QMessageBox msg;
    msg.setWindowIcon(QPixmap("logo.png"));
    msg.setText("Möchten Sie die Rechnung wirklich löschen?");
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

        m_dbManager->removeDbEntry(RECHNUNGEN, rgnr);
        m_dbManager->removeDbEntries(POSITIONEN, m_dbManager->getInvoiceFields()[Invoice_RgNr], rgnr);

        printAllInvoices();

        QMessageBox::information(this, "Info", "Die Rechnung wurde gelöscht!", QMessageBox::Ok);
    }
}

void MainWindow::on_twRgList_itemDoubleClicked(QTableWidgetItem *item)
{
    on_btnRgDetails_clicked();
}

void MainWindow::on_twCustomers_itemDoubleClicked(QTableWidgetItem *item)
{
    on_btnEditCustomer_clicked();
}

void MainWindow::on_actionProgramm_beenden_triggered()
{
    qApp->quit();
}

void MainWindow::on_twArticles_itemDoubleClicked(QTableWidgetItem *item)
{
    // read article from database
    QString artNr = ui->twArticles->item(item->row(), ArtNr)->text();
    Articles article;
    if (!m_dbManager->readArticle(artNr, article))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read customer!";
       return;
    }

    WindowArticle wa(this, &article, m_dbManager);
    wa.setWindowTitle("Artikel bearbeiten");

    if(wa.exec() == QDialog::Accepted)
        printAllArticles();
}

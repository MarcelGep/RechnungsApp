#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QImage>
#include <QPdfWriter>
#include <QListWidgetItem>
#include <QFileDialog>

#include <windowPositions.h>
#include <windowArticle.h>

#include "dbmanager.h"

#ifdef QT_DEBUG
    #define PATH_DATABASE "..\\RechnungsApp/database/mainDatabase.db"
    #define PATH_DATABASE_CLEAN "database/mainDatabase_clean.db"
    #define PATH_PDF "..\\RechnungsApp/invoice/"
#endif

#ifdef QT_NO_DEBUG
    #define PATH_DATABASE "database/mainDatabase.db"
    #define PATH_DATABASE_CLEAN "database/mainDatabase_clean.db"
    #define PATH_PDF "invoice/"
#endif

#define DEBUG_TAG_MAIN "MainWindow"

#define INVOICE_COLUMN_OFFSET    25
#define CUSTOMER_COLUMN_OFFSET   25
#define INVOICE_POSITIONS_OFFSET 25
#define INVOICE_ROW_HEIGHT       25
#define CUSTOMER_ROW_HEIGHT      25
#define ARTICLE_ROW_HEIGHT       25
#define ARTICLEPOS_ROW_HEIGHT    25

#define DEFAULT_FONT_SIZE       160
#define SUBJECT_FONT_SIZE       220
#define SENDER_SMALL_FONT_SIZE  115
#define ACCOUNT_FONT_SIZE       100

enum ArtPosColumns
{
    PosNr,
    ArtNrPos,
    BeschreibungPos,
    AnzahlPos,
    EinheitPos,
    EinzelPreisPos,
    SummePos,

    ArtPosColumnsCount
};

enum CustomerColumns
{
    KdNr,
    Firma,
    Name1,
    Name2,
    Strasse,
    Plz,
    Ort,
    Land,
    Telefon,
    Telefax,
    Email,
    Website,
    Rabatt,
    Kontostand,
    Info,

    CustomersColumnsCount
};

enum ArticleColumns
{
    Article_ArtNr,
    Article_Einheit,
    Article_Bezeichnung,
    Article_Beschreibung,
    Article_Preis,

    ArticleColumnsCount
};

enum SettingsColumns
{
    Kontakt,
    Anschrift,
    Konto,
    USt,
    Thx,
    FreeText,
};

enum CustomerTab
{
    OverviewTab,
    EditTab
};

enum MainTab
{
    CustomersTab,
    ArticlesTab,
    CalculationsTab,
    SettingsTab
};

enum InvoiceTabs
{
    CreateInvoiceTab,
    InvoicesTab
};

enum InvoiceColumns
{
    Invoice_RgNr,
    Invoice_KdNr,
    Invoice_RgDate,
    Invoice_Amount,
    Invoice_USt,
    Invoice_Skonto,
    Invoice_Currency,

    InvoiceColumsCount
};

enum PositionColumns
{
    Position_PosNr,
    Position_ArtNr,
    Position_RgNr,
    Position_Menge,
    Position_Gesamt,

    PositionColumns_Count
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void printAllCustomers();
    void printCustomer(Customers customer);
    void clearCustomerEdits() const;
    void printAllArticles();
    void createInvoice();

    void printAllInvoices();

private slots:
    void on_btnSaveCustomer_clicked();
    void on_btnDeleteCustomer_clicked();
    void on_btnNewCustomer_clicked();
    void on_btnEditCustomer_clicked();
    void on_tabWidKunden_tabBarClicked(int index);
    void on_btnCancelCustomer_clicked();
    void on_btnRgRechnung_clicked();
    void on_btnRgAngebot_clicked();
    void on_btnRgGutschrift_clicked();
    void on_btnArtNew_clicked();
    void on_btnArtDelete_clicked();
    void on_btnArtDelAll_clicked();
    void on_twCustomers_itemClicked(QTableWidgetItem *item);
    void on_btnRgClear_clicked();
    void on_cbRgArtikel_currentTextChanged(const QString &name);
    void on_sbRgCount_valueChanged(int value);
    void on_btnRgAddArticle_clicked();
    void on_btnRgDeteleAllArticle_clicked();
    void on_btnRgDeleteArticle_clicked();
    void on_twRgArticles_itemSelectionChanged();
    void on_cbRgArtikel_activated(int index);
    void on_leRgName_textChanged(const QString &text);
    void on_twRgArticles_itemClicked(QTableWidgetItem *item);
    void on_btnCustomerBill_clicked();
    void on_twCustomers_itemSelectionChanged();
    void on_leRgArtNr_returnPressed();
    void on_sbRgCount_editingFinished();
    void on_leRgUnit_returnPressed();
    void on_twRgArticles_itemChanged(QTableWidgetItem *item);
    void on_btnRgCreate_clicked();
    void on_btnSetCancel_clicked();
    void on_btnSetSave_clicked();
    void on_lwSetPdfPhrases_itemClicked(QListWidgetItem *item);
    void on_teSetContent_selectionChanged();
    void on_deRgDate_dateChanged(const QDate &date);
    void on_tabWidgetMain_currentChanged(int index);
    void on_tabWidgetInvoice_currentChanged(int index);
    void on_btnRgDetails_clicked();
    void on_twRgList_itemClicked(QTableWidgetItem *item);
    void on_btnRgDelete_clicked();

    void on_twRgList_itemDoubleClicked(QTableWidgetItem *item);

    void on_twCustomers_itemDoubleClicked(QTableWidgetItem *item);

    void on_twArticles_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnArtEdit_clicked();

    void on_twArticles_itemClicked(QTableWidgetItem *item);

    void on_cbRgArtikel_activated(const QString &arg1);

    void on_leRgSinglePrice_returnPressed();

    void on_cbLiefDate_stateChanged();

    void on_actionExit_triggered();

    void on_actionDbSave_triggered();

    void on_actionDbRestore_triggered();

    void on_leRgSinglePrice_textChanged(const QString &arg1);

    void on_actionUeber_triggered();

private:
    void setCustomerColumnsWidth() const;
    void clearCustomers();
    void clearArticles();
    void setArticleColumnsWidth() const;
    void clearBillEdits();
    void setArticlePosColumnsWidth() const;
    void updateTotalPrice();
    void fillSettingsEdit();

private:
    Ui::MainWindow *ui;
    DBManager* m_dbManager;
    int m_posNr;
    Settings m_settings;

    std::vector<Customers> m_customers;
    std::vector<Articles> m_articles;
    std::vector<Invoices> m_invoices;
    std::vector<Positions> m_positions;

    void readSettingsEdit();
    QString getSettings(SettingsColumns typ);
    void clearInvoices();
    void setInvoicesColumnsWidth() const;
    void clearPositions();
    void printPositions(QString rgnr);
    void setPositionsColumnsWidth() const;


    void articleEdit(QTableWidgetItem *item);
    void articleDelete();
    void articleAdd();
};

#endif // MAINWINDOW_H

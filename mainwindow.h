#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QImage>
#include <QPdfWriter>
#include <QListWidgetItem>

#include "dbmanager.h"

#define PATH_DATABASE "..\\RechnungsApp/database/mainDatabase.db"
#define PATH_PDF "..\\RechnungsApp/invoice/output_file.pdf"

#define DEBUG_TAG_MAIN "MainWindow"

#define CUSTOMER_COLUMN_OFFSET  25
#define CUSTOMER_ROW_HEIGHT     25
#define ARTICLE_ROW_HEIGHT      25
#define ARTICLEPOS_ROW_HEIGHT   25

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
    ArtNr,
    Einheit,
    Bezeichnung,
    Beschreibung,
    Preis,

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
    EvidenceTab,
    CustomersTab,
    ArticlesTab,
    CalculationsTab,
    SettingsTab
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void printAllCustomers();
    void printCustomer(Customers customer);
    void clearCustomerEdits() const;
    void printAllArticles();
    void clearArticleEdits();
    void createInvoice();

private slots:
    void on_btnSaveCustomer_clicked();
    void on_btnDeleteCustomer_clicked();
    void on_btnNewCustomer_clicked();
    void on_btnEditCustomer_clicked();
    void on_tabWidKunden_tabBarClicked(int index);
    void on_btnCancelCustomer_clicked();
    void on_tabMain_currentChanged(int index);
    void on_btnRgRechnung_clicked();
    void on_btnRgAngebot_clicked();
    void on_btnRgGutschrift_clicked();
    void on_btnArtSave_clicked();
    void on_twArticles_itemClicked(QTableWidgetItem *item);
    void on_btnArtNew_clicked();
    void on_btnArtCancel_clicked();
    void on_btnArtDelete_clicked();
    void on_btnArtDelAll_clicked();
    void on_twCustomers_itemClicked(QTableWidgetItem *item);
    void on_btnRgClear_clicked();
    void on_cbRgArtikel_currentTextChanged(const QString &name);
    void on_sbRgCount_valueChanged(int value);
    void on_btnRgAddArticle_clicked();
    void on_sbRgSinglePrice_valueChanged(double value);
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
    void on_teSetContent_textChanged();

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
    QPrinter* m_pdfPrinter;
    QPainter* m_painter;
    int m_posNr;
    Settings m_settings;

    std::vector<Customers> m_customers;
    std::vector<Articles> m_articles;

    void readSettingsEdit();
};

#endif // MAINWINDOW_H

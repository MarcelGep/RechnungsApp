#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include "dbmanager.h"

#define PATH_DATABASE "..\\RechnungsApp/database/mainDatabase.db"

#define DEBUG_TAG_MAIN "MainWindow"

#define CUSTOMER_COLUMN_OFFSET 25
#define CUSTOMER_ROW_HEIGHT 25
#define ARTICLE_ROW_HEIGHT 25

#define ARTIKEL "Artikel"
#define KUNDEN "Kunden"
#define RECHNUNG "Rechnungen"
#define POSITIONEN "Positionen"

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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void printAllCustomers();
    void printCustomer(Customers customer);
    void clearCustomerEdits() const;

    void printAllArticles();
    void clearArticleEdits();
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

private:
    void setCustomerColumnsWidth() const;

private:
    Ui::MainWindow *ui;
    DBManager* m_dbManager;

    QMap<int, QString> m_customerFields;
    QMap<int, QString> m_articleFields;
    QMap<int, QString> m_calculationFields;
    QMap<int, QString> m_positionFields;

    std::vector<Customers> m_customers;
    std::vector<Articles> m_articles;

    void clearCustomers();
    void clearArticles();
    void setArticleColumnsWidth() const;
};

#endif // MAINWINDOW_H

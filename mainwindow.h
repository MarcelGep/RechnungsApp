#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanager.h"

//#define PATH_DATABASE "F:/Develop/RechnungsApp/mainDatabase.db"
#define PATH_DATABASE "C:/Users/Marcel/Dropbox/Programmieren/RechnungsApp/database/mainDatabase.db"
#define CUSTOMER_COLUMN_OFFSET 15
#define CUSTOMER_ROW_HEIGHT 40

#define ARTIKEL "Artikel"
#define KUNDEN "Kunden"
#define RECHNUNG "Rechnungen"
#define POSITIONEN "Positionen"

enum Columns
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

    columnsCount
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

    void printAllCustomers() const;
    int getLastID(QString table);
    bool printCustomer();
    void clearCustomerEdits() const;

    void printAllAricles() const;
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
    void on_tvArtList_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    DBManager* m_dbManager;
};

#endif // MAINWINDOW_H

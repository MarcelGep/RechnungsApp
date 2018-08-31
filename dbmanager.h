#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSql>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "customers.h"
#include "articles.h"
#include "settings.h"
#include "invoices.h"
#include "positions.h"

#define DEBUG_TAG "DBManager"

#define ARTIKEL     "Artikel"
#define KUNDEN      "Kunden"
#define RECHNUNGEN  "Rechnungen"
#define POSITIONEN  "Positionen"
#define SETTINGS    "Settings"

class DBManager
{

public:
    DBManager(const QString &path);
    ~DBManager();

    bool isOpen() const;
    bool addCustomer(const Customers &customer);
    bool addBill(QString datum, double betrag);
    bool editCustomer(QString id, const Customers &customer);
    bool removeDbEntry(QString table, QString id);
    void removeBill(int billID);
    void closeDatabase();
    void openDatabase(QString path);
    bool dbEntryExist(QString table, QString id);
    bool readCustomers(std::vector<Customers> &customers) const;
    int readLastID(QString table) const;
    bool readCustomer(QString customerID, Customers &customer);
    QSqlQueryModel *readDbData(QString table);
    bool addArticle(const Articles &article);
    bool readArticles(std::vector<Articles> &articles) const;
    bool readArticle(QString articleID, Articles &article);
    bool editArticle(QString id, const Articles &article);
    QMap<int, QString> getCustomerFields() const;
    QMap<int, QString> getArticleFields() const;
    QMap<int, QString> getInvoiceFields() const;
    QMap<int, QString> getPositionFields() const;
    QMap<int, QString> getSettingsFields() const;
    bool readSettings(Settings);
    bool removeDBList(QString table);
    bool editSettings(Settings &settings);

    bool readSetting(QString typ, QString &data);
    bool editSetting(QString typ, QString data);
    bool readInvoices(std::vector<Invoices> &invoices);
    bool addPosition(const Positions &position);
    bool addInvoice(const Invoices &invoice);
    bool readPositions(std::vector<Positions> &positions, QString rgnr) const;
    bool removeDbEntries(QString table, QString ident, QString id);

private:
    QSqlDatabase m_db;
    QMap<int, QString> m_customerFields;
    QMap<int, QString> m_articleFields;
    QMap<int, QString> m_invoiceFields;
    QMap<int, QString> m_positionFields;
    QMap<int, QString> m_settingsFields;
    QString getDbIdent(QString table);
    QMap<int, QString> readFieldNames(QString table);
};

#endif // DBMANAGER_H

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSql>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "customers.h"
#include "articles.h"

#define DEBUG_TAG "DBManager"

class DBManager
{

public:
    DBManager(const QString &path);
    ~DBManager();

    bool isOpen() const;
    bool addCustomer(const Customers &customer);
    bool addBill(QString datum, double betrag);
    bool editCustomer(QString id, const Customers &customers);
    bool removeDbEntry(QString table, QString id);
    void removeBill(int billID);
//    bool removeAllArticles();
    void closeDatabase();
    bool dbEntryExist(QString table, QString id);
    bool readCustomers(std::vector<Customers> &customers) const;
    int readLastID(QString table) const;
    Customers readCustomer(QString customerID) const;

    QSqlQueryModel *readDbData(QString table);

    bool addArticle(const Articles &article);
    QMap<int, QString> readFieldNames(QString table);
private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H

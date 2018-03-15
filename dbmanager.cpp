#include "dbmanager.h"
#include <vector>
#include <iostream>
#include <map>
#include <stdexcept>

#include "mainwindow.h"

DBManager::DBManager(const QString &path)
{
    QFileInfo fileInfo(path);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
       qDebug() << DEBUG_TAG << ": Error - Connection to Database fail!";
    else
       qDebug() << DEBUG_TAG << ": Database Connection to " + fileInfo.absoluteFilePath() + " successfull!";
}

DBManager::~DBManager()
{
}

void DBManager::closeDatabase()
{
    QString connectionName = m_db.connectionName();

    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
    qDebug() << DEBUG_TAG << ": Connection to database closed!";
}

bool DBManager::dbEntryExist(QString table, QString id)
{
    if (isOpen())
    {
        QSqlQuery querySearch;
        querySearch.prepare("SELECT ID FROM '"+table+"'");
        querySearch.exec();

        while(querySearch.next())
        {
            QString desData = querySearch.value(0).toString();
            if(desData == id)
            {
                return true;
            }
        }
        return false;
    }
    else
    {
        qDebug() << "Database is not open!";
        return false;
    }
}

bool DBManager::isOpen() const
{
    return m_db.isOpen();
}

//bool DBManager::removeAllArticles()
//{
//    QSqlQuery removeQuery;
//    removeQuery.prepare("DELETE FROM articles");

//    if (removeQuery.exec())
//    {
//        qDebug() << DEBUG_TAG << ": All articles deleted!";
//        return true;
//    }
//    else
//    {
//        qDebug() << DEBUG_TAG << ": remove all articles failed: " << removeQuery.lastError();
//        return false;
//    }
//}
bool DBManager::addArticle(const Articles& article)
{
    bool success = false;
    QString col_names;

    std::vector<QString> entries = article.entries();

    for(std::vector<QString>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if( !col_names.isEmpty() )
        {
          col_names  += ", ";
        }

        col_names += *it;
    }

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO Artikel (" + col_names + ") VALUES (:einheit, "
                                                                    ":bezeichnung, "
                                                                    ":preis, "
                                                                    ":beschreibung)");
    queryAdd.bindValue(":einheit", article.unit());
    queryAdd.bindValue(":bezeichnung", article.name());
    queryAdd.bindValue(":preis", article.price());
    queryAdd.bindValue(":beschreibung", article.description());

    if(queryAdd.exec())
    {
        success = true;
        qDebug() << DEBUG_TAG << ": Article added to database!";

    }
    else
    {
        qDebug() << DEBUG_TAG << ": Add article to database failed - " << queryAdd.lastError();
    }

    return success;
}

bool DBManager::addCustomer(const Customers& customer)
{
    bool success = false;
    QString col_names;

    std::vector<QString> entries = customer.getEntries();

    for(std::vector<QString>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if( !col_names.isEmpty() )
        {
          col_names  += ", ";
        }

        col_names += *it;
    }

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO Kunden (" + col_names + ") VALUES (:firma, "
                                                                    ":name1, "
                                                                    ":name2, "
                                                                    ":strasse, "
                                                                    ":plz, "
                                                                    ":ort, "
                                                                    ":land, "
                                                                    ":telefon, "
                                                                    ":telefax, "
                                                                    ":email, "
                                                                    ":website, "
                                                                    ":rabatt, "
                                                                    ":kontostand, "
                                                                    ":info)");
    queryAdd.bindValue(":firma", customer.getFirma());
    queryAdd.bindValue(":name1", customer.getName1());
    queryAdd.bindValue(":name2", customer.getName2());
    queryAdd.bindValue(":strasse", customer.getStrasse());
    queryAdd.bindValue(":plz", customer.getPlz());
    queryAdd.bindValue(":ort", customer.getOrt());
    queryAdd.bindValue(":land", customer.getLand());
    queryAdd.bindValue(":telefon", customer.getTelefon());
    queryAdd.bindValue(":telefax", customer.getTelefax());
    queryAdd.bindValue(":email", customer.getEmail());
    queryAdd.bindValue(":website", customer.getWebsite());
    queryAdd.bindValue(":rabatt", customer.getRabatt());
    queryAdd.bindValue(":kontostand", customer.getKontostand());
    queryAdd.bindValue(":info", customer.getInfo());

    if(queryAdd.exec())
    {
        success = true;
        qDebug() << DEBUG_TAG << ": Customer added to database!";

    }
    else
    {
        qDebug() << DEBUG_TAG << ": Add customer to database failed - " << queryAdd.lastError();
    }

    return success;
}

bool DBManager::addBill(QString datum, double betrag)
{
    bool success = false;

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO Rechnungen (Datum, Betrag) VALUES (:datum, :betrag)");
    queryAdd.bindValue(":Datum", datum);
    queryAdd.bindValue(":Betrag", betrag);

    if(queryAdd.exec())
    {
        success = true;
        qDebug() << DEBUG_TAG << ": Bill added to database!";

    }
    else
    {
        qDebug() << DEBUG_TAG << ": Add bill to database failed - " << queryAdd.lastError();
    }

    return success;
}

bool DBManager::removeDbEntry(QString table, QString id)
{
    if(!isOpen())
    {
        qDebug() << DEBUG_TAG << ": Database not open!";
        return false;
    }

    QSqlQuery queryDelete;
    queryDelete.prepare("DELETE FROM '"+table+"' WHERE ID = '"+id+"'");

    if(!queryDelete.exec())
    {
        qDebug() << DEBUG_TAG << ": Remove database entry failed: " << queryDelete.lastError();
        return false;
    }
    else
        qDebug() << DEBUG_TAG << ": Remove database entry successfull!";

    return true;
}

void DBManager::removeBill(int billID)
{
    QString id = QString::number(billID);

    if(!isOpen())
    {
        qDebug() << DEBUG_TAG << ": Failed to open the database!";
        return;
    }

    if(id.isEmpty())
    {
        qDebug() << DEBUG_TAG << ": No bill selected!";
        return;
    }

    QSqlQuery queryDelete;
    queryDelete.prepare("DELETE FROM Rechnungen WHERE ID = '"+id+"'");

    if(!queryDelete.exec())
    {
        qDebug() << DEBUG_TAG << ": Remove bill failed: " << queryDelete.lastError();
    }
    else
        qDebug() << DEBUG_TAG << ": Remove bill successfull!";
}

bool DBManager::editCustomer(QString id, const Customers& customers)
{
    if(isOpen())
    {
        QSqlQuery query;

        query.prepare("UPDATE Kunden SET "
                              "Firma=:firma, "
                              "Name1=:name1, "
                              "Name2=:name2, "
                              "Strasse=:strasse, "
                              "PLZ=:plz, "
                              "Ort=:ort, "
                              "Land=:land, "
                              "Telefon=:telefon, "
                              "Telefax=:telefax, "
                              "Email=:email, "
                              "Website=:website, "
                              "Rabatt=:rabatt, "
                              "Kontostand=:kontostand, "
                              "Information=:info "
                      "WHERE ID='"+id+"'");

        query.bindValue(":firma", customers.getFirma());
        query.bindValue(":name1", customers.getName1());
        query.bindValue(":name2", customers.getName2());
        query.bindValue(":strasse", customers.getStrasse());
        query.bindValue(":plz", customers.getPlz());
        query.bindValue(":ort", customers.getOrt());
        query.bindValue(":land", customers.getLand());
        query.bindValue(":telefon", customers.getTelefon());
        query.bindValue(":telefax", customers.getTelefax());
        query.bindValue(":email", customers.getEmail());
        query.bindValue(":website", customers.getWebsite());
        query.bindValue(":rabatt", customers.getRabatt());
        query.bindValue(":kontostand",customers.getKontostand());
        query.bindValue(":info", customers.getInfo());


        if(query.exec())
        {
            return true;
        }
        else
        {
            qDebug() << DEBUG_TAG << ": Edit ERROR - " << query.lastError();
            return false;
        }
    }
    else
        return false;
}

int DBManager::readLastID(QString table) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM '"+table+"' ORDER BY ID DESC LIMIT 1; ");

    if(!query.exec())
        qDebug() << DEBUG_TAG << ": No table in database!";

    int idID = query.record().indexOf("ID");

    int lastId = -1;

    while(query.next())
        lastId = query.value(idID).toInt();

    return lastId;
}

Customers DBManager::readCustomer(QString customerID) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Kunden WHERE KdNr = '"+customerID+"'");

    Customers customer;

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": No table in database!";
    }
    else
    {
        int idKdNr = query.record().indexOf("KdNr");
        int idFirma = query.record().indexOf("Firma");
        int idName1= query.record().indexOf("Name1");
        int idName2 = query.record().indexOf("Name2");
        int idStrasse = query.record().indexOf("Strasse");
        int idPlz = query.record().indexOf("Plz");
        int idOrt = query.record().indexOf("Ort");
        int idLand= query.record().indexOf("Land");
        int idTelefon = query.record().indexOf("Telefon");
        int idTelefax= query.record().indexOf("Telefax");
        int idEmail= query.record().indexOf("Email");
        int idWebsite = query.record().indexOf("Website");
        int idRabatt = query.record().indexOf("Rabatt");
        int idKontostand = query.record().indexOf("Kontostand");
        int idInfo = query.record().indexOf("Information");

        while (query.next())
        {
            customer.setKdnr(query.value(idKdNr).toInt());
            customer.setFirma(query.value(idFirma).toString());
            customer.setName1(query.value(idName1).toString());
            customer.setName2(query.value(idName2).toString());
            customer.setStrasse(query.value(idStrasse).toString());
            customer.setPlz(query.value(idPlz).toInt());
            customer.setOrt(query.value(idOrt).toString());
            customer.setLand(query.value(idLand).toString());
            customer.setTelefon(query.value(idTelefon).toString());
            customer.setTelefax(query.value(idTelefax).toString());
            customer.setEmail(query.value(idEmail).toString());
            customer.setWebsite(query.value(idWebsite).toString());
            customer.setRabatt(query.value(idRabatt).toDouble());
            customer.setKontostand(query.value(idKontostand).toDouble());
            customer.setInfo(query.value(idInfo).toString());
        }
    }

    return customer;
}

bool DBManager::readCustomers(std::vector<Customers> &customers) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Kunden ORDER BY ID ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": No table in database!" << " " << query.lastError();
        return false;
    }

    int idKdNr = query.record().indexOf("KdNr");
    int idFirma = query.record().indexOf("Firma");
    int idName1= query.record().indexOf("Name1");
    int idName2 = query.record().indexOf("Name2");
    int idStrasse = query.record().indexOf("Strasse");
    int idPlz = query.record().indexOf("Plz");
    int idOrt = query.record().indexOf("Ort");
    int idLand= query.record().indexOf("Land");
    int idTelefon = query.record().indexOf("Telefon");
    int idTelefax= query.record().indexOf("Telefax");
    int idEmail= query.record().indexOf("Email");
    int idWebsite = query.record().indexOf("Website");
    int idRabatt = query.record().indexOf("Rabatt");
    int idKontostand = query.record().indexOf("Kontostand");
    int idInfo = query.record().indexOf("Information");

    while (query.next())
    {
        Customers customer;
        customer.setKdnr(query.value(idKdNr).toInt());
        customer.setFirma(query.value(idFirma).toString());
        customer.setName1(query.value(idName1).toString());
        customer.setName2(query.value(idName2).toString());
        customer.setStrasse(query.value(idStrasse).toString());
        customer.setPlz(query.value(idPlz).toInt());
        customer.setOrt(query.value(idOrt).toString());
        customer.setLand(query.value(idLand).toString());
        customer.setTelefon(query.value(idTelefon).toString());
        customer.setTelefax(query.value(idTelefax).toString());
        customer.setEmail(query.value(idEmail).toString());
        customer.setWebsite(query.value(idWebsite).toString());
        customer.setRabatt(query.value(idRabatt).toDouble());
        customer.setKontostand(query.value(idKontostand).toDouble());
        customer.setInfo(query.value(idInfo).toString());

        customers.push_back(customer);
    }
    qDebug() << DEBUG_TAG << ": Read all customers successfull!";

    return true;
}

QSqlQueryModel *DBManager::readDbData(QString table)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM '"+table+"' ORDER BY ID ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": Table " << table << " in database not found!" << " " << query.lastError();
    }

    QSqlQueryModel *modal = new QSqlQueryModel();
    modal->setQuery(query);

    return modal;
}

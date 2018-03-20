#include "dbmanager.h"
#include <vector>
#include <iostream>
#include <map>
#include <stdexcept>

#include "mainwindow.h"

DBManager::DBManager(const QString &path)
{
    QFileInfo fileInfo(path);

    // Datenbank öffnen
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    if (!m_db.open())
    {
       qDebug() << DEBUG_TAG << ": Error - Connection to Database fail!";
    }
    else
    {
       qDebug() << DEBUG_TAG << ": Database Connection to " + fileInfo.absoluteFilePath() + " successfull!";

    }
}

DBManager::~DBManager()
{
}

void DBManager::closeDatabase()
{
    if (m_db.isOpen())
    {
        // Datenbank schließen
        m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(m_db.connectionName());
        qDebug() << DEBUG_TAG << ": Connection to database closed!";
    }
}

int DBManager::readLastID(QString table) const
{
    int lastID = -1;

    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT seq FROM sqlite_sequence WHERE NAME='" + table + "';");

        if(!query.exec())
        {
            qDebug() << DEBUG_TAG << ": " << query.lastError();
        }
        else
        {
            while (query.next())
                lastID = query.value(0).toInt();
        }
    }
    else
    {
        qDebug() << "Database is not open!";
    }

    return lastID;
}

bool DBManager::dbEntryExist(QString table, QString id)
{
    QString ident = "";
    if (table == KUNDEN)
        ident = "KdNr";
    else if (table == ARTIKEL)
        ident = "ArtNr";
    else if (table == RECHNUNG)
        ident = "RgnNr";
    else if (table == POSITIONEN)
        ident = "PosNr";
    else
        return false;

    if (isOpen())
    {
        QSqlQuery querySearch;
        querySearch.prepare("SELECT " + ident + " FROM " + table);
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

    std::vector<QString> entries = article.getEntries();

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
    queryAdd.bindValue(":einheit", article.getUnit());
    queryAdd.bindValue(":bezeichnung", article.getName());
    queryAdd.bindValue(":preis", article.getPrice());
    queryAdd.bindValue(":beschreibung", article.getDescription());

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

bool DBManager::removeDBList(QString table)
{
    if(!isOpen())
    {
        qDebug() << DEBUG_TAG << ": Database not open!";
        return false;
    }

    QSqlQuery queryDelete;
    queryDelete.prepare("DELETE FROM " + table);

    if(!queryDelete.exec())
    {
        qDebug() << DEBUG_TAG << ": Remove all entries from database '" + table + "' failed: " << queryDelete.lastError();
        return false;
    }
    else
        qDebug() << DEBUG_TAG << ": Remove all entries from database '" + table + "' successfull!";

    return true;
}

bool DBManager::removeDbEntry(QString table, QString id)
{
    if(!isOpen())
    {
        qDebug() << DEBUG_TAG << ": Database not open!";
        return false;
    }

    QString ident = "";
    if (table == KUNDEN)
        ident = "KdNr";
    else if (table == ARTIKEL)
        ident = "ArtNr";
    else if (table == RECHNUNG)
        ident = "RgnNr";
    else if (table == POSITIONEN)
        ident = "PosNr";
    else
        return false;

    QSqlQuery queryDelete;
    queryDelete.prepare("DELETE FROM " + table + " WHERE " + ident + " = " + id);

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

bool DBManager::editArticle(QString id, const Articles& article)
{
    if(isOpen())
    {
        QSqlQuery query;

        query.prepare("UPDATE Artikel SET "
                              "Einheit=:einheit, "
                              "Bezeichnung=:bezeichnung, "
                              "Beschreibung=:beschreibung, "
                              "Preis=:preis "
                      "WHERE ArtNr='"+id+"'");

        query.bindValue(":einheit", article.getUnit());
        query.bindValue(":bezeichnung", article.getName());
        query.bindValue(":beschreibung", article.getDescription());
        query.bindValue(":preis", article.getPrice());

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

bool DBManager::editCustomer(QString id, const Customers& customer)
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
                      "WHERE KdNr='"+id+"'");

        query.bindValue(":firma", customer.getFirma());
        query.bindValue(":name1", customer.getName1());
        query.bindValue(":name2", customer.getName2());
        query.bindValue(":strasse", customer.getStrasse());
        query.bindValue(":plz", customer.getPlz());
        query.bindValue(":ort", customer.getOrt());
        query.bindValue(":land", customer.getLand());
        query.bindValue(":telefon", customer.getTelefon());
        query.bindValue(":telefax", customer.getTelefax());
        query.bindValue(":email", customer.getEmail());
        query.bindValue(":website", customer.getWebsite());
        query.bindValue(":rabatt", customer.getRabatt());
        query.bindValue(":kontostand",customer.getKontostand());
        query.bindValue(":info", customer.getInfo());


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

QMap<int, QString> DBManager::readFieldNames(QString table)
{
    QMap<int, QString> fields;

    if (m_db.open())
    {
        QSqlQuery query;
        query.prepare("select * from " + table + " LIMIT 0, 0");
        query.exec();

        for (int i = 0; i < query.record().count(); i++)
        {
            fields.insert(i, query.record().fieldName(i));
        }
    }
    else
    {
        qDebug() << "Database is not open!";
    }

    return fields;
}

//int DBManager::readLastID(QString table) const
//{
//    QString ident = "";
//    if (table == KUNDEN)
//        ident = "KdNr";
//    else if (table == ARTIKEL)
//        ident = "ArtNr";
//    else if (table == RECHNUNG)
//        ident = "RgnNr";
//    else if (table == POSITIONEN)
//        ident = "PosNr";
//    else
//        return -1;

//    QSqlQuery query;
//    query.prepare("SELECT last_insert_rowid()");

//    if(!query.exec())
//        qDebug() << DEBUG_TAG << ": " << query.lastError();

//    return query.lastInsertId().toInt();
//}

bool DBManager::readArticle(QString articleID, Articles &article)
{
    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Artikel WHERE ArtNr = " + articleID);

        if (!query.exec())
        {
            qDebug() << DEBUG_TAG << ": " << query.lastError();
            return false;
        }
        else
        {
            int idArtNr = query.record().indexOf("ArtNr");
            int idUnit = query.record().indexOf("Einheit");
            int idName= query.record().indexOf("Bezeichnung");
            int idPrice = query.record().indexOf("Preis");
            int idDescription = query.record().indexOf("Beschreibung");

            if (query.next())
            {
                article.setArtNr(query.value(idArtNr).toInt());
                article.setUnit(query.value(idUnit).toString());
                article.setName(query.value(idName).toString());
                article.setPrice(query.value(idPrice).toDouble());
                article.setDescription(query.value(idDescription).toString());
            }
            else
            {
                qDebug() << DEBUG_TAG << ": No article found!";
                return false;
            }
        }
    }
    else
    {
        qDebug() << "Database is not open!";
        return false;
    }

    return true;
}

bool DBManager::readCustomer(QString customerID, Customers &customer)
{
    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Kunden WHERE KdNr = " + customerID);

        if(!query.exec())
        {
            qDebug() << DEBUG_TAG << ": No table in database!";
            return false;
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
    }
    else
    {
        qDebug() << "Database is not open!";
        return false;
    }

    return true;
}

bool DBManager::readArticles(std::vector<Articles> &articles) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Artikel ORDER BY ArtNr ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": " << query.lastError();
        return false;
    }

    int idArtNr = query.record().indexOf("ArtNr");
    int idUnit = query.record().indexOf("Einheit");
    int idName= query.record().indexOf("Bezeichnung");
    int idPrice = query.record().indexOf("Preis");
    int idDescription = query.record().indexOf("Beschreibung");

    while (query.next())
    {
        Articles article;
        article.setArtNr(query.value(idArtNr).toInt());
        article.setUnit(query.value(idUnit).toString());
        article.setName(query.value(idName).toString());
        article.setPrice(query.value(idPrice).toDouble());
        article.setDescription(query.value(idDescription).toString());

        articles.push_back(article);
    }
    qDebug() << DEBUG_TAG << ": Read all articles successfull!";

    return true;
}

bool DBManager::readCustomers(std::vector<Customers> &customers) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Kunden ORDER BY KdNr ASC");

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

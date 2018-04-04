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

       // Read table fields from DB
       m_customerFields = readFieldNames(KUNDEN);
       m_articleFields = readFieldNames(ARTIKEL);
       m_invoiceFields = readFieldNames(RECHNUNGEN);
       m_positionFields = readFieldNames(POSITIONEN);
       m_settingsFields = readFieldNames(SETTINGS);
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
    QString ident = getDbIdent(table);

    if (isOpen())
    {
        QSqlQuery querySearch;
        querySearch.prepare("SELECT \"" + ident + "\" FROM " + table);
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

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO Artikel (\"" + m_articleFields[Einheit] + "\", "
                                         "\"" + m_articleFields[Bezeichnung] + "\", "
                                         "\"" + m_articleFields[Preis] + "\", "
                                         "\"" + m_articleFields[Beschreibung] + "\") "
                             "VALUES (:einheit, "
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

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO Kunden (\"" + m_customerFields[Firma] + "\", "
                                         "\"" + m_customerFields[Name1] + "\", "
                                         "\"" + m_customerFields[Name2] + "\", "
                                         "\"" + m_customerFields[Strasse] + "\", "
                                         "\"" + m_customerFields[Plz] + "\", "
                                         "\"" + m_customerFields[Ort] + "\", "
                                         "\"" + m_customerFields[Land] + "\", "
                                         "\"" + m_customerFields[Telefon] + "\", "
                                         "\"" + m_customerFields[Telefax] + "\", "
                                         "\"" + m_customerFields[Email] + "\", "
                                         "\"" + m_customerFields[Website] + "\", "
                                         "\"" + m_customerFields[Rabatt] + "\", "
                                         "\"" + m_customerFields[Kontostand] + "\", "
                                         "\"" + m_customerFields[Info] + "\") "
                            "VALUES (:firma, "
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

bool DBManager::readInvoices(std::vector<Invoices> &invoices)
{
    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Rechnungen ORDER BY \"" + m_invoiceFields[Invoice_RgNr] + "\" ASC");

        if(!query.exec())
        {
            qDebug() << DEBUG_TAG << ": No table in database!" << " " << query.lastError();
            return false;
        }
        else
        {
            int idRgNr = query.record().indexOf(m_invoiceFields[Invoice_RgNr]);
            int idKdNr = query.record().indexOf(m_invoiceFields[Invoice_KdNr]);
            int idRgDate= query.record().indexOf(m_invoiceFields[Invoice_RgDate]);
            int idSubDate = query.record().indexOf(m_invoiceFields[Invoice_SubDate]);
            int idAmount = query.record().indexOf(m_invoiceFields[Invoice_Amount]);
            int idDescription = query.record().indexOf(m_invoiceFields[Invoice_Description]);
            int idUst = query.record().indexOf(m_invoiceFields[Invoice_USt]);
            int idSkonto = query.record().indexOf(m_invoiceFields[Invoice_Skonto]);
            int idCurrency = query.record().indexOf(m_invoiceFields[Invoice_Currency]);

            while (query.next())
            {
                Invoices invoice;
                invoice.setRgnr(query.value(idRgNr).toInt());
                invoice.setKdnr(query.value(idKdNr).toInt());
                invoice.setRgdate(query.value(idRgDate).toString());
                invoice.setSubdate(query.value(idSubDate).toString());
                invoice.setAmount(query.value(idAmount).toDouble());
                invoice.setDescription(query.value(idDescription).toString());
                invoice.setUst(query.value(idUst).toDouble());
                invoice.setSkonto(query.value(idSkonto).toDouble());
                invoice.setCurrency(query.value(idCurrency).toString());

                invoices.push_back(invoice);
            }
            qDebug() << DEBUG_TAG << ": Read all invoices successfull!";
        }
    }
    else
    {
        qDebug() << "Database is not open!";
        return false;
    }

    return true;
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

QMap<int, QString> DBManager::getSettingsFields() const
{
    return m_settingsFields;
}

QMap<int, QString> DBManager::getPositionFields() const
{
    return m_positionFields;
}

QMap<int, QString> DBManager::getInvoiceFields() const
{
    return m_invoiceFields;
}

QMap<int, QString> DBManager::getArticleFields() const
{
    return m_articleFields;
}

QMap<int, QString> DBManager::getCustomerFields() const
{
    return m_customerFields;
}

bool DBManager::removeDbEntry(QString table, QString id)
{
    if(!isOpen())
    {
        qDebug() << DEBUG_TAG << ": Database not open!";
        return false;
    }

    QString ident = getDbIdent(table);

    QSqlQuery queryDelete;
    queryDelete.prepare("DELETE FROM " + table + " WHERE \"" + ident + "\" = " + id);

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
    queryDelete.prepare("DELETE FROM Rechnungen WHERE \"" + m_invoiceFields[0] + "\" = '"+id+"'");

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
                              "'"+m_articleFields[Einheit]+"' = :einheit, "
                              "'"+m_articleFields[Bezeichnung]+"' = :bezeichnung, "
                              "'"+m_articleFields[Beschreibung]+"' = :beschreibung, "
                              "'"+m_articleFields[Preis]+"' = :preis "
                      "WHERE \"" + m_articleFields[ArtNr] + "\" = '"+id+"'");

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
                              "'"+m_customerFields[Firma]+"'= :firma, "
                              "'"+m_customerFields[Name1]+"' = :name1, "
                              "'"+m_customerFields[Name2]+"' = :name2, "
                              "'"+m_customerFields[Strasse]+"' = :strasse, "
                              "'"+m_customerFields[Plz]+"' = :plz, "
                              "'"+m_customerFields[Ort]+"' = :ort, "
                              "'"+m_customerFields[Land]+"' = :land, "
                              "'"+m_customerFields[Telefon]+"' = :telefon, "
                              "'"+m_customerFields[Telefax]+"' = :telefax, "
                              "'"+m_customerFields[Email]+"' = :email, "
                              "'"+m_customerFields[Website]+"' = :website, "
                              "'"+m_customerFields[Rabatt]+"' = :rabatt, "
                              "'"+m_customerFields[Kontostand]+"' = :kontostand, "
                              "'"+m_customerFields[Info]+"' = :info "
                      "WHERE \"" + m_customerFields[KdNr] + "\" = '"+id+"'");

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

bool DBManager::editSetting(QString typ, QString data)
{
    if(isOpen())
    {
        QSqlQuery query;

        query.prepare("UPDATE Settings SET \"" + typ + "\" = :data");
        query.bindValue(":data", data);

        if(query.exec())
        {
            return true;
        }
        else
        {
            qDebug() << DEBUG_TAG << ": Edit Settings ERROR - " << query.lastError();
            return false;
        }
    }
    else
        return false;
}

bool DBManager::editSettings(Settings &settings)
{
    if(isOpen())
    {
        QSqlQuery query;

        query.prepare("UPDATE Settings SET \"" + m_settingsFields[Kontakt] + "\" = :kontakt, "
                                          "\"" + m_settingsFields[Anschrift] + "\" = :anschrift, "
                                          "\"" + m_settingsFields[Konto] + "\" = :konto, "
                                          "\"" + m_settingsFields[USt] + "\" = :ust, "
                                          "\"" + m_settingsFields[Thx] + "\" = :thx, "
                                          "\"" + m_settingsFields[FreeText] + "\" = :freetext");

        query.bindValue(":kontakt", settings.getKontakt());
        query.bindValue(":anschrift", settings.getAnschrift());
        query.bindValue(":konto", settings.getKonto());
        query.bindValue(":ust", settings.getUst());
        query.bindValue(":thx", settings.getThx());
        query.bindValue(":freetext", settings.getFreeText());

        if(query.exec())
        {
            return true;
        }
        else
        {
            qDebug() << DEBUG_TAG << ": Edit Settings ERROR - " << query.lastError();
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
//        ident = "Kd-Nr.";
//    else if (table == ARTIKEL)
//        ident = "Art-Nr.";
//    else if (table == RECHNUNG)
//        ident = "RgnNr";
//    else if (table == POSITIONEN)
//        ident = "Pos.";
//    else
//        return -1;

//    QSqlQuery query;
//    query.prepare("SELECT last_insert_rowid()");

//    if(!query.exec())
//        qDebug() << DEBUG_TAG << ": " << query.lastError();

//    return query.lastInsertId().toInt();
//}

bool DBManager::readSetting(QString typ, QString& data)
{
    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT \"" + typ + "\" FROM Settings");

        if (!query.exec())
        {
            qDebug() << DEBUG_TAG << ": " << query.lastError();
            return false;
        }
        else
        {
            if (query.next())
            {
                data = query.value(0).toString();
            }
            else
            {
                qDebug() << DEBUG_TAG << ": No phrase found!";
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

bool DBManager::readSettings(Settings &settings)
{

}

bool DBManager::readArticle(QString articleID, Articles &article)
{
    if (m_db.isOpen())
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM Artikel WHERE \"" + m_articleFields[ArtNr] + "\" = " + articleID);

        if (!query.exec())
        {
            qDebug() << DEBUG_TAG << ": " << query.lastError();
            return false;
        }
        else
        {
            int idArtNr = query.record().indexOf(m_articleFields[ArtNr]);
            int idUnit = query.record().indexOf(m_articleFields[Einheit]);
            int idName= query.record().indexOf(m_articleFields[Bezeichnung]);
            int idPrice = query.record().indexOf(m_articleFields[Preis]);
            int idDescription = query.record().indexOf(m_articleFields[Beschreibung]);

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
        query.prepare("SELECT * FROM Kunden WHERE \"" + m_customerFields[KdNr] + "\" = " + customerID);

        if(!query.exec())
        {
            qDebug() << DEBUG_TAG << ": No table in database!";
            return false;
        }
        else
        {
            int idKdNr = query.record().indexOf(m_customerFields[KdNr]);
            int idFirma = query.record().indexOf(m_customerFields[Firma]);
            int idName1= query.record().indexOf(m_customerFields[Name1]);
            int idName2 = query.record().indexOf(m_customerFields[Name2]);
            int idStrasse = query.record().indexOf(m_customerFields[Strasse]);
            int idPlz = query.record().indexOf(m_customerFields[Plz]);
            int idOrt = query.record().indexOf(m_customerFields[Ort]);
            int idLand= query.record().indexOf(m_customerFields[Land]);
            int idTelefon = query.record().indexOf(m_customerFields[Telefon]);
            int idTelefax= query.record().indexOf(m_customerFields[Telefax]);
            int idEmail= query.record().indexOf(m_customerFields[Email]);
            int idWebsite = query.record().indexOf(m_customerFields[Website]);
            int idRabatt = query.record().indexOf(m_customerFields[Rabatt]);
            int idKontostand = query.record().indexOf(m_customerFields[Kontostand]);
            int idInfo = query.record().indexOf(m_customerFields[Info]);

            if (query.next())
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
            else
            {
                qDebug() << DEBUG_TAG << ": No Data in customer " + customerID + " !";
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

bool DBManager::readArticles(std::vector<Articles> &articles) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Artikel ORDER BY \"" + m_articleFields[ArtNr] + "\" ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": " << query.lastError();
        return false;
    }

    int idArtNr = query.record().indexOf(m_articleFields[ArtNr]);
    int idUnit = query.record().indexOf(m_articleFields[Einheit]);
    int idName= query.record().indexOf(m_articleFields[Bezeichnung]);
    int idPrice = query.record().indexOf(m_articleFields[Preis]);
    int idDescription = query.record().indexOf(m_articleFields[Beschreibung]);

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
    query.prepare("SELECT * FROM Kunden ORDER BY \"" + m_customerFields[KdNr] + "\" ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": No table in database!" << " " << query.lastError();
        return false;
    }

    int idKdNr = query.record().indexOf(m_customerFields[KdNr]);
    int idFirma = query.record().indexOf(m_customerFields[Firma]);
    int idName1= query.record().indexOf(m_customerFields[Name1]);
    int idName2 = query.record().indexOf(m_customerFields[Name2]);
    int idStrasse = query.record().indexOf(m_customerFields[Strasse]);
    int idPlz = query.record().indexOf(m_customerFields[Plz]);
    int idOrt = query.record().indexOf(m_customerFields[Ort]);
    int idLand= query.record().indexOf(m_customerFields[Land]);
    int idTelefon = query.record().indexOf(m_customerFields[Telefon]);
    int idTelefax= query.record().indexOf(m_customerFields[Telefax]);
    int idEmail= query.record().indexOf(m_customerFields[Email]);
    int idWebsite = query.record().indexOf(m_customerFields[Website]);
    int idRabatt = query.record().indexOf(m_customerFields[Rabatt]);
    int idKontostand = query.record().indexOf(m_customerFields[Kontostand]);
    int idInfo = query.record().indexOf(m_customerFields[Info]);

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
    QString ident = getDbIdent(table);

    QSqlQuery query;
    query.prepare("SELECT * FROM '"+table+"' ORDER \"" + ident + "\" ASC");

    if(!query.exec())
    {
        qDebug() << DEBUG_TAG << ": Table " << table << " in database not found!" << " " << query.lastError();
    }

    QSqlQueryModel *modal = new QSqlQueryModel();
    modal->setQuery(query);

    return modal;
}

QString DBManager::getDbIdent(QString table)
{
    QString ident = "";
    if (table == KUNDEN)
        ident = m_customerFields[KdNr];
    else if (table == ARTIKEL)
        ident = m_articleFields[ArtNr];
    else if (table == RECHNUNGEN)
        ident = m_invoiceFields[0];
    else if (table == POSITIONEN)
        ident = m_positionFields[PosNr];

    return ident;
}

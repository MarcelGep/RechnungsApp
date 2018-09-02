#ifndef POSITIONS_H
#define POSITIONS_H

#include "articles.h"
#include <QString>

class Positions
{
public:
    Positions();
//    Positions(int pos, int rgnr, int artnr, int menge, double total);
    Positions(int pos, int rgnr, int artnr, QString beschreibung, QString einheit, int menge, double price, double total);
    Positions(int pos, int artnr, QString name, int menge, QString unit, double price, double total, QString description);
    ~Positions();

    int getRgnr() const;
    void setRgnr(int value);

    int getMenge() const;
    void setMenge(int value);

    double getTotal() const;
    void setTotal(double value);

    int getArtnr() const;
    void setArtnr(int value);

    Articles getArticle() const;
    void setArticle(const Articles &value);

    int getPos() const;
    void setPos(int value);

    QString getBeschreibung() const;
    void setBeschreibung(const QString &value);

    QString getEinheit() const;
    void setEinheit(const QString &value);

    double getPrice() const;
    void setPrice(double value);

private:
    int pos;
    int rgnr;
    int artnr;
    QString beschreibung;
    QString einheit;
    int menge;
    double price;
    double total;
    Articles article;
};

#endif // POSITIONS_H

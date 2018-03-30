#include "customers.h"

Customers::Customers()
{
    this->firma = "";
    this->name1 = "";
    this->name2 = "";
    this->strasse = "";
    this->plz = 0;
    this->ort = "";
    this->land = "";
    this->telefon = "";
    this->telefax = "";
    this->email = "";
    this->website = "";
    this->rabatt = 0;
    this->kontostand = 0;
    this->info = "";
}

Customers::Customers(const int &kdnr, const QString &firma,
                     const QString &name1,
                     const QString &name2,
                     const QString &strasse,
                     const int &plz,
                     const QString &ort, const QString &land,
                     const QString &telefon,
                     const QString &telefax,
                     const QString &email,
                     const QString &website,
                     const double &rabatt,
                     const double &kontostand,
                     const QString &info)
{
    this->kdnr = kdnr;
    this->firma = firma;
    this->name1 = name1;
    this->name2 = name2;
    this->strasse = strasse;
    this->plz = plz;
    this->ort = ort;
    this->land = land;
    this->telefon = telefon;
    this->telefax = telefax;
    this->email = email;
    this->website = website;
    this->rabatt = rabatt;
    this->kontostand = kontostand;
    this->info = info;
}

Customers::~Customers()
{
}

QString Customers::getWebsite() const
{
    return website;
}

void Customers::setWebsite(const QString &value)
{
    website = value;
}

QString Customers::getFirma() const
{
    return firma;
}

void Customers::setFirma(const QString &value)
{
    firma = value;
}

QString Customers::getName1() const
{
    return name1;
}

void Customers::setName1(const QString &value)
{
    name1 = value;
}

QString Customers::getName2() const
{
    return name2;
}

void Customers::setName2(const QString &value)
{
    name2 = value;
}

QString Customers::getStrasse() const
{
    return strasse;
}

void Customers::setStrasse(const QString &value)
{
    strasse = value;
}

int Customers::getPlz() const
{
    return plz;
}

void Customers::setPlz(int value)
{
    plz = value;
}

QString Customers::getOrt() const
{
    return ort;
}

void Customers::setOrt(const QString &value)
{
    ort = value;
}

QString Customers::getTelefon() const
{
    return telefon;
}

void Customers::setTelefon(const QString &value)
{
    telefon = value;
}

QString Customers::getEmail() const
{
    return email;
}

void Customers::setEmail(const QString &value)
{
    email = value;
}

double Customers::getRabatt() const
{
    return rabatt;
}

void Customers::setRabatt(const double &value)
{
    rabatt = value;
}

double Customers::getKontostand() const
{
    return kontostand;
}

void Customers::setKontostand(double value)
{
    kontostand = value;
}

QString Customers::getLand() const
{
    return land;
}

void Customers::setLand(const QString &value)
{
    land = value;
}

QString Customers::getTelefax() const
{
    return telefax;
}

void Customers::setTelefax(const QString &value)
{
    telefax = value;
}

QString Customers::getInfo() const
{
    return info;
}

void Customers::setInfo(const QString &value)
{
    info = value;
}

int Customers::getKdnr() const
{
    return kdnr;
}

void Customers::setKdnr(int value)
{
    kdnr = value;
}

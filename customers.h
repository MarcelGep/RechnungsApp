#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <QString>
#include <vector>

class Customers
{
    public:
        Customers();

        Customers(const int &kdnr,
                  const QString &firma,
                  const QString &name1,
                  const QString &name2,
                  const QString &strasse,
                  const int &plz,
                  const QString &ort,
                  const QString &land,
                  const QString &telefon, const QString &telefax,
                  const QString &email,
                  const QString &website,
                  const double &rabatt,
                  const double &kontostand,
                  const QString &info);

        ~Customers();

        QString getFirma() const;
        void setFirma(const QString &value);

        QString getName1() const;
        void setName1(const QString &value);

        QString getName2() const;
        void setName2(const QString &value);

        QString getStrasse() const;
        void setStrasse(const QString &value);

        int getPlz() const;
        void setPlz(int value);

        QString getOrt() const;
        void setOrt(const QString &value);

        QString getTelefon() const;
        void setTelefon(const QString &value);

        QString getEmail() const;
        void setEmail(const QString &value);

        double getRabatt() const;
        void setRabatt(const double &value);

        double getKontostand() const;
        void setKontostand(double value);

        QString getLand() const;
        void setLand(const QString &value);

        QString getTelefax() const;
        void setTelefax(const QString &value);

        QString getInfo() const;
        void setInfo(const QString &value);

        int getKdnr() const;
        void setKdnr(int value);

        QString getWebsite() const;
        void setWebsite(const QString &value);

private:
        int kdnr;
        QString firma;
        QString name1;
        QString name2;
        QString strasse;
        int plz;
        QString ort;
        QString land;
        QString telefon;
        QString telefax;
        QString email;
        QString website;
        double rabatt;
        double kontostand;
        QString info;
};

#endif // CUSTOMERS_H

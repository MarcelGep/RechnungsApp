#ifndef INVOICES_H
#define INVOICES_H

#include <QString>
#include <vector>

class Invoices
{
    public:
        Invoices();

        Invoices(int rgnr,
                 int kdnr,
                 QString rgdate,
                 QString subdate,
                 double amount,
                 QString description,
                 double ust,
                 double skonto,
                 QString currency);

        ~Invoices();

        int rgnr() const;
        void setRgnr(int rgnr);

        int kdnr() const;
        void setKdnr(int kdnr);

        QString rgdate() const;
        void setRgdate(const QString &rgdate);

        QString subdate() const;
        void setSubdate(const QString &subdate);

        double amount() const;
        void setAmount(double amount);

        QString description() const;
        void setDescription(const QString &description);

        double ust() const;
        void setUst(double ust);

        double skonto() const;
        void setSkonto(double skonto);

        QString currency() const;
        void setCurrency(const QString &currency);

private:
        int m_rgnr;
        int m_kdnr;
        QString m_rgdate;
        QString m_subdate;
        double m_amount;
        QString m_description;
        double m_ust;
        double m_skonto;
        QString m_currency;

};

#endif // INVOICES_H

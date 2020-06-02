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
                 double summe,
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

        double amount() const;
        void setAmount(double amount);

        int ust() const;
        void setUst(int ust);

        int skonto() const;
        void setSkonto(int skonto);

        QString currency() const;
        void setCurrency(const QString &currency);

private:
        int m_rgnr;
        int m_kdnr;
        QString m_rgdate;
        double m_amount;
        int m_ust;
        int m_skonto;
        QString m_currency;

};

#endif // INVOICES_H

#include "invoices.h"

Invoices::Invoices()
{
    m_rgnr = 0;
    m_kdnr = 0;
    m_amount = 0;
    m_rgdate = "";
    m_ust = 0;
    m_skonto = 0;
    m_currency = "";
}

Invoices::Invoices(int rgnr,
                   int kdnr,
                   QString rgdate,
                   double summe,
                   double ust,
                   double skonto,
                   QString currency)
{
    m_rgnr = rgnr;
    m_kdnr = kdnr;
    m_rgdate = rgdate;
    m_amount = summe;
    m_ust = ust;
    m_skonto = skonto;
    m_currency = currency;
}

Invoices::~Invoices()
{

}

int Invoices::rgnr() const
{
    return m_rgnr;
}

void Invoices::setRgnr(int rgnr)
{
    m_rgnr = rgnr;
}

int Invoices::kdnr() const
{
    return m_kdnr;
}

void Invoices::setKdnr(int kdnr)
{
    m_kdnr = kdnr;
}

QString Invoices::rgdate() const
{
    return m_rgdate;
}

void Invoices::setRgdate(const QString &rgdate)
{
    m_rgdate = rgdate;
}

double Invoices::amount() const
{
    return m_amount;
}

void Invoices::setAmount(double amount)
{
    m_amount = amount;
}

int Invoices::ust() const
{
    return m_ust;
}

void Invoices::setUst(int ust)
{
    m_ust = ust;
}

int Invoices::skonto() const
{
    return m_skonto;
}

void Invoices::setSkonto(int skonto)
{
    m_skonto = skonto;
}

QString Invoices::currency() const
{
    return m_currency;
}

void Invoices::setCurrency(const QString &currency)
{
    m_currency = currency;
}



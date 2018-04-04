#include "invoices.h"

Invoices::Invoices()
{
    m_rgnr = 0;
    m_kdnr = 0;
    m_amount = 0;
    m_rgdate = "";
    m_subdate = "";
    m_description = "";
    m_ust = 0;
    m_skonto = 0;
    m_currency = "";
}

Invoices::Invoices(int rgnr,
                   int kdnr,
                   QString rgdate,
                   QString subdate,
                   double amount,
                   QString description,
                   double ust,
                   double skonto,
                   QString currency)
{
    m_rgnr = rgnr;
    m_kdnr = kdnr;
    m_rgdate = rgdate;
    m_subdate = subdate;
    m_amount = amount;
    m_description = description;
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

QString Invoices::subdate() const
{
    return m_subdate;
}

void Invoices::setSubdate(const QString &subdate)
{
    m_subdate = subdate;
}

double Invoices::amount() const
{
    return m_amount;
}

void Invoices::setAmount(double amount)
{
    m_amount = amount;
}

QString Invoices::description() const
{
    return m_description;
}

void Invoices::setDescription(const QString &description)
{
    m_description = description;
}

double Invoices::ust() const
{
    return m_ust;
}

void Invoices::setUst(double ust)
{
    m_ust = ust;
}

double Invoices::skonto() const
{
    return m_skonto;
}

void Invoices::setSkonto(double skonto)
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



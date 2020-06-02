#include "articles.h"

Articles::Articles()
{
    m_artnr = 0;
    m_unit = "";
    m_description = "";
    m_price = 0;
}

Articles::Articles(const int &artnr,
                   const QString &description,
                   const QString &unit,
                   const double &price)
{
    m_artnr = artnr;
    m_description = description;
    m_unit = unit;
    m_price = price;
}

Articles::~Articles()
{

}

int Articles::getArtNr() const
{
    return m_artnr;
}

void Articles::setArtNr(const int &artnr)
{
    m_artnr = artnr;
}

QString Articles::getDescription() const
{
    return m_description;
}

void Articles::setDescription(const QString &description)
{
    m_description = description;
}

QString Articles::getUnit() const
{
    return m_unit;
}

void Articles::setUnit(const QString &unit)
{
    m_unit = unit;
}

double Articles::getPrice() const
{
    return m_price;
}

void Articles::setPrice(double price)
{
    m_price = price;
}

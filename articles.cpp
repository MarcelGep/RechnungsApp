#include "articles.h"

Articles::Articles()
{
    m_artnr = 0;
    m_name = "";
    m_unit = "";
    m_description = "";
    m_price = 0;
}

Articles::Articles(const int &artnr,
                   const QString &name,
                   const QString &description,
                   const QString &unit, const double &price)
{
    m_artnr = artnr;
    m_name = name;
    m_description = description;
    m_unit = unit;
    m_price = price;

    setTableColumns();
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

QString Articles::getName() const
{
    return m_name;
}

void Articles::setName(const QString &name)
{
    m_name = name;
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

std::vector<QString> Articles::getEntries() const
{
    return m_entries;
}

void Articles::setEntries(const std::vector<QString> &entries)
{
    m_entries = entries;
}

void Articles::setTableColumns()
{
    if (m_entries.size() > 0)
        m_entries.clear();

    m_entries.push_back("Einheit");
    m_entries.push_back("Bezeichnung");
    m_entries.push_back("Preis");
    m_entries.push_back("Beschreibung");
}

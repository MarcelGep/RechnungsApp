#include "settings.h"
#include <QDate>

Settings::Settings()
{
    m_kontakt = "";
    m_anschrift = "";
    m_konto = "";
    m_ust = "";
    m_thx = "";
    m_freeText = "";

    // current date
    m_date = m_date.currentDate();
    m_dateStr = (m_date.toString("dd.MM.yyyy"));

    // invoice labels
    invoiceNrLabel = "Rechnungs-Nr.:";
    invoiceDateLabel = "Rechnungsdatum:";
    deliveryDateLabel = "Lieferdatum:";
    deliveryNrLabel = "Lieferschein-Nr.:";
    customerNrLabel = "Kunden-Nr.:";
}

Settings::~Settings()
{

}

QString Settings::getKontakt() const
{
    return m_kontakt;
}

void Settings::setKontakt(const QString &kontakt)
{
    m_kontakt = kontakt;
}

QString Settings::getAnschrift() const
{
    return m_anschrift;
}

void Settings::setAnschrift(const QString &anschrift)
{
    m_anschrift = anschrift;
}

QString Settings::getKonto() const
{
    return m_konto;
}

void Settings::setKonto(const QString &konto)
{
    m_konto = konto;
}

QString Settings::getUst() const
{
    return m_ust;
}

void Settings::setUst(const QString &ust)
{
    m_ust = ust;
}

QString Settings::getThx() const
{
    return m_thx;
}

void Settings::setThx(const QString &thx)
{
    m_thx = thx;
}

QString Settings::getFreeText() const
{
    return m_freeText;
}

void Settings::setFreeText(const QString &freeText)
{
    m_freeText = freeText;
}

QDate Settings::getDate() const
{
    return m_date;
}

void Settings::setDate(const QDate &date)
{
    m_date = date;
}

QString Settings::getDateStr() const
{
    return m_dateStr;
}

void Settings::setDateStr(const QString &dateStr)
{
    m_dateStr = dateStr;
}

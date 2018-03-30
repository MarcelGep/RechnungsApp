#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDate>
#include <QString>

class Settings
{

    public:
        Settings();
        ~Settings();

        QString getKontakt() const;
        void setKontakt(const QString &kontakt);

        QString getAnschrift() const;
        void setAnschrift(const QString &anschrift);

        QString getKonto() const;
        void setKonto(const QString &konto);

        QString getUst() const;
        void setUst(const QString &ust);

        QString getThx() const;
        void setThx(const QString &thx);

        QString getFreeText() const;
        void setFreeText(const QString &freeText);

        QDate getDate() const;
        void setDate(const QDate &date);

        QString getDateStr() const;
        void setDateStr(const QString &dateStr);

private:
        QString m_kontakt;
        QString m_anschrift;
        QString m_konto;
        QString m_ust;
        QString m_thx;
        QString m_freeText;


        QString invoiceNrLabel;
        QString deliveryNrLabel;
        QString invoiceDateLabel;
        QString deliveryDateLabel;
        QString customerNrLabel;

        QString m_dateStr;
        QDate m_date;

};

#endif // SETTINGS_H

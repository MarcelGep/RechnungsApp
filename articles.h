#ifndef ARTICLES_H
#define ARTICLES_H

#include <QString>
#include <vector>

class Articles
{
public:
    Articles();

    Articles(const int &artnr,
             const QString &name,
             const QString &description,
             const QString &unit,
             const double &price);

    ~Articles();

    int getArtNr() const;
    void setArtNr(const int &artnr);

    QString getName() const;
    void setName(const QString &name);

    QString getDescription() const;
    void setDescription(const QString &description);

    QString getUnit() const;
    void setUnit(const QString &unit);

    double getPrice() const;
    void setPrice(double price);

private:
    int m_artnr;
    QString m_name;
    QString m_description;
    QString m_unit;
    double m_price;
};

#endif // ARTICLES_H

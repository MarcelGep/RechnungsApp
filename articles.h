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

    int artnr() const;
    void setArtnr(const int &artnr);

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString unit() const;
    void setUnit(const QString &unit);

    double price() const;
    void setPrice(double price);

    void setTableColumns();

    std::vector<QString> entries() const;
    void setEntries(const std::vector<QString> &entries);

private:
    int m_artnr;
    QString m_name;
    QString m_description;
    QString m_unit;
    double m_price;

    std::vector<QString> m_entries;
};

#endif // ARTICLES_H

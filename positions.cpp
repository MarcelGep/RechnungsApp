#include "positions.h"

Positions::Positions()
{
    pos = -1;
    rgnr = -1;
    artnr = -1;
    menge = -1;
    total = -1;
}

Positions::Positions(int pos, int rgnr, int artnr, int menge, double total)
{
    this->pos = pos;
    this->rgnr = rgnr;
    this->artnr = artnr;
    this->menge = menge;
    this->total = total;
}

Positions::Positions(int pos, int artnr, QString name, int menge, QString unit, double price, double total, QString description)
{
    this->pos = pos;
    this->artnr = artnr;
    this->menge = menge;
    this->total = total;
    article.setName(name);
    article.setUnit(unit);
    article.setPrice(price);
    article.setDescription(description);
}

Positions::~Positions()
{

}

int Positions::getRgnr() const
{
    return rgnr;
}

void Positions::setRgnr(int value)
{
    rgnr = value;
}

int Positions::getMenge() const
{
    return menge;
}

void Positions::setMenge(int value)
{
    menge = value;
}

double Positions::getTotal() const
{
    return total;
}

void Positions::setTotal(double value)
{
    total = value;
}

int Positions::getArtnr() const
{
    return artnr;
}

void Positions::setArtnr(int value)
{
    artnr = value;
}

Articles Positions::getArticle() const
{
    return article;
}

void Positions::setArticle(const Articles &value)
{
    article = value;
}

int Positions::getPos() const
{
    return pos;
}

void Positions::setPos(int value)
{
    pos = value;
}

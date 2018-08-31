#include "windowArticle.h"
#include "ui_windowArticle.h"
#include <QMessageBox>

WindowArticle::WindowArticle(QWidget *parent, Articles *article, DBManager *dbmanager) :
    QDialog(parent),
    ui(new Ui::WindowArticle),
    m_dbManager(dbmanager)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    ui->leArtNr->setText(QString::number(article->getArtNr()));
    ui->leArtUnit->setText(article->getUnit());
    ui->leArtPrice->setText(QString::number(article->getPrice(), 'f', 2));
    ui->leArtName->setText(article->getName());
    ui->ptArtDescription->setPlainText(article->getDescription());
}

WindowArticle::~WindowArticle()
{
    delete ui;
}

Articles WindowArticle::getArticle()
{
    Articles article;
    article.setArtNr(ui->leArtNr->text().toInt());
    article.setName(ui->leArtName->text());
    article.setPrice(ui->leArtPrice->text().toDouble());
    article.setUnit(ui->leArtUnit->text());
    article.setDescription(ui->ptArtDescription->toPlainText());

    return article;
}

void WindowArticle::on_btnArtCancel_clicked()
{
    reject();
}

void WindowArticle::on_btnArtSave_clicked()
{
    int artnr = ui->leArtNr->text().toInt();
    QString unit = ui->leArtUnit->text();
    QString name = ui->leArtName->text();
    double price = ui->leArtPrice->text().toDouble();
    QString description = ui->ptArtDescription->toPlainText();

    Articles article(artnr, name, description, unit, price);

    if (m_dbManager->editArticle(QString::number(artnr), article))
    {
        accept();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Fehler beim bearbeiten des Artikels!", QMessageBox::Ok);
        reject();
    }
}

void WindowArticle::clearArticleEdits()
{
    ui->leArtNr->clear();
    ui->leArtName->clear();
    ui->leArtPrice->clear();
    ui->leArtUnit->clear();
    ui->ptArtDescription->clear();
}

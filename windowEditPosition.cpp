#include "windowEditPosition.h"
#include "ui_windowEditPosition.h"

WindowEditPosition::WindowEditPosition(QWidget *parent, QString pos, QString rgnr, DBManager *dbmanager) :
    QDialog(parent),
    ui(new Ui::WindowEditPosition),
    m_dbManager(dbmanager),
    m_pos(pos),
    m_rgNr(rgnr)
{
    ui->setupUi(this);

    printPosition();
}

WindowEditPosition::~WindowEditPosition()
{
    delete ui;
}

void WindowEditPosition::printPosition()
{
    Positions position;
    m_dbManager->readPosition(position, m_pos, m_rgNr);

    ui->leEditPosPosition->setText(QString::number(position.getPos()));
    ui->leEditPosArtNr->setText(QString::number(position.getArtnr()));

    ui->teEditPosBezeichnung->setText(position.getBeschreibung());
    ui->teEditPosBezeichnung->selectAll();

    ui->leEditPosEinheit->setText(position.getEinheit());
    ui->leEditPosAnzahl->setText(QString::number(position.getMenge()));
    ui->leEditPosPreis->setText(QLocale().toCurrencyString(position.getPrice()).split(" ").value(0));
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(position.getTotal()).split(" ").value(0));
}

//resizeLineEditToContents

void WindowEditPosition::on_btnEditPosSave_clicked()
{
    int artnr = ui->leEditPosArtNr->text().toInt();
    QString beschreibung = ui->teEditPosBezeichnung->toPlainText();
    QString einheit = ui->leEditPosEinheit->text();
    int menge = ui->leEditPosAnzahl->text().toInt();
    double preis = ui->leEditPosPreis->text().replace(".", "").replace(",", ".").toDouble();
    double summe = ui->leEditPosSumme->text().replace(".", "").replace(",", ".").toDouble();

    Positions position(m_pos.toInt(), m_rgNr.toInt(), artnr, beschreibung, einheit, menge, preis, summe);

    m_dbManager->editPosition(position);

    reject();
}

void WindowEditPosition::on_btnEditPosCancel_clicked()
{
    reject();
}

void WindowEditPosition::on_leEditPosAnzahl_textChanged(const QString &menge)
{
    double summe = menge.toInt() * ui->leEditPosPreis->text().replace(".", "").replace(",", ".").toDouble();
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(summe).split(" ").value(0));
}

void WindowEditPosition::on_leEditPosPreis_textChanged(const QString &preis)
{
    QString preisNeu = preis;
    double summe = preisNeu.replace(".", "").replace(",", ".").toDouble() * ui->leEditPosAnzahl->text().toInt();
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(summe).split(" ").value(0));
}

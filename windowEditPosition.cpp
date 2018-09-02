#include "windowEditPosition.h"
#include "ui_windowEditPosition.h"

WindowEditPosition::WindowEditPosition(QWidget *parent, Positions position) :
    QDialog(parent),
    ui(new Ui::WindowEditPosition),
    m_position(position)
{
    ui->setupUi(this);

    ui->leEditPosPosition->setText(QString::number(m_position.getPos()));
    ui->leEditPosArtNr->setText(QString::number(m_position.getArtnr()));

    ui->teEditPosBezeichnung->setText(m_position.getBeschreibung());
    ui->teEditPosBezeichnung->selectAll();

    ui->leEditPosEinheit->setText(m_position.getEinheit());
    ui->leEditPosAnzahl->setText(QString::number(m_position.getMenge()));
    ui->leEditPosPreis->setText(QLocale().toCurrencyString(m_position.getPrice()).split(" ").value(0));
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(m_position.getTotal()).split(" ").value(0));
}

WindowEditPosition::~WindowEditPosition()
{
    delete ui;
}

//resizeLineEditToContents

void WindowEditPosition::on_btnEditPosSave_clicked()
{
    // TODO! Save edits!
    QString beschreibung = ui->teEditPosBezeichnung->toPlainText();
    QString einheit = ui->leEditPosEinheit->text();
    int menge = ui->leEditPosAnzahl->text().toInt();
    double preis = ui->leEditPosPreis->text().replace(".", "").replace(",", ".").toDouble();
    double summe = ui->leEditPosSumme->text().replace(".", "").replace(",", ".").toDouble();

    Positions position(m_position.getPos(), m_position.getRgnr(), m_position.getArtnr(), beschreibung, einheit, menge, preis, summe);

    emit editPosition(position);

    reject();
}

void WindowEditPosition::on_btnEditPosCancel_clicked()
{
    reject();
}

void WindowEditPosition::on_leEditPosAnzahl_textChanged(const QString &menge)
{
    double summe = menge.toInt() * ui->leEditPosPreis->text().replace(",", ".").toDouble();
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(summe).split(" ").value(0));
}

void WindowEditPosition::on_leEditPosPreis_textChanged(const QString &preis)
{
    QString preisNeu = preis;
    double summe = preisNeu.replace(",", ".").toDouble() * ui->leEditPosAnzahl->text().toInt();
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(summe).split(" ").value(0));
}

#include "windowEditPosition.h"
#include "ui_windowEditPosition.h"

WindowEditPosition::WindowEditPosition(QWidget *parent, Positions *position) :
    QDialog(parent),
    ui(new Ui::WindowEditPosition),
    m_position(position)
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
    ui->leEditPosPosition->setText(QString::number(m_position->getPos()));
    ui->leEditPosArtNr->setText(QString::number(m_position->getArtnr()));

    ui->teEditPosBezeichnung->setText(m_position->getBeschreibung());
    ui->teEditPosBezeichnung->selectAll();

    ui->leEditPosEinheit->setText(m_position->getEinheit());
    ui->leEditPosAnzahl->setText(QString::number(m_position->getMenge()));
    ui->leEditPosPreis->setText(QLocale().toCurrencyString(m_position->getPrice()).split(" ").value(0));
    ui->leEditPosSumme->setText(QLocale().toCurrencyString(m_position->getTotal()).split(" ").value(0));
}

//resizeLineEditToContents

void WindowEditPosition::on_btnEditPosSave_clicked()
{
    m_position->setArtnr(ui->leEditPosArtNr->text().toInt());
    m_position->setBeschreibung(ui->teEditPosBezeichnung->toPlainText());
    m_position->setEinheit(ui->leEditPosEinheit->text());
    m_position->setMenge(ui->leEditPosAnzahl->text().toInt());
    m_position->setPrice(ui->leEditPosPreis->text().replace(".", "").replace(",", ".").toDouble());
    m_position->setTotal(ui->leEditPosSumme->text().replace(".", "").replace(",", ".").toDouble());

    accept();
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

Positions *WindowEditPosition::position() const
{
    return m_position;
}

#include "windowEditPosition.h"
#include "ui_windoweditposition.h"

WindowEditPosition::WindowEditPosition(QWidget *parent, Positions position) :
    QDialog(parent),
    ui(new Ui::WindowEditPosition),
    m_position(position)
{
    ui->setupUi(this);

    ui->leEditPosPosition->setText(QString::number(m_position.getPos()));
    ui->leEditPosArtNr->setText(QString::number(m_position.getArtnr()));
    ui->leEditPosBezeichnung->setText(m_position.getArticle().getDescription());
    ui->leEditPosName->setText(m_position.getArticle().getName());
    ui->leEditPosEinheit->setText(m_position.getArticle().getUnit());
    ui->leEditPosPreis->setText(QString::number(m_position.getArticle().getPrice()));
    ui->leEditPosSumme->setText(QString::number(m_position.getTotal()));
    ui->leEditPosAnzahl->setText(QString::number(m_position.getMenge()));
}

WindowEditPosition::~WindowEditPosition()
{
    delete ui;
}

void WindowEditPosition::on_btnEditPosSave_clicked()
{
    // TODO! Save edits!

    accept();
}

void WindowEditPosition::on_btnEditPosCancel_clicked()
{
    reject();
}

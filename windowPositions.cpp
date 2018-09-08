#include "windowPositions.h"
#include "ui_windowPositions.h"
#include "windowEditPosition.h"

#include <QMessageBox>

WindowPositions::WindowPositions(QWidget *parent, QString rgnr, DBManager* dbmanager) :
    QDialog(parent),
    ui(new Ui::WindowPositions),
    m_dbManager(dbmanager),
    m_windowSize(0, 0),
    m_selectedRow( -1 ),
    m_rgNr(rgnr),
    m_summe( 0 )
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    // Setup invoice positions
    ui->twRgPositions->setColumnCount(ArtPosColumnsCount);

    ui->twRgPositions->setHorizontalHeaderItem(PosNr, new QTableWidgetItem("Pos."));
    ui->twRgPositions->setHorizontalHeaderItem(ArtNrPos, new QTableWidgetItem("Art-Nr."));
    ui->twRgPositions->setHorizontalHeaderItem(BeschreibungPos, new QTableWidgetItem("Beschreibung"));
    ui->twRgPositions->setHorizontalHeaderItem(AnzahlPos, new QTableWidgetItem("Anzahl"));
    ui->twRgPositions->setHorizontalHeaderItem(EinheitPos, new QTableWidgetItem("Einheit"));
    ui->twRgPositions->setHorizontalHeaderItem(EinzelPreisPos, new QTableWidgetItem("E-Preis"));
    ui->twRgPositions->setHorizontalHeaderItem(SummePos, new QTableWidgetItem("Gesamt"));

    QFont fontInvoicePos("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twRgPositions->horizontalHeader()->setFont(fontInvoicePos);
    ui->twRgPositions->horizontalHeader()->setSectionResizeMode(BeschreibungPos, QHeaderView::Stretch);

    printPositions();
}

WindowPositions::~WindowPositions()
{
    clearPositions();
    delete ui;
}

void WindowPositions::printPositions()
{
    double summe = 0;

    // clear invoice list
    if(ui->twRgPositions->rowCount() > 0)
        clearPositions();

    // read invoice positions from database
    if (!m_dbManager->readPositions(m_positions, m_rgNr))
    {
       qDebug() << DEBUG_TAG_MAIN << ": Error read invoice positions!";
       return;
    }

    // print all exist positions on the invoice no
    for (std::vector<Positions>::iterator it = m_positions.begin(); it != m_positions.end(); ++it)
    {
       int row = ui->twRgPositions->rowCount();
       ui->twRgPositions->insertRow(row);
       ui->twRgPositions->setRowHeight(row, INVOICE_ROW_HEIGHT);
       ui->twRgPositions->setItem(row, PosNr, new QTableWidgetItem(QString::number(it->getPos())));
       ui->twRgPositions->setItem(row, ArtNrPos, new QTableWidgetItem(QString::number(it->getArtnr())));
       ui->twRgPositions->setItem(row, BeschreibungPos, new QTableWidgetItem(it->getBeschreibung()));
       ui->twRgPositions->setItem(row, AnzahlPos, new QTableWidgetItem(QString::number(it->getMenge())));
       ui->twRgPositions->setItem(row, EinheitPos, new QTableWidgetItem(it->getEinheit()));
       ui->twRgPositions->setItem(row, EinzelPreisPos, new QTableWidgetItem(QLocale().toCurrencyString(it->getPrice())));
       ui->twRgPositions->item(row, EinzelPreisPos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twRgPositions->setItem(row, SummePos, new QTableWidgetItem(QLocale().toCurrencyString(it->getTotal())));
       ui->twRgPositions->item(row, SummePos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

       summe += it->getTotal();
    }

    // Set customer table column width offset
    setPositionsColumnsWidth();

    // Edit window size to table size
    m_windowSize.setHeight(ui->twRgPositions->rowHeight(1) * ui->twRgPositions->rowCount());
    resize(m_windowSize.width() + WINDOW_SIZE_WIDTH_OFFSET, m_windowSize.height() + WINDOW_SIZE_HEIGHT_OFFSET);
    this->setFixedSize(this->width(),this->height());

    ui->lePositionsSumme->setText(QLocale().toCurrencyString(summe));

    m_summe = summe;
}

void WindowPositions::clearPositions()
{
    while (ui->twRgPositions->rowCount() > 0)
    {
        ui->twRgPositions->removeRow(0);
    }
}

void WindowPositions::setPositionsColumnsWidth()
{
    ui->twRgPositions->resizeColumnsToContents();
    int windowSize = 0;

    for (int i = 0; i < ui->twRgPositions->columnCount(); i++)
    {
        int tempWidth = ui->twRgPositions->columnWidth(i);
        ui->twRgPositions->setColumnWidth(i, tempWidth + INVOICE_POSITIONS_OFFSET);
        windowSize += tempWidth + INVOICE_POSITIONS_OFFSET;
    }
    m_windowSize.setWidth(windowSize);
}

void WindowPositions::on_twRgPositions_itemDoubleClicked(QTableWidgetItem */*item*/)
{
    on_btnPositionEdit_clicked();
}

void WindowPositions::on_btnPositionEdit_clicked()
{
    if(ui->twRgPositions->selectedItems().size() > 0)
    {
        m_selectedRow = ui->twRgPositions->currentRow();

        WindowEditPosition epos(this, &m_positions[m_selectedRow]);
        epos.setWindowTitle("Position bearbeiten");
        if(epos.exec() == QDialog::Accepted)
        {
            Positions *position = epos.position();
            ui->twRgPositions->setItem(m_selectedRow, PosNr, new QTableWidgetItem(QString::number(position->getPos())));
            ui->twRgPositions->setItem(m_selectedRow, ArtNrPos, new QTableWidgetItem(QString::number(position->getArtnr())));
            ui->twRgPositions->setItem(m_selectedRow, BeschreibungPos, new QTableWidgetItem(position->getBeschreibung()));
            ui->twRgPositions->setItem(m_selectedRow, AnzahlPos, new QTableWidgetItem(QString::number(position->getMenge())));
            ui->twRgPositions->setItem(m_selectedRow, EinheitPos, new QTableWidgetItem(position->getEinheit()));
            ui->twRgPositions->setItem(m_selectedRow, EinzelPreisPos, new QTableWidgetItem(QLocale().toCurrencyString(position->getPrice())));
            ui->twRgPositions->item(m_selectedRow, EinzelPreisPos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            ui->twRgPositions->setItem(m_selectedRow, SummePos, new QTableWidgetItem(QLocale().toCurrencyString(position->getTotal())));
            ui->twRgPositions->item(m_selectedRow, SummePos)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

            double summe = 0;
            for(int i = 0; i < ui->twRgPositions->rowCount(); i++)
            {
                summe += ui->twRgPositions->item(i, SummePos)->text().split(" ").value(0).replace(".", "").replace(",", ".").toDouble();
            }

            ui->lePositionsSumme->setText(QLocale().toCurrencyString(summe));

            m_summe = summe;
            m_positions[m_selectedRow] = *position;
        }
    }
    else
    {
        QMessageBox::warning(this, "Position bearbeiten", "Es wurde keine Position ausgewählt!", QMessageBox::Ok);
    }
}

void WindowPositions::on_btnPositionDelete_clicked()
{
    if(ui->twRgPositions->selectedItems().size() > 0)
    {
        bool lastPosition = false;
        QMessageBox msg;
        msg.setWindowTitle("Position löschen");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Ja");
        msg.setButtonText(QMessageBox::No, "Nein");
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Question);

        if(ui->twRgPositions->rowCount() > 1)
        {
            msg.setText("Möchten Sie die Position löschen?");
        }
        else
        {
            msg.setText("Durch löschen der letzten Position wird automatisch die ausgwählte Rechnung gelöscht.\n\n"
                        "Möchten Sie fortfahren?");
            lastPosition = true;
        }

        if(msg.exec() == QMessageBox::Yes)
        {
            m_dbManager->deletePosition(m_rgNr, ui->twRgPositions->item(m_selectedRow, 0)->text());

            if(lastPosition)
            {
                m_dbManager->removeDbEntry(RECHNUNG, m_rgNr);
                reject();
            }
            else
            {
                printPositions();
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "Position löschen", "Es wurde keine Position ausgewählt!", QMessageBox::Ok);
    }
}

void WindowPositions::on_twRgPositions_itemClicked(QTableWidgetItem *item)
{
    m_selectedRow = item->row();
}

void WindowPositions::on_twRgPositions_itemChanged(QTableWidgetItem *item)
{
    //int i = 0;
}
void WindowPositions::on_twRgPositions_cellChanged(int row, int column)
{
    //int i = 0;
}

void WindowPositions::on_btnPositionsClose_clicked()
{
    reject();
}

void WindowPositions::on_btnPositionsSave_clicked()
{
    accept();
}

double WindowPositions::summe() const
{
    return m_summe;
}

std::vector<Positions> WindowPositions::positions() const
{
    return m_positions;
}

#include "windowPositions.h"
#include "ui_windowPositions.h"

#include "windowEditPosition.h"

WindowPositions::WindowPositions(QWidget *parent, std::vector<Positions> positions) :
    QDialog(parent),
    ui(new Ui::WindowPositions),
    m_positions(positions),
    m_windowSize(0, 0),
    m_selectedRow( -1 )
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
    // clear invoice list
    if(ui->twRgPositions->rowCount() > 0)
        clearPositions();

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
    }

    // Set customer table column width offset
    setPositionsColumnsWidth();

    // Edit window size to table size
    m_windowSize.setHeight(ui->twRgPositions->rowHeight(1) * ui->twRgPositions->rowCount());
    resize(m_windowSize.width() + WINDOW_SIZE_WIDTH_OFFSET, m_windowSize.height() + WINDOW_SIZE_HEIGHT_OFFSET);
    this->setFixedSize(this->width(),this->height());
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

void WindowPositions::on_btnClose_clicked()
{
    reject();
}

void WindowPositions::on_twRgPositions_itemDoubleClicked(QTableWidgetItem */*item*/)
{
    m_selectedRow = ui->twRgPositions->currentRow();

    int pos = m_positions[m_selectedRow].getPos();
    int artNr = m_positions[m_selectedRow].getArtnr();
    int rgnr = m_positions[m_selectedRow].getRgnr();
    QString description = m_positions[m_selectedRow].getBeschreibung();
    QString unit = m_positions[m_selectedRow].getEinheit();
    int menge = m_positions[m_selectedRow].getMenge();
    double price = m_positions[m_selectedRow].getPrice();
    double total = m_positions[m_selectedRow].getTotal();

    Positions position(pos, rgnr, artNr, description,unit, menge, price, total);

    WindowEditPosition epos(this, position);
    epos.setWindowTitle("Position bearbeiten");

    connect(&epos, SIGNAL(editPosition(Positions)), this, SLOT(positionChanged(Positions)));

    if(epos.exec() == QDialog::Rejected)
    {
        return;
    }
}

void WindowPositions::positionChanged(Positions position)
{
    m_positions[m_selectedRow] = position;
    printPositions();
}

std::vector<Positions> WindowPositions::positions() const
{
    return m_positions;
}

void WindowPositions::setPositions(const std::vector<Positions> &positions)
{
    m_positions = positions;
}

void WindowPositions::on_btnPositionSave_clicked()
{
    //TODO save rechnung to DB
}

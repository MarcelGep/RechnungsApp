#include "windowPositions.h"
#include "ui_windowPositions.h"

#include "windowEditPosition.h"

WindowPositions::WindowPositions(QWidget *parent, std::vector<Positions> positions) :
    QDialog(parent),
    ui(new Ui::WindowPositions),
    m_positions(positions),
    m_windowSize(0, 0)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    // Setup invoice positions
    ui->twRgPositions->setColumnCount(8);

    ui->twRgPositions->setHorizontalHeaderItem(0, new QTableWidgetItem("Pos."));
    ui->twRgPositions->setHorizontalHeaderItem(1, new QTableWidgetItem("Art-Nr."));
    ui->twRgPositions->setHorizontalHeaderItem(2, new QTableWidgetItem("Bezeichnung"));
    ui->twRgPositions->setHorizontalHeaderItem(3, new QTableWidgetItem("Beschreibung"));
    ui->twRgPositions->setHorizontalHeaderItem(4, new QTableWidgetItem("Anzahl"));
    ui->twRgPositions->setHorizontalHeaderItem(5, new QTableWidgetItem("Einheit"));
    ui->twRgPositions->setHorizontalHeaderItem(6, new QTableWidgetItem("E-Preis"));
    ui->twRgPositions->setHorizontalHeaderItem(7, new QTableWidgetItem("Gesamt"));

    QFont fontInvoicePos("MS Shell Dlg 2", 8, QFont::Bold);
    ui->twRgPositions->horizontalHeader()->setFont(fontInvoicePos);
    ui->twRgPositions->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->twRgPositions->horizontalHeader()->setSectionResizeMode(Article_Beschreibung, QHeaderView::Stretch);

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
       ui->twRgPositions->setItem(row, 0, new QTableWidgetItem(QString::number(it->getPos())));
       ui->twRgPositions->setItem(row, 1, new QTableWidgetItem(QString::number(it->getArtnr())));
       ui->twRgPositions->setItem(row, 2, new QTableWidgetItem(it->getArticle().getName()));
       ui->twRgPositions->setItem(row, 3, new QTableWidgetItem(it->getArticle().getDescription()));
       ui->twRgPositions->setItem(row, 4, new QTableWidgetItem(QString::number(it->getMenge())));
       ui->twRgPositions->setItem(row, 5, new QTableWidgetItem(it->getArticle().getUnit()));
       ui->twRgPositions->setItem(row, 6, new QTableWidgetItem(QString::number(it->getArticle().getPrice(), 'f', 2) + " €"));
       ui->twRgPositions->item(row, 6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       ui->twRgPositions->setItem(row, 7, new QTableWidgetItem(QString::number(it->getTotal(),'f', 2) + " €"));
       ui->twRgPositions->item(row, 7)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
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
    int selectedRow = ui->twRgPositions->currentRow();

    int pos = ui->twRgPositions->item(selectedRow, 0)->text().toInt();
    int artNr = ui->twRgPositions->item(selectedRow, 1)->text().toInt();
    QString name = ui->twRgPositions->item(selectedRow, 2)->text();
    QString description = ui->twRgPositions->item(selectedRow, 3)->text();
    int menge = ui->twRgPositions->item(selectedRow, 4)->text().toInt();
    QString unit = ui->twRgPositions->item(selectedRow, 5)->text();
    double price = ui->twRgPositions->item(selectedRow, 6)->text().replace(",", ".").toDouble();
    double total = ui->twRgPositions->item(selectedRow, 7)->text().replace(",", ".").toDouble();

    Positions position(pos, artNr, name, menge, unit, price, total, description);

    WindowEditPosition epos(this, position);
    epos.setWindowTitle("Position bearbeiten");
    if(epos.exec() == QDialog::Rejected)
        return;


//    for (std::vector<Positions>::iterator it = m_positions.begin(); it != m_positions.end(); ++it)
//    {
//       int row = ui->twRgPositions->rowCount();
//       ui->twRgPositions->insertRow(row);
//       ui->twRgPositions->setRowHeight(row, INVOICE_ROW_HEIGHT);
//       ui->twRgPositions->setItem(row, 0, new QTableWidgetItem(QString::number(it->getPos())));
//       ui->twRgPositions->setItem(row, 1, new QTableWidgetItem(QString::number(it->getArtnr())));
//       ui->twRgPositions->setItem(row, 2, new QTableWidgetItem(it->getArticle().getName()));
//       ui->twRgPositions->setItem(row, 3, new QTableWidgetItem(it->getArticle().getDescription()));
//       ui->twRgPositions->setItem(row, 4, new QTableWidgetItem(QString::number(it->getMenge())));
//       ui->twRgPositions->setItem(row, 5, new QTableWidgetItem(it->getArticle().getUnit()));
//       ui->twRgPositions->setItem(row, 6, new QTableWidgetItem(QString::number(it->getArticle().getPrice(), 'f', 2) + " €"));
//       ui->twRgPositions->item(row, 6)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
//       ui->twRgPositions->setItem(row, 7, new QTableWidgetItem(QString::number(it->getTotal(),'f', 2) + " €"));
//       ui->twRgPositions->item(row, 7)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    }
}

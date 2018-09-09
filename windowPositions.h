#ifndef WINDOWPOSITIONS_H
#define WINDOWPOSITIONS_H

#include <QDialog>

#include <mainwindow.h>
#include <positions.h>
#include <dbmanager.h>

#define WINDOW_SIZE_WIDTH_OFFSET  100
#define WINDOW_SIZE_HEIGHT_OFFSET 300

namespace Ui {
class WindowPositions;
}

class WindowPositions : public QDialog
{
    Q_OBJECT

public:
    explicit WindowPositions(QWidget *parent = nullptr, QString rgnr = "", DBManager *dbmanager = NULL);
    ~WindowPositions();

    void setPositions(const std::vector<Positions> &positions);
    std::vector<Positions> positions() const;
    double summe() const;

    bool positionDeleted() const;
    void setPositionDeleted(bool positionDeleted);

    QString deletedPos() const;
    void clearDeletedPos();

    bool deleteInvoice() const;
    void setDeleteInvoice(bool deleteInvoice);

private slots:
    void on_twRgPositions_itemDoubleClicked(QTableWidgetItem *);
    void on_btnPositionEdit_clicked();
    void on_btnPositionDelete_clicked();
    void on_twRgPositions_itemClicked(QTableWidgetItem *item);
    void on_twRgPositions_itemChanged(QTableWidgetItem *item);
    void on_twRgPositions_cellChanged(int row, int column);
    void on_btnPositionsClose_clicked();
    void on_btnPositionsSave_clicked();

private:
    Ui::WindowPositions *ui;
    DBManager *m_dbManager;
    QSize m_windowSize;
    int m_selectedRow;
    QString m_rgNr;
    std::vector<Positions> m_positions;
    double m_summe;
    bool m_positionDeleted;
    QString m_deletedPos;
    bool m_deleteInvoice;

    void printPositions();
    void clearPositions();
    void setPositionsColumnsWidth();
    void updateSumme();
};

#endif // WINDOWPOSITIONS_H

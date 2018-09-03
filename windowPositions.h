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

private slots:
    void on_btnClose_clicked();
    void on_twRgPositions_itemDoubleClicked(QTableWidgetItem *);
    void on_btnPositionEdit_clicked();
    void on_btnPositionDelete_clicked();
    void on_twRgPositions_itemClicked(QTableWidgetItem *item);

    void on_twRgPositions_itemChanged(QTableWidgetItem *item);

    void on_twRgPositions_cellChanged(int row, int column);

private:
    Ui::WindowPositions *ui;
    DBManager *m_dbManager;
    QSize m_windowSize;
    int m_selectedRow;
    QString m_rgNr;

    void printPositions();
    void clearPositions();
    void setPositionsColumnsWidth();
};

#endif // WINDOWPOSITIONS_H

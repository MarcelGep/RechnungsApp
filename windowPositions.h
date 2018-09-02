#ifndef WINDOWPOSITIONS_H
#define WINDOWPOSITIONS_H

#include <QDialog>
#include <mainwindow.h>
#include <positions.h>

#define WINDOW_SIZE_WIDTH_OFFSET  100
#define WINDOW_SIZE_HEIGHT_OFFSET 300

namespace Ui {
class WindowPositions;
}

class WindowPositions : public QDialog
{
    Q_OBJECT

public:
    explicit WindowPositions(QWidget *parent = nullptr, std::vector<Positions> positions = {});
    ~WindowPositions();

    std::vector<Positions> positions() const;
    void setPositions(const std::vector<Positions> &positions);

private slots:
    void on_btnClose_clicked();
    void on_twRgPositions_itemDoubleClicked(QTableWidgetItem *);
    void positionChanged(Positions position);

    void on_btnPositionSave_clicked();

private:
    Ui::WindowPositions *ui;
    std::vector<Positions> m_positions;
    QSize m_windowSize;
    int m_selectedRow;

    void printPositions();
    void clearPositions();
    void setPositionsColumnsWidth();
};

#endif // WINDOWPOSITIONS_H

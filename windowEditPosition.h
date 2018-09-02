#ifndef WINDOWEDITPOSITION_H
#define WINDOWEDITPOSITION_H

#include <QDialog>
#include "positions.h"

namespace Ui {
class WindowEditPosition;
}

class WindowEditPosition : public QDialog
{
    Q_OBJECT

public:
    explicit WindowEditPosition(QWidget *parent = nullptr, Positions position = {});
    ~WindowEditPosition();

private slots:
    void on_btnEditPosSave_clicked();
    void on_btnEditPosCancel_clicked();

    void on_leEditPosAnzahl_textChanged(const QString &menge);

    void on_leEditPosPreis_textChanged(const QString &preis);

private:
    Ui::WindowEditPosition *ui;
    Positions m_position;

signals:
    void editPosition(Positions);
};

#endif // WINDOWEDITPOSITION_H

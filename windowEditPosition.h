#ifndef WINDOWEDITPOSITION_H
#define WINDOWEDITPOSITION_H

#include <QDialog>
#include "positions.h"

#include <dbmanager.h>

namespace Ui {
class WindowEditPosition;
}

class WindowEditPosition : public QDialog
{
    Q_OBJECT

public:
    explicit WindowEditPosition(QWidget *parent = nullptr, QString pos = "", QString rgnr = "", DBManager *dbmanager = NULL);
    ~WindowEditPosition();

private slots:
    void on_btnEditPosSave_clicked();
    void on_btnEditPosCancel_clicked();
    void on_leEditPosAnzahl_textChanged(const QString &menge);
    void on_leEditPosPreis_textChanged(const QString &preis);

private:
    Ui::WindowEditPosition *ui;
    DBManager *m_dbManager;
    QString m_pos;
    QString m_rgNr;

    void printPosition();

signals:
    void editPosition(Positions);
};

#endif // WINDOWEDITPOSITION_H

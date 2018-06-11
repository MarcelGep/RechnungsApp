#ifndef WINDOWARTICLE_H
#define WINDOWARTICLE_H

#include <QDialog>
#include <QTableWidget>

#include <articles.h>

#include <dbmanager.h>


namespace Ui {
class WindowArticle;
}

class WindowArticle : public QDialog
{
    Q_OBJECT

public:
    explicit WindowArticle(QWidget *parent = 0, Articles *article = NULL, DBManager *dbmanager = NULL);
    ~WindowArticle();

private slots:
    void on_btnArtCancel_clicked();
    void on_btnArtSave_clicked();

private:
    Ui::WindowArticle *ui;
    DBManager *m_dbManager;
    void clearArticleEdits();
};

#endif // WINDOWARTICLE_H

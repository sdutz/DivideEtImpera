#ifndef MAINDLG_H
#define MAINDLG_H

#include <QDialog>
#include <QDir>
#include <QMimeData>

namespace Ui {
class MainDlg;
}

class MainDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MainDlg(QWidget *parent = 0);
    ~MainDlg();

private slots:
    void on_Select_clicked();

    void on_Run_clicked();

protected:
    void    dragEnterEvent(   QDragEnterEvent *event);
    void    dropEvent(        QDropEvent *event);
    void    dragLeaveEvent(   QDragLeaveEvent *event);
    void    dragMoveEvent(    QDragMoveEvent *event);

private :
    void    UpdateLayout() ;
    void    SetIcons() ;
    void    DoMsbBox( const QString& sMsg) ;

private:
    Ui::MainDlg *ui;

    QDir        m_qDir ;
    QString     m_sTitle ;
    QStringList m_slMonths ;
};

#endif // MAINDLG_H

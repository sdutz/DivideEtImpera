/*
    Divide et Impera : a small file utility to divide files into folders by date
    Copyright (C) <Lorenzo Zambelli>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "maindlg.h"
#include "ui_maindlg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>


// --------------------------------------------------------------------
#define CHECK_PTR_PARAM( p)   if ( p == NULL) return false ;
#define PIX_OFFS   1

// --------------------------------------------------------------------
bool
GetPixBtnSize( const QSize& btnSize, QSize* pPixSize)
{
    CHECK_PTR_PARAM( pPixSize)

    int nMin ;

    nMin = qMin( btnSize.width(), btnSize.height()) - PIX_OFFS ;
    pPixSize->setHeight( nMin) ;
    pPixSize->setWidth(  nMin) ;

    return true ;
}

// --------------------------------------------------------------------
MainDlg::MainDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDlg)
{
    ui->setupUi(this);

    ui->progBar->hide();
    ui->Run->setEnabled( false);
    SetIcons();

    m_sTitle = windowTitle() ;
    setAcceptDrops( true);
    m_slMonths << "Gennaio" << "Febbraio" << "Marzo" << "Aprile" << "Maggio" << "Giugno" << "Luglio" << "Agosto" << "Settembre" << "Ottobre" << "Novembre" << "Dicembre" ;
}

// --------------------------------------------------------------------
MainDlg::~MainDlg()
{
    delete ui;
}

//----------------------------------------------------
void
MainDlg::SetIcons()
{
    QIcon icon ;
    QSize pixSize ;

    GetPixBtnSize( ui->Run->size(), &pixSize) ;
    icon.addFile( "icons/start-here.png", pixSize) ;
    ui->Run->setIcon( icon) ;

    GetPixBtnSize( ui->Select->size(), &pixSize) ;
    icon.addFile( "icons/folder-image.png", pixSize) ;
    ui->Select->setIcon( icon) ;

    setWindowIcon( icon);
}

// --------------------------------------------------------------------
void
MainDlg::on_Select_clicked()
{
    QFileDialog qFile( this) ;

    qFile.setFileMode( QFileDialog::DirectoryOnly);

    qFile.setOption(   QFileDialog::ShowDirsOnly, true) ;

    if ( qFile.exec()) {
        m_qDir = qFile.directory() ;
        UpdateLayout() ;
    }

}

// --------------------------------------------------------------------
void
MainDlg::UpdateLayout()
{
    setWindowTitle( m_sTitle + "   " + m_qDir.absolutePath());
    ui->Run->setEnabled( true);
}

// --------------------------------------------------------------------
void
MainDlg::on_Run_clicked()
{
    bool          bOk ;
    int           nFiles ;
    int           nCopied ;
    int           nCreated ;
    QDate         qDate ;
    QDir          qLev1 ;
    QDir          qLev2 ;
    QString       sYear ;
    QString       sMonth ;
    QString       sFileName ;
    QFileInfoList qInfoList ;

    m_qDir.setFilter( QDir::Files);
    qInfoList = m_qDir.entryInfoList() ;
    nFiles    = qInfoList.count() ;
    bOk       = true ;
    nCopied   = 0 ;
    nCreated  = 0 ;
    ui->progBar->show();
    ui->progBar->setRange( 0, nFiles - 1) ;

    for ( int i = 0 ;  i < nFiles  &&  bOk ;  i ++) {
         qDate = qInfoList.at(i).lastModified().date() ;

         sYear = QString::number( qDate.year()) ;
         qLev1.setPath( m_qDir.absolutePath() + "/" + sYear);
         if ( ! qLev1.exists()) {
            bOk = bOk  &&  m_qDir.mkdir( sYear) ;
            nCreated ++ ;
         }

         sMonth = m_slMonths.at( qDate.month() - 1) ;
         qLev2.setPath( qLev1.absolutePath() + "/" + sMonth) ;
         if ( ! qLev2.exists()) {
             bOk = bOk  &&  qLev1.mkdir( sMonth) ;
             nCreated ++ ;
         }

        sFileName = qInfoList.at(i).fileName() ;
        if ( bOk  &&  QFile::copy( qInfoList.at(i).absoluteFilePath(), qLev2.absolutePath() + "/" + sFileName)) {
            nCopied ++ ;
            bOk = bOk  &&  m_qDir.remove( sFileName) ;
        }

        ui->progBar->setValue( i);

    }


    DoMsbBox( QString( "Create %1 cartelle, spostati %2 file su %3").arg( nCreated).arg( nCopied).arg( nFiles)) ;
    ui->progBar->hide();
}

//----------------------------------------------------
void
MainDlg::DoMsbBox( const QString& sMsg)
{
    QMessageBox box ;

    box.setText( sMsg);
    box.exec() ;
}

//----------------------------------------------------
void
MainDlg::dragEnterEvent( QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

//----------------------------------------------------
void
MainDlg::dragLeaveEvent( QDragLeaveEvent *event)
{
    event->accept();
}

//----------------------------------------------------
void
MainDlg::dropEvent( QDropEvent *event)
{
    const QMimeData* pData ;
    QDir qDir ;

    pData = event->mimeData() ;

    if ( ! pData->hasUrls())
        return ;

    qDir.setPath( pData->urls().at(0).toLocalFile()) ;

    if ( qDir.exists()) {
        m_qDir = qDir ;
        UpdateLayout();
    }
}

//----------------------------------------------------
void
MainDlg::dragMoveEvent( QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


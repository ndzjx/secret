
#include <QPalette>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "MainForm.h"
#include "CUploadWidget.h"
#include "CSettingWidget.h"
#include "CBrowserWidget.h"
#include "GHYTitleBar.h"

#ifdef Q_OS_WIN
#pragma comment( lib, "user32.lib" )
#include <qt_windows.h>
#endif

MainForm::MainForm(QWidget *parent) : QWidget(parent)
{
	QPalette pal( palette() ) ;
    pal.setColor( QPalette::Background, QColor( "#F2F2F2" ) ) ;
    setAutoFillBackground( true ) ;
    setPalette( pal ) ;
	
	auto pTitleBar = new GHYTitleBar( this ) ;
	pTitleBar->init( Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint ) ;
	setWindowTitle( QStringLiteral( "Ò×ÔÆ" ) ) ;
	setWindowIcon( QPixmap( ":/res/main.png" ) ) ;

	auto pUpload = new CUploadWidget( this ) ;
	m_pBrowserWidget = new CBrowserWidget( this ) ;
	auto pSetting = new CSettingWidget( this ) ;

	auto pTab = new QTabWidget( this ) ;
	pTab->addTab( pUpload, QPixmap( ":/res/tabupload.png" ), QStringLiteral( "Í¬²½" ) ) ;
	pTab->addTab( m_pBrowserWidget, QPixmap( ":/res/tabbrower.png" ), QStringLiteral( "ä¯ÀÀ" ) ) ;
	pTab->addTab( pSetting, QPixmap( ":/res/tabsetting.png" ), QStringLiteral( "ÅäÖÃ" ) ) ;
	
	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->setSpacing( 0 ) ;
	pLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	pLayout->addWidget( pTitleBar ) ;

	auto pSubLayout = new QVBoxLayout( this ) ;
	pSubLayout->setSpacing( 0 ) ;
	pSubLayout->setContentsMargins( 10, 10, 10, 10 ) ;
	pSubLayout->addWidget( pTab ) ;
	pLayout->addLayout( pSubLayout ) ;

	this->setLayout( pLayout ) ;
	this->setFixedSize( 480, 640 ) ;
}

void MainForm::mousePressEvent( QMouseEvent* event )
{
#ifdef Q_OS_WIN
	if ( ReleaseCapture() )
	{
		SendMessage( HWND( winId() ), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0 ) ;
		event->ignore() ;
	}
#endif
}

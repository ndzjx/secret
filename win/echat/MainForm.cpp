
#include <QPalette>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "MainForm.h"
#include "GHYTitleBar.h"
#include "eChatWidget.h"
#include "CSettingWidget.h"

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
	setWindowTitle( QStringLiteral( "易聊" ) ) ;
	setWindowIcon( QPixmap( ":/res/main.png" ) ) ;

	auto pTab = new QTabWidget( this ) ;
	m_pChatWidget = new eChatWidget( this ) ;
    pTab->addTab( m_pChatWidget, QPixmap( ":/res/tabsetting.png" ), QStringLiteral( "2017黑马交流群" ) ) ;
	pTab->addTab( new CSettingWidget( this ), QPixmap( ":/res/tabsetting.png" ), QStringLiteral( "配置" ) ) ;
	
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
    this->setFixedSize( 480, 800 ) ;
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


#include <QBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QDragEnterEvent>
#include "CUploadWidget.h"

CUploadWidget::CUploadWidget(QWidget *parent)
	: QWidget(parent)
{
	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	pLayout->setSpacing( 0 ) ;

	auto pLabel = new QLabel( this ) ;
	pLabel->setPixmap( QPixmap( ":/res/upbk.png" ) ) ;
	pLayout->addWidget( pLabel ) ;
	
	pLabel->setAcceptDrops( false ) ;
	setAcceptDrops( true ) ;
}

void CUploadWidget::dragEnterEvent( QDragEnterEvent* e )
{
// 	if ( e->mimeData()->hasFormat( "text/uri-list" ) )
// 	{
// 		e->acceptProposedAction() ;
// 	}

    e->mimeData();
}

void CUploadWidget::dropEvent( QDropEvent* e )
{

}


#include <QBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QHeaderView>
#include "CUploadWidget.h"

CUploadWidget::CUploadWidget(QWidget *parent) :
	QWidget( parent ),
	m_pTable( NULL )
{
	auto pLabel = new QLabel( this ) ;
	pLabel->setAcceptDrops( false ) ;
	pLabel->setPixmap( QPixmap( ":/res/upbk.png" ) ) ;

	m_pTable = new QTableWidget( 0, 3, this ) ;
	m_pTable->verticalHeader()->setVisible( false ) ;
	m_pTable->horizontalHeader()->setStretchLastSection( true ) ;
	m_pTable->setColumnWidth( 0, 250 ) ;
	m_pTable->setColumnWidth( 1, 80 ) ;
    m_pTable->setHorizontalHeaderLabels( QStringList()
		<< QStringLiteral( "文件" )
		<< QStringLiteral( "大小" )
		<< QStringLiteral( "状态" ) ) ;

	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	pLayout->setSpacing( 0 ) ;
	pLayout->addWidget( pLabel ) ;
	pLayout->addWidget( m_pTable ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowTitle( QStringLiteral( "数据上传" ) ) ;
}

void CUploadWidget::dragEnterEvent( QDragEnterEvent* e )
{
	if ( e->mimeData()->hasFormat( "text/uri-list" ) )
	{
		e->acceptProposedAction() ;
	}
}

void CUploadWidget::dropEvent( QDropEvent* e )
{
	auto urls = e->mimeData()->urls() ;
	if ( urls.isEmpty() )
	{
		return ;
	}

	vector<QString> items ;
	foreach( QUrl url, urls )
	{
		items.emplace_back( url.toLocalFile() ) ;
	}

	if ( !items.empty() )
	{
		addTableItems( items ) ;
	}
}

void CUploadWidget::addTableItems( const vector<QString>& items )
{
	for ( auto&& it : items )
	{
		auto row = m_pTable->rowCount() ;
		m_pTable->setRowCount( row + 1 ) ;

		{
			auto item_file_name = new QTableWidgetItem( it ) ;
			item_file_name->setData( Qt::UserRole, it ) ;
			item_file_name->setToolTip( it ) ;

			boost::filesystem::path path( it.toStdString() ) ;
			item_file_name->setText( path.filename().generic_string().c_str() ) ;
			m_pTable->setItem( row, 0, item_file_name ) ;
		}

		{
			boost::filesystem::path path( it.toStdString() ) ;
			auto bytes = (double)secret::file_size( path.generic_string().c_str() ) ;
			bytes /= 1024 * 1024 ;

			char out[ 64 ] = { 0 } ;
			sprintf_s( out, "%.2fM", bytes ) ;

			m_pTable->setItem( row, 1, new QTableWidgetItem( out ) ) ;
		}

		{
			m_pTable->setItem( row, 2, new QTableWidgetItem( QStringLiteral( "准备中..." ) ) ) ;
		}

		setTableItemStatus( row, 1 ) ;
	}
}

void CUploadWidget::setTableItemStatus( int item, int status )
{
	QBrush br ;
	switch ( status )
	{
		case 1 :
			br = QBrush( QColor( 255, 0, 0 ) ) ;
			break ;

		default :
			return ;
	}

	for ( int i = 0 ; i < m_pTable->columnCount() ; ++i )
	{
		m_pTable->item( item, i )->setBackground( br ) ;
	}
}

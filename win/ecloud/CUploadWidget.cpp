
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
		<< QStringLiteral( "�ļ�" )
		<< QStringLiteral( "��С" )
		<< QStringLiteral( "״̬" ) ) ;

	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	pLayout->setSpacing( 0 ) ;
	pLayout->addWidget( pLabel ) ;
	pLayout->addWidget( m_pTable ) ;

	this->setAcceptDrops( true ) ;
	this->setWindowTitle( QStringLiteral( "�����ϴ�" ) ) ;

	connect( this, &CUploadWidget::tableItemStatusChanged,
		this, &CUploadWidget::setTableItemStatus, Qt::QueuedConnection ) ;
}

void CUploadWidget::dragEnterEvent( QDragEnterEvent* e )
{
	e->acceptProposedAction() ;
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

		auto filename = it.toLocal8Bit().toStdString() ;

		{
			auto item_file_name = new QTableWidgetItem( it ) ;
			item_file_name->setToolTip( it ) ;
			item_file_name->setText( QString().fromLocal8Bit(
				boost::filesystem::path( filename ).filename().generic_string().c_str() ) ) ;
			m_pTable->setItem( row, 0, item_file_name ) ;
		}

		{
			auto bytes = (double)secret::file_size( filename.c_str() ) ;
			bytes /= 1024 * 1024 ;

			char out[ 64 ] = { 0 } ;
			sprintf_s( out, "%.2fM", bytes ) ;

			m_pTable->setItem( row, 1, new QTableWidgetItem( out ) ) ;
		}

		{
			m_pTable->setItem( row, 2, new QTableWidgetItem( QStringLiteral( "׼����..." ) ) ) ;
		}

		// ���ϴ������ύ�����е�������
		global_pc().post( [ = ]
		{
			file_meta fm ;
			if ( meta_from_file( fm, filename.c_str() ) != 0 )
			{
				// �Ƿ��ļ�
				emit tableItemStatusChanged( row, 3 ) ;
				return ;
			}

			// �ļ��Ѿ����������д���
			if ( global_cloudfile_exist( fm.id ) )
			{
				emit tableItemStatusChanged( row, 1 ) ;
				return ;
			}

			// ����һ���ڵ��ϴ��ɹ����ɹ�
			for ( auto&& dbnode : global_cloudnodes() )
			{
				auto node = dbnode.to_meta() ;

				// �ϴ���
				emit tableItemStatusChanged( row, 2 ) ;

				// �ϴ��ɹ�
				if ( file_to_service( node, fm, filename.c_str(), node.user.c_str() ) == 200 )
				{
					emit tableItemStatusChanged( row, 0 ) ;
					return ;
				}
			}

			// �ϴ�ʧ��
			emit tableItemStatusChanged( row, 4 ) ;
		} ) ;
	}
}

void CUploadWidget::setTableItemStatus( int item, int status )
{
	QBrush br ;
	switch ( status )
	{
		case 0 :
			m_pTable->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "���" ) ) ) ;
			br = QBrush( QColor( 128, 255, 0 ) ) ;
			break ;

		case 1 :
			m_pTable->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "�Ѵ���" ) ) ) ;
			br = QBrush( QColor( 128, 255, 0 ) ) ;
			break ;

		case 2 :
			m_pTable->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "������..." ) ) ) ;
			br = QBrush( QColor( 230, 230, 0 ) ) ;
			break ;

		case 3 :
			m_pTable->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "�Ƿ��ļ�" ) ) ) ;
			br = QBrush( QColor( 255, 90, 90 ) ) ;
			break ;

		case 4 :
			m_pTable->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "ʧ��" ) ) ) ;
			br = QBrush( QColor( 255, 90, 90 ) ) ;
			break ;

		default :
			return ;
	}

	for ( int i = 0 ; i < m_pTable->columnCount() ; ++i )
	{
		m_pTable->item( item, i )->setBackground( br ) ;
	}
}


#include <QDateTime>
#include <QHostInfo>
#include <QTemporaryFile>
#include <QMessageBox>
#include <QDir>
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
	m_pTable->setColumnWidth( 0, 600 ) ;
	m_pTable->setColumnWidth( 1, 80 ) ;
    m_pTable->setHorizontalHeaderLabels( QStringList()
		<< QStringLiteral( "�ļ�" )
		<< QStringLiteral( "��С" )
		<< QStringLiteral( "״̬" ) ) ;

	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	pLayout->setSpacing( 0 ) ;
	pLayout->addWidget( m_pTable ) ;
	pLayout->addWidget( pLabel ) ;

	this->setFixedHeight( 800 ) ;
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

void dir_scan( QString path, vector<pair<string,string>>& result )
{
	QDir dir( path ) ;
	
	foreach( QFileInfo fi, dir.entryInfoList() )
	{
		if ( fi.isFile() )
		{
			auto file = fi.absoluteFilePath().toLocal8Bit().toStdString() ;
			if ( file.empty() )
			{
				continue ;
			}

			result.emplace_back( "", file ) ;
		}

		else
		{
			if ( fi.fileName() == "." || fi.fileName() == ".." )
			{
				continue ;
			}
			
			result.emplace_back( ".dir", fi.absoluteFilePath().toLocal8Bit().toStdString() ) ;
			dir_scan( fi.absoluteFilePath(), result ) ;
		}
	}
}

void CUploadWidget::addTableItems( const vector<QString>& items )
{
	// ���� : �ϴ������ļ�
	auto task_upload = [ this ]( auto filename, auto row )
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

		// �����ļ��ƶ�Ŀ�ĵ�
		auto cloud = global_cloudnodes() ;
		string dist ;
		for ( auto&& dbnode : cloud )
		{
			dist += dbnode.user ;
			dist += ',' ;
		}

		// ʹ������һ����Ч�ڵ�����ϴ�����
		for ( auto&& dbnode : cloud )
		{
			auto node = dbnode.to_meta() ;
				
			// �ϴ���
			emit tableItemStatusChanged( row, 2 ) ;

			// С�ļ�ֱ���ϴ�
			if ( fm.bytes <= CHUNK_SIZE )
			{
				// �ϴ��ɹ�
				if ( file_to_service( node, fm, filename.c_str(), dist.c_str() ) == 200 )
				{
					emit tableItemStatusChanged( row, 0 ) ;
					return ;
				}

				continue ;
			}

			ParallelMapReduce<bool> pmr ;

			// ���ļ��ֿ��ϴ�
			for ( auto&& chunk : meta_split_chunk( fm, CHUNK_SIZE ) )
			{
				// ÿ����ʹ�ö��������ϴ�
				pmr.map( global_pc(), [ node, chunk, filename, dist ]( auto result )
				{
					// ���Ѿ����ھͲ����ϴ���
					dbmeta_cloudfile dbchunk ;
					dbchunk.from_meta( chunk, node.user, 0 ) ;
					if ( global_cloudchunk_exist( dbchunk ) )
					{
						result->set_value( true ) ;
						return ;
					}

					// �ϴ��ÿ�
					if ( file_to_service( node, chunk, filename.c_str(), dist.c_str() ) == 200 )
					{
						result->set_value( true ) ;
						return ;
					}

					result->set_value( false ) ;
				} ) ;
			}

			// �ռ����п���ϴ����
			bool allok = true ;
			pmr.reduce( [ &allok ]( bool ok )
			{
				if ( ok == false )
				{
					allok = false ;
				}
			} ) ;

			if ( allok )
			{
				// �ϴ��ɹ�
				emit tableItemStatusChanged( row, 0 ) ;
				return ;
			}
		}

		// �ϴ�ʧ��
		emit tableItemStatusChanged( row, 4 ) ;
	} ;

	// UI : �ļ��ϴ�
	auto file_upload = [ this, task_upload ]( auto fi )
	{
		const auto row = m_pTable->rowCount() ;
		m_pTable->setRowCount( row + 1 ) ;

		{
			auto item_file_name = new QTableWidgetItem() ;
			item_file_name->setToolTip( fi.absoluteFilePath() ) ;
			item_file_name->setText( fi.fileName() ) ;
			m_pTable->setItem( row, 0, item_file_name ) ;
		}

		{
			auto bytes = (double)fi.size() ;
			bytes /= 1024 * 1024 ;

			char out[ 64 ] = { 0 } ;
			sprintf_s( out, "%.2fM", bytes ) ;

			m_pTable->setItem( row, 1, new QTableWidgetItem( out ) ) ;
		}

		{
			m_pTable->setItem( row, 2, new QTableWidgetItem( QStringLiteral( "׼����..." ) ) ) ;
		}

		// ���ϴ������ύ�����е�������
		global_pc().post( std::bind( task_upload, fi.absoluteFilePath().toLocal8Bit().toStdString(), row ) ) ;
	} ;

	// ���� : �ϴ��ļ���
	auto task_dir = [ this, file_upload ]( auto fi )
	{
		QTemporaryFile tmpfile ;
		if ( !tmpfile.open() )
		{
			return false ;
		}

		vector<pair<string,string>> result ;
		{
			auto filelist = result ;
			dir_scan( fi.absoluteFilePath(), filelist ) ;
			if ( filelist.empty() )
			{
				return false ;
			}

			ParallelMapReduce<pair<string,string>> pmr ;
			for ( auto&& it : filelist )
			{
				pmr.map( global_pc(), [ &it ]( auto result )
				{
					if ( it.first == ".dir" )
					{
						result->set_value( it ) ;
						return ;
					}

					auto hash = hash_md5( it.second.c_str() ) ;
					if ( !hash.empty() )
					{
						it.first = hash ;
						result->set_value( it ) ;
						return ;
					}

					result->set_value( make_pair( string(), string() ) ) ;
				} ) ;
			}
			
			const auto subsize = fi.absoluteFilePath().toLocal8Bit().toStdString().size() ;
			pmr.reduce( [ &, subsize ]( auto&& it )
			{
				if ( !it.first.empty() && !it.second.empty() )
				{
					it.second = it.second.substr( subsize ) ;
					result.emplace_back( it ) ;
				}
			} ) ;
		}
		
		auto tmpname = tmpfile.fileName().toLocal8Bit().toStdString() ;
		{
			auto douh = string( "," ) ;
			auto crlf = string( "\n" ) ;
			for ( auto&& it : result )
			{
				tmpfile.write( it.first.c_str(), it.first.size() ) ;
				tmpfile.write( douh.c_str(), douh.size() ) ;
				tmpfile.write( it.second.c_str(), it.second.size() ) ;
				tmpfile.write( crlf.c_str(), crlf.size() ) ;
			}

			tmpfile.setAutoRemove( true ) ;
			tmpfile.close() ;
		}

		file_meta fm ;
		fm.id = ".dir" ;
		fm.id += "?" ;
		fm.id += QHostInfo::localHostName().toLocal8Bit().toStdString() ;
		fm.id += "?" ;
		fm.id += fi.absoluteFilePath().toLocal8Bit().toStdString() ;
		fm.id += "?" ;
		fm.id += QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ).toLocal8Bit().toStdString() ;
		fm.tag = fi.fileName().toLocal8Bit().toStdString() ;

		bool goon = false ;
		auto cloud = global_cloudnodes() ;
		string dist ;
		for ( auto&& dbnode : cloud )
		{
			dist += dbnode.user ;
			dist += ',' ;
		}

		for ( auto&& dbnode : cloud )
		{
			if ( file_to_service( dbnode.to_meta(), fm, tmpname.c_str(), dist.c_str() ) == 200 )
			{
				goon = true ;
				break ;
			}
		}

		if ( !goon )
		{
			return false ;
		}

		for ( auto&& it : result )
		{
			if ( it.first == ".dir" )
			{
				continue ;
			}
			
			file_upload( QFileInfo( fi.absoluteFilePath() + QString().fromLocal8Bit( it.second.c_str() ) ) ) ;
		}

		return true ;
	} ;

	for ( auto&& it : items )
	{
		QFileInfo fi( it ) ;
		if ( fi.isDir() )
		{
			if ( !task_dir( fi ) )
			{
				QMessageBox::critical(
					this,
					QStringLiteral( "����" ),
					QStringLiteral( "�ϴ��ļ���ʧ��" ),
					QMessageBox::Yes,
					QMessageBox::Yes ) ;
			}
		}

		else
		{
			file_upload( fi ) ;
		}
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

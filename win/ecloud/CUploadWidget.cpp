
#include <QSettings>
#include <QMovie>
#include <QMenu>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QDateTime>
#include <QHostInfo>
#include <QTemporaryFile>
#include <QMessageBox>
#include <QDir>
#include <QBoxLayout>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QHeaderView>
#include "CUploadWidget.h"

CUploadWidget::CUploadWidget(QWidget *parent) :
	QWidget( parent ),
	m_pListDirs( nullptr ),
	m_pTableUpload( nullptr ),
	m_pBtnSync( nullptr ),
	m_pLabelMV( nullptr )
{
	auto pLabel = new QLabel( QStringLiteral( "我的文件夹:" ), this ) ;
	
	auto pMovie = new QMovie( ":/res/working.gif", QByteArray(), this ) ;
	pMovie->start() ;

	m_pLabelMV = new QLabel( this ) ;
	m_pLabelMV->setFixedSize( 48, 48 ) ;
	m_pLabelMV->hide() ;
	m_pLabelMV->setMovie( pMovie ) ;

	m_pListDirs = new QListWidget( this ) ;
	m_pListDirs->setViewMode( QListWidget::IconMode ) ;
	m_pListDirs->setResizeMode( QListWidget::Adjust ) ;
	m_pListDirs->setMovement( QListWidget::Static ) ;
	m_pListDirs->setIconSize( QSize( 64, 64 ) ) ;
	m_pListDirs->setContextMenuPolicy( Qt::CustomContextMenu ) ;
	connect( m_pListDirs, &QListWidget::itemDoubleClicked, []( auto item )
	{
		QDesktopServices::openUrl( QUrl::fromLocalFile( item->data( Qt::UserRole ).toString() ) ) ;
	} ) ;

	connect( m_pListDirs, &QListWidget::customContextMenuRequested, [ this ]( auto pos )
	{
		if ( m_pBtnSync->isEnabled() == false )
		{
			return ;
		}

		auto pItem = m_pListDirs->itemAt( pos ) ;
		if ( pItem != nullptr )
		{
			auto pMenu = new QMenu( this ) ;
			auto pActDel = new QAction( QStringLiteral( "删除" ), this ) ;
			pMenu->addAction( pActDel ) ;
			
			connect( pActDel, &QAction::triggered, [ pItem, this ]()
			{
				m_pListDirs->removeItemWidget( pItem ) ;
				delete pItem ;
			} ) ;

			pMenu->exec( QCursor::pos() ) ;
		}
	} ) ;
	
	m_pTableUpload = new QTableWidget( 0, 3, this ) ;
	m_pTableUpload->verticalHeader()->setVisible( false ) ;
	m_pTableUpload->horizontalHeader()->setStretchLastSection( true ) ;
	m_pTableUpload->setColumnWidth( 0, 300 ) ;
	m_pTableUpload->setColumnWidth( 1, 80 ) ;
    m_pTableUpload->setHorizontalHeaderLabels( QStringList()
		<< QStringLiteral( "文件" )
		<< QStringLiteral( "大小" )
		<< QStringLiteral( "状态" ) ) ;

	m_pBtnSync = new QPushButton( this ) ;
	m_pBtnSync->setFixedSize( 48, 48 ) ;
	m_pBtnSync->setStyleSheet(
		"QPushButton{ border-image: url(:/res/sync1.png); }"
		"QPushButton:hover{ border-image: url(:/res/sync2.png); }"
		"QPushButton:pressed{ border-image: url(:/res/sync2.png); }" ) ;

	connect( m_pBtnSync, &QPushButton::clicked, [ = ]
	{
		if ( m_pListDirs->count() == 0 )
		{
			return ;
		}

		setWorking( true ) ;

		auto fina = plan_make( [ = ]
		{
			emit working( false ) ;
		} ) ;

		vector<QString> dirs ;
		for ( long i = 0 ; i < m_pListDirs->count() ; ++i )
		{
			dirs.emplace_back( m_pListDirs->item( i )->data( Qt::UserRole ).toString() ) ;
		}

		m_pTableUpload->setRowCount( 0 ) ;
		this->addTableItems( dirs, fina ) ;
	} ) ;

	auto pLayout = new QVBoxLayout( this ) ;
	pLayout->addWidget( pLabel ) ;
	pLayout->addWidget( m_pListDirs, 1 ) ;
	pLayout->addWidget( m_pTableUpload, 3 ) ;

	auto pHCenter = new QHBoxLayout( this ) ;
	pHCenter->addStretch() ;
	pHCenter->addWidget( m_pBtnSync ) ;
	pHCenter->addWidget( m_pLabelMV ) ;
	pHCenter->addStretch() ;
	pLayout->addLayout( pHCenter ) ;

	this->resize( 480, 640 ) ;
	this->setAcceptDrops( true ) ;
	this->setWindowTitle( QStringLiteral( "数据同步" ) ) ;

	connect( this, &CUploadWidget::tableItemStatusChanged,
		this, &CUploadWidget::setTableItemStatus, Qt::BlockingQueuedConnection ) ;

	connect( this, &CUploadWidget::working,
		this, &CUploadWidget::setWorking, Qt::BlockingQueuedConnection ) ;

	connect( this, &CUploadWidget::appendItem,
		this, &CUploadWidget::itemAppend, Qt::BlockingQueuedConnection ) ;

	connect( this, &CUploadWidget::dirItemStatusChanged,
		this, &CUploadWidget::SetDirItemStatusChanged, Qt::BlockingQueuedConnection ) ;

	QSettings setting( "secret", "ecloud" ) ;
	setting.beginGroup( "mydirs" ) ;
	for ( auto&& val : setting.allKeys() )
	{
		addDir( val ) ;
	}
}

CUploadWidget::~CUploadWidget()
{
	QSettings setting( "secret", "ecloud" ) ;
	setting.beginGroup( "mydirs" ) ;

	for ( auto&& val : setting.allKeys() )
	{
		setting.remove( val ) ;
	}

	for ( long i = 0 ; i < m_pListDirs->count() ; ++i )
	{
		setting.setValue( m_pListDirs->item( i )->data( Qt::UserRole ).toString(), 0 ) ;
	}
}

void CUploadWidget::setWorking( bool work )
{
	this->setAcceptDrops( !work ) ;
	m_pLabelMV->setHidden( !work ) ;
	m_pBtnSync->setHidden( work ) ;
}

int CUploadWidget::itemAppend( QString tooltip, QString text, qint64 size )
{
	const auto row = m_pTableUpload->rowCount() ;
	m_pTableUpload->setRowCount( row + 1 ) ;

	{
		auto item_file_name = new QTableWidgetItem ;
		item_file_name->setToolTip( tooltip ) ;
		item_file_name->setText( text ) ;
		m_pTableUpload->setItem( row, 0, item_file_name ) ;
	}

	{
		auto bytes = (double)size ;
		bytes /= 1024 * 1024 ;

		char out[ 64 ] = { 0 } ;
		sprintf_s( out, "%.2fM", bytes ) ;

		m_pTableUpload->setItem( row, 1, new QTableWidgetItem( out ) ) ;
	}

	{
		m_pTableUpload->setItem( row, 2, new QTableWidgetItem( QStringLiteral( "准备中..." ) ) ) ;
	}

	return row ;
}

void CUploadWidget::SetDirItemStatusChanged( QString dir, int status )
{
	for ( long i = 0 ; i < m_pListDirs->count() ; ++i )
	{
		auto pItem = m_pListDirs->item( i ) ;
		if ( pItem->data( Qt::UserRole ).toString() == dir )
		{
			QPixmap ico( ":/res/dir_normal.png" ) ;
			switch ( status )
			{
			case 0 :
				ico = QPixmap( ":/res/dir_ok.png" ) ;
				break ;

			case 1 :
				ico = QPixmap( ":/res/dir_error.png" ) ;
				break ;
			}

			pItem->setIcon( ico ) ;
			break ;
		}
	}
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

	for ( auto&& it : items )
	{
		QFileInfo fi( it ) ;
		if ( !fi.isDir() )
		{
			continue ;
		}

		if ( fi.fileName() == "." || fi.fileName() == ".." )
		{
			continue ;
		}

		addDir( fi.absoluteFilePath() ) ;
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

void CUploadWidget::addTableItems( const vector<QString>& items, std::shared_ptr<void> fina )
{
	// 任务 : 上传单个文件
	auto task_upload = [ this, fina ]( auto filename, auto row )
	{
		file_meta fm ;
		if ( meta_from_file( fm, filename.c_str() ) != 0 )
		{
			// 非法文件
			emit tableItemStatusChanged( row, 3 ) ;
			return ;
		}

		// 文件已经在邮箱云中存在
		if ( global_cloudfile_exist( fm.id ) )
		{
			emit tableItemStatusChanged( row, 1 ) ;
			return ;
		}

		// 生成文件云端目的地
		auto cloud = global_cloudnodes() ;
		string dist ;
		for ( auto&& dbnode : cloud )
		{
			dist += dbnode.user ;
			dist += ',' ;
		}

		// 使用任意一个有效节点完成上传操作
		for ( auto&& dbnode : cloud )
		{
			auto node = dbnode.to_meta() ;
				
			// 上传中
			emit tableItemStatusChanged( row, 2 ) ;

			// 小文件直接上传
			if ( fm.bytes <= CHUNK_SIZE )
			{
				// 上传成功
				if ( file_to_service( node, fm, filename.c_str(), dist.c_str() ) == 200 )
				{
					emit tableItemStatusChanged( row, 0 ) ;
					return ;
				}

				continue ;
			}

			ParallelMapReduce<bool> pmr ;

			// 大文件分块上传
			for ( auto&& chunk : meta_split_chunk( fm, CHUNK_SIZE ) )
			{
				// 每个块使用独立任务上传
				pmr.map( global_pc(), [ node, chunk, filename, dist ]( auto result )
				{
					// 块已经存在就不再上传了
					dbmeta_cloudfile dbchunk ;
					dbchunk.from_meta( chunk, node.user, 0 ) ;
					if ( global_cloudchunk_exist( dbchunk ) )
					{
						result->set_value( true ) ;
						return ;
					}

					// 上传该块
					if ( file_to_service( node, chunk, filename.c_str(), dist.c_str() ) == 200 )
					{
						result->set_value( true ) ;
						return ;
					}

					result->set_value( false ) ;
				} ) ;
			}

			// 收集所有块的上传结果
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
				// 上传成功
				emit tableItemStatusChanged( row, 0 ) ;
				return ;
			}
		}

		// 上传失败
		emit tableItemStatusChanged( row, 4 ) ;
	} ;

	// UI : 文件上传
	auto file_upload = [ this, task_upload ]( auto fi )
	{
		auto row = emit appendItem( fi.absoluteFilePath(), fi.fileName(), fi.size() ) ;

		// 将上传任务提交到并行调度器中
		global_pc().post( std::bind( task_upload, fi.absoluteFilePath().toLocal8Bit().toStdString(), row ) ) ;
	} ;

	// 任务 : 上传文件夹
	auto task_dir = [ this, file_upload ]( auto fi )
	{
		emit dirItemStatusChanged( fi.absoluteFilePath(), 2 ) ;

		auto this_result = make_shared<bool>( false ) ;
		auto this_fina = plan_make( [ = ]
		{
			if ( *this_result )
			{
				emit dirItemStatusChanged( fi.absoluteFilePath(), 0 ) ;
			}

			else
			{
				emit dirItemStatusChanged( fi.absoluteFilePath(), 1 ) ;
			}
		} ) ;

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

		*this_result = true ;
		return true ;
	} ;

	for ( auto&& it : items )
	{
		QFileInfo fi( it ) ;
		if ( fi.isDir() )
		{
			global_pc().post( std::bind( task_dir, fi ) ) ;
		}
	}
}

bool CUploadWidget::addDir( const QString& dir )
{
	QFileInfo fi( dir ) ;
	if ( !fi.isDir() )
	{
		return false ;
	}

	for ( int i = 0 ; i < m_pListDirs->count() ; ++i )
	{
		auto pItem = m_pListDirs->item( i ) ;
		if ( pItem->data( Qt::UserRole ).toString() == fi.absoluteFilePath() )
		{
			return false ;
		}
	}
	
	auto pItem = new QListWidgetItem( m_pListDirs ) ;
	pItem->setData( Qt::UserRole, fi.absoluteFilePath() ) ;
	pItem->setToolTip( fi.absoluteFilePath() ) ;
	pItem->setText( fi.fileName() ) ;
	pItem->setIcon( QPixmap( ":/res/dir_normal.png" ) ) ;
	m_pListDirs->addItem( pItem ) ;
	return true ;
}

void CUploadWidget::setTableItemStatus( int item, int status )
{
	QBrush br ;
	switch ( status )
	{
		case 0 :
			m_pTableUpload->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "完成" ) ) ) ;
			br = QBrush( QColor( 128, 255, 0 ) ) ;
			break ;

		case 1 :
			m_pTableUpload->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "已存在" ) ) ) ;
			br = QBrush( QColor( 128, 255, 0 ) ) ;
			break ;

		case 2 :
			m_pTableUpload->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "发送中..." ) ) ) ;
			br = QBrush( QColor( 230, 230, 0 ) ) ;
			break ;

		case 3 :
			m_pTableUpload->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "非法文件" ) ) ) ;
			br = QBrush( QColor( 255, 90, 90 ) ) ;
			break ;

		case 4 :
			m_pTableUpload->setItem( item, 2, new QTableWidgetItem( QStringLiteral( "失败" ) ) ) ;
			br = QBrush( QColor( 255, 90, 90 ) ) ;
			break ;

		default :
			return ;
	}

	for ( int i = 0 ; i < m_pTableUpload->columnCount() ; ++i )
	{
		m_pTableUpload->item( item, i )->setBackground( br ) ;
	}
}

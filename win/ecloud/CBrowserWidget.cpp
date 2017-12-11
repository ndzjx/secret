#include "CBrowserWidget.h"
#include <QCoreApplication>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "ecloud.h"
#include <QDebug>
#include <QFileDialog>
#include "CHackConsts.h"
#include <QSettings>
#include <QFileInfo>
#include <QMessageBox>
#include "CTreeAnalysis.h"
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QQueue>
#include <QFileIconProvider>
#include <QtWin>
#include <QTemporaryFile>

QPixmap fileIcon(const QString &extension)
{
	QFileIconProvider icon_provider;
	QIcon icon;
	QTemporaryFile tmp_file(QDir::tempPath() + QDir::separator() +
		QCoreApplication::applicationName() + "_XXXXXX." + extension);
	tmp_file.setAutoRemove(false);

	if (tmp_file.open())
	{
		QString file_name = tmp_file.fileName();
		tmp_file.write(QByteArray());
		tmp_file.close();

		auto pix = icon_provider.icon(QFileInfo(file_name)).pixmap( 32, 32 );
		tmp_file.remove();
		return pix ;
	}

	return QPixmap();
}

CBrowserWidget::CBrowserWidget(QWidget *parent) :
    QWidget(parent),
    m_pLstDirs(NULL),
    m_pCurNode(NULL),
    m_pTreeNode(NULL),
    m_oDirs()
{
	connect( this, &CBrowserWidget::sigInitData,
		this, &CBrowserWidget::slotInitData, Qt::BlockingQueuedConnection ) ;
	
	connect( this, &CBrowserWidget::sigUpdated,
		this, &CBrowserWidget::slotUpdated, Qt::BlockingQueuedConnection ) ;
	
	connect( this, &CBrowserWidget::sigDownloadStatusChanged,
		this, &CBrowserWidget::slotDownloadStatusChanged, Qt::BlockingQueuedConnection ) ;

    QHBoxLayout *pHblTop = new QHBoxLayout(this);
    QPushButton *pBtnUpward = new QPushButton(this);
	pBtnUpward->setToolTip( QStringLiteral( "返回" ) ) ;
	pBtnUpward->setText( QStringLiteral( "返回" ) ) ;
    pBtnUpward->setIcon(QIcon(":/res/upward.png"));
    connect (pBtnUpward, &QPushButton::clicked, [this](){
        if (NULL == m_pCurNode)
        {
            clearTree(m_pTreeNode);
            slotInitData(m_oDirs);
            return;
        }
        if (m_pCurNode->parent())
        {
            m_pCurNode = m_pCurNode->parent();
            showDirs();
        }
        else
        {
            clearTree(m_pTreeNode);
            m_pCurNode = NULL;
            slotInitData(m_oDirs);
        }
    });
    QPushButton *pBtnRefresh = new QPushButton(this);
	pBtnRefresh->setToolTip( QStringLiteral( "刷新" ) ) ;
	pBtnRefresh->setText( QStringLiteral( "刷新" ) ) ;
    pBtnRefresh->setIcon(QIcon(":/res/refresh.png"));
    connect(pBtnRefresh, &QPushButton::clicked, [this]() {
        initData();
    });

    pHblTop->addWidget(pBtnUpward);
    pHblTop->addWidget(pBtnRefresh);
    pHblTop->addStretch();
    pHblTop->setContentsMargins(0,0,0,0);
    QWidget *pWidgetTop = new QWidget(this);
    pWidgetTop->setLayout(pHblTop);

    m_pLstDirs = new QListWidget( this ) ;
    m_pLstDirs->setViewMode( QListWidget::IconMode ) ;
    m_pLstDirs->setResizeMode( QListWidget::Adjust ) ;
    m_pLstDirs->setMovement( QListWidget::Static ) ;
    m_pLstDirs->setIconSize( QSize( 64, 64 ) ) ;
    m_pLstDirs->setGridSize(QSize(64, 64));
    m_pLstDirs->setContextMenuPolicy( Qt::CustomContextMenu ) ;
    connect( m_pLstDirs, &QListWidget::itemDoubleClicked, [this]( auto item )
    {
        QString sValue = item->data( Qt::UserRole ).toString();
		QString sFileName = m_pCurNode ? m_pCurNode->fileName() : "";
		QString sT = item->text();
        // 向上的按钮会涉及delete tree setnull
        if ((NULL == m_pCurNode) && !sValue.isEmpty()) //||
                //((NULL != m_pCurNode) && (m_pCurNode->fileName() != item->text())))
        {
            clearTree(m_pTreeNode);
            m_pCurNode = new CTreeAnalysis;
            m_pTreeNode = m_pCurNode;
            m_pCurNode->init(sValue);
            m_pTreeNode->setFileName(item->text());
        }
        else
        {
            QString sText = item->text();
            CTreeAnalysis *pNode(NULL);
            for (int i = 0; i < m_pCurNode->fileList().size(); ++i)
            {
                pNode = m_pCurNode->fileList().at(i);
                if(sText == pNode->fileName())
                {
                    if (CTreeAnalysis::eFile == pNode->fileType())
                    {
                        return;
                    }

                    m_pCurNode = pNode;
                    break;
                }
            }
        }

        showDirs();
    } ) ;
    connect( m_pLstDirs, &QListWidget::customContextMenuRequested, [ this ]( auto pos )
    {
        auto pItem = m_pLstDirs->itemAt( pos ) ;
        if ( pItem != nullptr )
        {
            auto pMenu = new QMenu( this ) ;
            auto pActDown = new QAction( QStringLiteral( "下载" ), this ) ;
            pMenu->addAction( pActDown ) ;

            connect( pActDown, &QAction::triggered, [ pItem, this ]()
            {
                // 下载 包括建文件夹之类的
				QSettings oSetting( QCoreApplication::applicationDirPath() + "/ecloud.ini", QSettings::IniFormat ) ;
				oSetting.beginGroup( "setting" ) ;
                QString sDownloadPath = oSetting.value(c_DownloadPath).toString();
                QFileInfo oFileInfo(sDownloadPath);
                if (!oFileInfo.isDir())
                {
                    QMessageBox::warning(this, QStringLiteral("错误"),
                                         QStringLiteral("目录%1不存在，请在设置界面指定默认的下载目录").arg(sDownloadPath),
                                         QStringLiteral("确定"));
                    return;
                }

                QString sText = pItem->text();

                QFileInfo oInfo(sDownloadPath + "/" + sText);
                if (oInfo.exists())
                {
                    QMessageBox::warning(this, QStringLiteral("错误"),
                                         QStringLiteral("文件(夹)%1已存在").arg(sDownloadPath + "/" + sText),
                                         QStringLiteral("确定"));
                    return;
                }

                if (NULL == m_pCurNode)
                {
                    QString sValue = pItem->data( Qt::UserRole ).toString();
                    // 向上的按钮会涉及delete tree setnull
                    if (!sValue.isEmpty())
                    {
                        m_pCurNode = new CTreeAnalysis;
                        clearTree(m_pTreeNode);
                        m_pTreeNode = m_pCurNode;
                        m_pCurNode->init(sValue);
                        m_pTreeNode->setFileName(pItem->text());

                        buildPath(m_pCurNode, sDownloadPath + "/" + sText);
                    }
					clearTree(m_pTreeNode);
					m_pCurNode = NULL;
					m_pTreeNode = NULL;
                }
                else
                {
                    CTreeAnalysis *pNode(NULL);
                    for (int i = 0; i < m_pCurNode->fileList().size(); ++i)
                    {
                        pNode = m_pCurNode->fileList().at(i);
                        if(sText == pNode->fileName())
                        {
                            break;
                        }
                    }
                    if (CTreeAnalysis::eFile == pNode->fileType())
                    {
                        // 下载文件
                        string id = pNode->fileMD5().toLocal8Bit().data();
                        string file = (sDownloadPath + "/" + pNode->fileName()).toLocal8Bit().data();
						const int row = appendDownload( QString().fromLocal8Bit( id.c_str() ), QString().fromLocal8Bit( file.c_str() ) ) ;
						global_pc().post( [ id, file, this, row ]
						{
							emit sigDownloadStatusChanged( row, 1 ) ;
							if ( global_cloudfile_download( id.c_str(), file.c_str() ) )
							{
								emit sigDownloadStatusChanged( row, 0 ) ;
								return ;
							}
							emit sigDownloadStatusChanged( row, 2 ) ;
						} ) ;
                    }
                    else
                    {
                        buildPath(pNode, sDownloadPath + "/" + sText);
                    }
                }

            } ) ;

            pMenu->exec( QCursor::pos() ) ;
        }
    } ) ;
	
	m_pTableDownload = new QTableWidget( 0, 2, this ) ;
	m_pTableDownload->verticalHeader()->setVisible( false ) ;
	m_pTableDownload->horizontalHeader()->setStretchLastSection( true ) ;
	m_pTableDownload->setColumnWidth( 0, 350 ) ;
    m_pTableDownload->setHorizontalHeaderLabels( QStringList()
		<< QStringLiteral( "文件" )
		<< QStringLiteral( "状态" ) ) ;

    auto pVblCenter = new QVBoxLayout(this);
    pVblCenter->addWidget(pWidgetTop);
    pVblCenter->addWidget(m_pLstDirs, 2);
	pVblCenter->addWidget(m_pTableDownload, 1);

    initData();

    setLayout(pVblCenter);
    //resize(800, 600);
}

void CBrowserWidget::slotInitData(map< pair<string, string>, string > &data)
{
	m_pLstDirs->setEnabled( true ) ;
	m_pLstDirs->clear();

    QStringList oPathList;
    while (m_pCurNode)
    {
        if (!m_pCurNode->fileName().isEmpty())
        {
            oPathList << m_pCurNode->fileName();
        }
        m_pCurNode = m_pCurNode->parent();
    }

    m_pCurNode = NULL;
    clearTree(m_pTreeNode);
	m_oDirs = std::move(data);

    // 添加根数据
    for (auto it = m_oDirs.begin(); it != m_oDirs.end(); ++it)
    {
        auto pItem = new QListWidgetItem( m_pLstDirs ) ;
        pItem->setData( Qt::UserRole, QString::fromLocal8Bit(it->second.c_str())) ;
        pItem->setToolTip( QString::fromLocal8Bit((it->first.first+ ":" +it->first.second).c_str()) ) ;
        pItem->setText( QFileInfo(QString::fromLocal8Bit(it->first.second.c_str())).fileName() ) ;
        pItem->setIcon( QPixmap( ":/res/dir_normal.png" ).scaled( 32, 32 ) ) ;
        m_pLstDirs->addItem( pItem ) ;
    }

    // 定位
    if (NULL == m_pCurNode && oPathList.size() > 0)
    {
        QString sRoot = oPathList.at(oPathList.size() - 1);
        for (int i = 0; i < m_pLstDirs->count(); ++i)
        {
            QListWidgetItem *pItem = m_pLstDirs->item(i);
			QString sText = pItem->text();
            if (sText == sRoot)
            {
                m_pCurNode = new CTreeAnalysis;
                m_pCurNode->init(pItem->data( Qt::UserRole ).toString());
                m_pTreeNode = m_pCurNode;
                m_pTreeNode->setFileName(pItem->text());
                break;
            }
        }
        for (int i = oPathList.size() - 2; i >= 0; --i)
        {
            bool bFind(false);
            for (int j = 0; j < m_pCurNode->fileList().size(); ++j)
            {
                if (oPathList.at(i) == m_pCurNode->fileList().at(j)->fileName())
                {
                    m_pCurNode = m_pCurNode->fileList().at(j);
                    bFind = true;
                    break;
                }
            }
            if (!bFind)
            {
                m_pCurNode = NULL;
                clearTree(m_pTreeNode);
                break;
            }
        }
        if (m_pCurNode)
        {
            showDirs();
        }
    }
}

void CBrowserWidget::slotUpdated()
{
	initData() ;
}

void CBrowserWidget::slotDownloadStatusChanged( int item, int status )
{
	QBrush br ;
	switch ( status )
	{
		case 0 :
			m_pTableDownload->setItem( item, 1, new QTableWidgetItem( QStringLiteral( "完成" ) ) ) ;
			br = QBrush( QColor( 128, 255, 0 ) ) ;
			break ;

		case 1 :
			m_pTableDownload->setItem( item, 1, new QTableWidgetItem( QStringLiteral( "下载中..." ) ) ) ;
			br = QBrush( QColor( 230, 230, 0 ) ) ;
			break ;

		case 2 :
			m_pTableDownload->setItem( item, 1, new QTableWidgetItem( QStringLiteral( "失败" ) ) ) ;
			br = QBrush( QColor( 255, 90, 90 ) ) ;
			break ;

		default :
			return ;
	}

	for ( int i = 0 ; i < m_pTableDownload->columnCount() ; ++i )
	{
		m_pTableDownload->item( item, i )->setBackground( br ) ;
	}
}

void CBrowserWidget::initData()
{
	if ( !m_pLstDirs->isEnabled() )
	{
		return ;
	}

	m_pLstDirs->clear();
	m_pLstDirs->setDisabled( true ) ;

	global_pc().post( [=]
	{
		emit sigInitData(global_clouddirs());
	} ) ;
}

void CBrowserWidget::showDirs()
{
    m_pLstDirs->clear();
    CTreeAnalysis *pNode(NULL);

    for (int i = 0; i < m_pCurNode->fileList().size(); ++i)
    {
        pNode = m_pCurNode->fileList().at(i);

        auto pItem = new QListWidgetItem( m_pLstDirs ) ;
        pItem->setText( pNode->fileName()) ;
        pItem->setToolTip(pNode->fileName());

        if (CTreeAnalysis::eDir == pNode->fileType())
        {
            pItem->setIcon( QPixmap( ":/res/dir_normal.png" ).scaled( 32, 32 ) ) ;
        }
        else
        {
			pItem->setIcon( fileIcon( QFileInfo( pNode->fileName() ).completeSuffix() ) ) ;
        }
        m_pLstDirs->addItem( pItem ) ;
    }
}
void CBrowserWidget::buildPath(CTreeAnalysis *node, const QString &dir)
{
    // 新建sText目录
    QDir().mkpath(dir);
    // 建立m_pCurNode结构 ，遇到目录建目录，遇到文件下文件

    QQueue<CTreeAnalysis*> oQueue;
    node->setFilePath(dir);
    oQueue.enqueue(node);

    CTreeAnalysis *pNode(NULL);
    while (!oQueue.empty())
    {
        pNode = oQueue.dequeue();
        CTreeAnalysis *pNodeTmp(NULL);
        for (int i = 0; i < pNode->fileList().size(); ++i)
        {
            pNodeTmp = pNode->fileList().at(i);
            if (CTreeAnalysis::eFile == pNodeTmp->fileType())
            {
				string id = pNodeTmp->fileMD5().toLocal8Bit().data();
				string file = (pNode->filePath() + "/" + pNodeTmp->fileName()).toLocal8Bit().data();
				const int row = appendDownload( QString().fromLocal8Bit( id.c_str() ), QString().fromLocal8Bit( file.c_str() ) ) ;
				global_pc().post( [ this, id, file, row ]
				{
					emit sigDownloadStatusChanged( row, 1 ) ;
					if ( global_cloudfile_download( id.c_str(), file.c_str() ) )
					{
						emit sigDownloadStatusChanged( row, 0 ) ;
						return ;
					}
					emit sigDownloadStatusChanged( row, 2 ) ;
				} ) ;
            }
            else if (CTreeAnalysis::eDir == pNodeTmp->fileType())
            {
                QString sPath = pNode->filePath() + "/" + pNodeTmp->fileName();
                pNodeTmp->setFilePath(sPath);
                QDir().mkpath(sPath);
                oQueue.enqueue(pNodeTmp);
            }
        }
    }
}

void CBrowserWidget::clearTree(CTreeAnalysis *node)
{
    if (NULL != node)
    {
        for (int i = 0; i < node->fileList().size(); ++i)
        {
            clearTree(node->fileList().at(i));
        }
        if (m_pTreeNode == node)
        {
            m_pTreeNode = NULL;
        }
        delete node;
    }
}

void CBrowserWidget::update()
{
	emit sigUpdated() ;
}

int CBrowserWidget::appendDownload( QString tooltip, QString text )
{
	const auto row = m_pTableDownload->rowCount() ;
	m_pTableDownload->setRowCount( row + 1 ) ;

	{
		auto item_file_name = new QTableWidgetItem ;
		item_file_name->setToolTip( tooltip ) ;
		item_file_name->setText( text ) ;
		m_pTableDownload->setItem( row, 0, item_file_name ) ;
	}

	{
		m_pTableDownload->setItem( row, 1, new QTableWidgetItem( QStringLiteral( "准备中..." ) ) ) ;
	}

	return row ;
}

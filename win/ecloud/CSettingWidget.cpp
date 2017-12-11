#include "CSettingWidget.h"
#include <QCoreApplication>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include "CAddEmailDialog.h"
#include "ecloud.h"
#include "CHackConsts.h"

CSettingWidget::CSettingWidget(QWidget *parent) :
    QWidget(parent),
    m_pTblWidgetEmail(NULL)
{
    //auto pLblEmail = new QLabel(QString::fromLocal8Bit("邮件列表:"), this);

	auto pTool = new QWidget( this ) ;
	auto pToolLayout = new QHBoxLayout( pTool ) ;
	pToolLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	auto pBtnAdd = new QPushButton(pTool);
	pBtnAdd->setIcon( QPixmap( ":/res/add.png" ) ) ;
	pBtnAdd->setToolTip( QStringLiteral( "添加节点" ) ) ;
	pBtnAdd->setText( QStringLiteral( "添加节点" ) ) ;
	//pBtnAdd->setFixedSize( QSize( 32, 32 ) ) ;
    auto pBtnDel = new QPushButton(pTool);
	pBtnDel->setIcon( QPixmap( ":/res/minus.png" ) ) ;
	pBtnDel->setToolTip( QStringLiteral( "删除节点" ) ) ;
	pBtnDel->setText( QStringLiteral( "删除节点" ) ) ;
	//pBtnDel->setFixedSize( QSize( 32, 32 ) ) ;
	pToolLayout->addWidget( pBtnAdd ) ;
	pToolLayout->addWidget( pBtnDel ) ;
	pToolLayout->addStretch() ;

    m_pTblWidgetEmail = new QTableWidget(0, 3, this);
    //m_pTblWidgetEmail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pTblWidgetEmail->setEditTriggers(QTableWidget::NoEditTriggers);
    m_pTblWidgetEmail->setSelectionBehavior(QTableWidget::SelectRows);
    m_pTblWidgetEmail->setSelectionMode(QTableWidget::SingleSelection);
	m_pTblWidgetEmail->verticalHeader()->setVisible( false ) ;
	m_pTblWidgetEmail->horizontalHeader()->setStretchLastSection( true ) ;
	m_pTblWidgetEmail->setColumnWidth( 0, 270 ) ;
	m_pTblWidgetEmail->setColumnWidth( 1, 80 ) ;
	m_pTblWidgetEmail->setColumnWidth( 2, 80 ) ;
    m_pTblWidgetEmail->setHorizontalHeaderLabels( QStringList()
        << QStringLiteral( "节点" )
        << QStringLiteral( "发送节点" )
        << QStringLiteral( "接收节点") );
    m_pTblWidgetEmail->setCurrentCell(-1, -1);

    auto pVblLeft = new QVBoxLayout(this);
    pVblLeft->addWidget(pTool);
    pVblLeft->addWidget(m_pTblWidgetEmail);
    auto pTopWidget = new QWidget(this);
    pTopWidget->setLayout(pVblLeft);

    auto pBtnDownPath = new QPushButton(QStringLiteral("设置同步目录"));
	pBtnDownPath->setIcon( QPixmap( ":/res/save.png" ) ) ;

    auto pHblMid = new QHBoxLayout(this);
    auto pLedtAddr = new QLineEdit(this);
    pLedtAddr->setEnabled(false);

	QSettings setting( QCoreApplication::applicationDirPath() + "/ecloud.ini", QSettings::IniFormat ) ;
	setting.beginGroup( "setting" ) ;
	auto strDownloadPath = setting.value(c_DownloadPath).toString() ;
	if ( strDownloadPath.isEmpty() )
	{
		strDownloadPath = QCoreApplication::applicationDirPath() + "/cloudfiles" ;
	}

	setting.setValue(c_DownloadPath, strDownloadPath);
	QDir().mkdir( strDownloadPath ) ;
	pLedtAddr->setText( strDownloadPath ) ;

	pHblMid->addWidget(pLedtAddr);
	pHblMid->addWidget(pBtnDownPath);
    auto pWidgetMid = new QWidget(this);
    pWidgetMid->setLayout(pHblMid);

    auto pHblBtm = new QHBoxLayout(this);
    pHblBtm->addStretch();
    //pHblBtm->addWidget(pBtnDownPath);
    //pHblBtm->addWidget(pBtnAdd);
    //pHblBtm->addWidget(pBtnDel);
    pHblBtm->setContentsMargins(0,0,0,0);
    auto pWidgetBtm = new QWidget(this);
    pWidgetBtm->setLayout(pHblBtm);

    auto pVblBottom = new QVBoxLayout(this);
    pVblBottom->addWidget(pWidgetMid);
    pVblBottom->addWidget(pWidgetBtm);
    pVblBottom->setContentsMargins(0,0,0,0);
    auto pBottomWidget = new QWidget(this);
    pBottomWidget->setLayout(pVblBottom);

    auto pVblCenter = new QVBoxLayout(this);
    pVblCenter->addWidget(pTopWidget);
    pVblCenter->addWidget(pBottomWidget);
    pVblCenter->setStretch(0, 2);
    pVblCenter->setContentsMargins(0,0,0,0);

    connect(pBtnDownPath, &QPushButton::clicked, [this, pLedtAddr]() {
        QString sPath = QFileDialog::getExistingDirectory(this);
        if (!sPath.isEmpty())
        {
			QSettings setting( QCoreApplication::applicationDirPath() + "/ecloud.ini", QSettings::IniFormat ) ;
			setting.beginGroup( "setting" ) ;
			setting.setValue(c_DownloadPath, sPath);
            pLedtAddr->setText(sPath);
        }
    });

    connect(pBtnAdd, &QPushButton::clicked, [this](){
        CAddEmailDialog *pInputDialog = new CAddEmailDialog(NULL);
        if (pInputDialog->exec())
        {
            addEmailtoWidget(pInputDialog->acnt(), pInputDialog->pwd(), pInputDialog->smtp(), pInputDialog->pop3());

            service_meta model;
            model.user = pInputDialog->acnt().toLocal8Bit().data();
            model.pawd = pInputDialog->pwd().toLocal8Bit().data();
            model.smtp = pInputDialog->smtp().toLocal8Bit().data();
            model.pop3 = pInputDialog->pop3().toLocal8Bit().data();

            dbmeta_cloudnode cloudnode;
            cloudnode.from_meta(model);
            global_cloudnode_add(cloudnode);
        }
    });

    connect(pBtnDel, &QPushButton::clicked, [this](){
        if (m_pTblWidgetEmail->currentIndex().isValid())
        {
            auto row = m_pTblWidgetEmail->currentRow();
            service_meta model;

            model.user = m_pTblWidgetEmail->item(row, 0)->text().toLocal8Bit().data();
            //model.pawd = m_pTblWidgetEmail->item(row, 1)->text().toLocal8Bit().data();
            model.smtp = m_pTblWidgetEmail->item(row, 2)->text().toLocal8Bit().data();
            model.pop3 = m_pTblWidgetEmail->item(row, 3)->text().toLocal8Bit().data();

            dbmeta_cloudnode cloudnode;
            cloudnode.from_meta(model);
            global_cloudnode_del(cloudnode);

            m_pTblWidgetEmail->removeRow(m_pTblWidgetEmail->currentRow());
        }
    });

    // 添加数据
    vector<dbmeta_cloudnode> vecEmails;
    vecEmails = global_cloudnodes_all();
    for (int i = 0; i < vecEmails.size(); ++i)
    {
        service_meta meta = vecEmails[i].to_meta();
        addEmailtoWidget(QString::fromLocal8Bit(meta.user.c_str()),
                         QString::fromLocal8Bit(meta.pawd.c_str()),
                         QString::fromLocal8Bit(meta.smtp.c_str()),
                         QString::fromLocal8Bit(meta.pop3.c_str()) );
    }
    setLayout(pVblCenter);
    //resize(800, 600);
}

void CSettingWidget::addEmailtoWidget(const QString &acnt, const QString &pwd, const QString &smtp, const QString &pop3)
{
    auto row = m_pTblWidgetEmail->rowCount() ;
    m_pTblWidgetEmail->setRowCount( row + 1 ) ;
    m_pTblWidgetEmail->setItem(row, 0, new QTableWidgetItem(acnt));
    //m_pTblWidgetEmail->setItem(row, 1, new QTableWidgetItem(pwd));
	pwd;

	if ( smtp.size() < 4 )
	{
		m_pTblWidgetEmail->setItem(row, 1, new QTableWidgetItem(""));
	}

	else
	{
		auto pItem = new QTableWidgetItem("ready");
		pItem->setTextAlignment( Qt::AlignCenter );
		m_pTblWidgetEmail->setItem(row, 1, pItem);
	}

	if ( pop3.size() < 4 )
	{
		m_pTblWidgetEmail->setItem(row, 2, new QTableWidgetItem(""));
	}

	else
	{
		auto pItem = new QTableWidgetItem("ready");
		pItem->setTextAlignment( Qt::AlignCenter );
		m_pTblWidgetEmail->setItem(row, 2, pItem);
	}
}

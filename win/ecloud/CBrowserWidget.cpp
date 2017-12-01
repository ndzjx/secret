#include "CBrowserWidget.h"
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHeaderView>
#include "ecloud.h"
#include <QDebug>
#include <QFileDialog>

CBrowserWidget::CBrowserWidget(QWidget *parent) : QWidget(parent)
{
    auto pLblEmail = new QLabel(QString::fromLocal8Bit("文件列表:"), this);
    m_pTblWidgetFile = new QTableWidget(0, 3, this);
    m_pTblWidgetFile->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pTblWidgetFile->setEditTriggers(QTableWidget::NoEditTriggers);
    m_pTblWidgetFile->setSelectionBehavior(QTableWidget::SelectRows);
    //m_pTblWidgetEmail->setSelectionMode(QTableWidget::SingleSelection);
    m_pTblWidgetFile->setHorizontalHeaderLabels( QStringList()
        << QStringLiteral( "文件名" )
        << QStringLiteral( "文件大小" )
        << QStringLiteral("文件位置"));
    m_pTblWidgetFile->setCurrentCell(-1, -1);

    auto pBtnDownLoad = new QPushButton(QStringLiteral("下载"), this);
    QHBoxLayout *pHblBottom = new QHBoxLayout(this);
    pHblBottom->addStretch();
    pHblBottom->addWidget(pBtnDownLoad);
    auto pBottomWidget = new QWidget(this);
    pBottomWidget->setLayout(pHblBottom);
    connect(pBtnDownLoad, &QPushButton::clicked, [this](){

        QList<QTableWidgetItem*>items = m_pTblWidgetFile->selectedItems();
        int nCount = items.count();
        if (0 == nCount)
        {
            return;
        }
        if (nCount <= m_pTblWidgetFile->columnCount())
        {
            QString sFileName = QFileDialog::getSaveFileName(this);
            if (!sFileName.isEmpty())
            {
                global_cloudfile_download(items.at(0)->data(
                                          Qt::UserRole).toString().toLocal8Bit().data(),
                                          sFileName.toLocal8Bit().data());
            }
        }
        else
        {
            QString sDir = QFileDialog::getExistingDirectory();

            for(int i = 0;i < nCount; i += m_pTblWidgetFile->columnCount())
            {
                QTableWidgetItem *item = items.at(i);
                //int row = m_pTblWidgetFile->row(item);//获取选中的行
                QString sFileName = sDir + "/" + item->text();//获取内容
                global_cloudfile_download(item->data(Qt::UserRole).toString().toLocal8Bit().data(),
                                          sFileName.toLocal8Bit().data());

            }
        }

    });

    auto pVblCenter = new QVBoxLayout(this);
    pVblCenter->addWidget(pLblEmail);
    pVblCenter->addWidget(m_pTblWidgetFile);
    pVblCenter->addWidget(pBottomWidget);

    // 添加数据
    vector<dbmeta_cloudfile> vecEmails;
    vecEmails = global_cloudfiles();
    for (int i = 0; i < vecEmails.size(); ++i)
    {
        auto row = m_pTblWidgetFile->rowCount() ;
        m_pTblWidgetFile->setRowCount( row + 1 ) ;
        auto pTblItem = new QTableWidgetItem(QString::fromLocal8Bit(vecEmails[i].tag.c_str()));
        pTblItem->setData(Qt::UserRole, vecEmails[i].id.c_str());
        m_pTblWidgetFile->setItem(row, 0, pTblItem);
        m_pTblWidgetFile->setItem(row, 1, new QTableWidgetItem(QString::fromLocal8Bit(vecEmails[i].bytes.c_str())));
        m_pTblWidgetFile->setItem(row, 2, new QTableWidgetItem(QString::fromLocal8Bit(vecEmails[i].service.c_str())));
    }
    setLayout(pVblCenter);
    resize(800, 600);
}


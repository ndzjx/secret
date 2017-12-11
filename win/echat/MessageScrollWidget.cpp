#include "MessageScrollWidget.h"
#include <QLayout>
#include <QLabel>
#include <QAbstractSlider>

MessageScrollWidget::MessageScrollWidget(QWidget *parent)
    : QWidget(parent),
      m_pmwMessage(),
      m_psaMessage()
{
	connect( this, &MessageScrollWidget::sigAddImageMsg,
		this, &MessageScrollWidget::slotAddImageMsg, Qt::BlockingQueuedConnection ) ;
	
	connect( this, &MessageScrollWidget::sigAddTextMsg,
		this, &MessageScrollWidget::slotAddTextMsg, Qt::BlockingQueuedConnection ) ;
	
	connect( this, &MessageScrollWidget::sigAddVoiceMsg,
		this, &MessageScrollWidget::slotAddVoiceMsg, Qt::BlockingQueuedConnection ) ;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    m_pmwMessage = new MessageWidget(this);
    m_psaMessage = new QScrollArea(this);

    //滚动条自动滚动到底部
    connect( m_psaMessage->verticalScrollBar(), &QAbstractSlider::rangeChanged,
        this, &MessageScrollWidget::onAutoScroll ) ;

    initUI();
}

void MessageScrollWidget::slotAddImageMsg(QPixmap icon, QString nickName, QPixmap msg)
{
    m_pmwMessage->addImageMsg(icon, nickName, msg);            
}

void MessageScrollWidget::slotAddTextMsg(QPixmap icon, QString nickName, QString msg)
{
    m_pmwMessage->addTextMsg(icon, nickName, msg);
}

void MessageScrollWidget::slotAddVoiceMsg(QPixmap icon, QString nickName, QString msg)
{
    m_pmwMessage->addVoiceMsg( icon, nickName, msg);
}

void MessageScrollWidget::addImageMsg(QPixmap icon, QString nickName, QPixmap msg)
{
    emit sigAddImageMsg( icon, nickName, msg ) ;
}

void MessageScrollWidget::addTextMsg(QPixmap icon, QString nickName, QString msg)
{
    emit sigAddTextMsg( icon, nickName, msg ) ;
}

void MessageScrollWidget::addVoiceMsg(QPixmap icon, QString nickName, QString msg)
{
    emit sigAddVoiceMsg( icon, nickName, msg);
}

void MessageScrollWidget::onAutoScroll(int min, int max)
{
    m_psaMessage->verticalScrollBar()->setSliderPosition(max);
//    m_psaMessage->verticalScrollBar()->setValue(max);
}

void MessageScrollWidget::initUI()
{
    //给MessageWidget添加滚动条
    m_psaMessage->setWidget(m_pmwMessage);
    m_psaMessage->setWidgetResizable(true);
//    m_psaMessage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    m_psaMessage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    layout()->addWidget(m_psaMessage);
    //this->setFixedSize(500, 500);
}

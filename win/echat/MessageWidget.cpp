#include "MessageWidget.h"
#include "MessageConsts.h"
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include "mod_media.h"
#include "echat.h"

MessageWidget::MessageWidget(QWidget* parent)
    : QWidget(parent),
      m_pmsgLayout(nullptr),
      m_oMessageList(),
      m_pWidgetList()
{    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_pmsgLayout = new QVBoxLayout(this);
    QVBoxLayout *stretchLayout = new QVBoxLayout(this);
    stretchLayout->addStretch();
    mainLayout->addLayout(m_pmsgLayout);
    mainLayout->addLayout(stretchLayout);
    setLayout(mainLayout);
}

MessageWidget::~MessageWidget()
{
    clear();
}

void MessageWidget::addImageMsg(QPixmap icon, QString nickName, QPixmap msg)
{
    Message imageMsg = {icon, nickName, "", msg};

    m_oMessageList.push_back(imageMsg);
    addMsgWidget(imageMsg);
//    refreshWidgetList();
//    paintWidgetList();
}

void MessageWidget::addTextMsg(QPixmap icon, QString nickName, QString msg)
{
	Message textMsg = { icon, nickName, msg, QPixmap() };
    m_oMessageList.push_back(textMsg);
    addMsgWidget(textMsg);
//    refreshWidgetList();
    //    paintWidgetList();
}

void MessageWidget::addVoiceMsg(QPixmap icon, QString nickName, QString msg)
{
    Message voiceMsg = {icon, nickName, msg, QPixmap(":/res/btn-voice.png")};
    m_oMessageList.push_back(voiceMsg);
    addMsgWidget(voiceMsg);
}

void MessageWidget::addMsgWidget(Message msg)
{   
    m_pmsgLayout->addWidget(createMsgWidget(msg));
}

void MessageWidget::paintWidgetList()
{    
    for (auto it = m_pWidgetList.begin(); it != m_pWidgetList.end(); ++it)
    {
        m_pmsgLayout->addWidget(*it);
    }   
}

void MessageWidget::refreshWidgetList()
{
    ClearWidgetList();
    for (auto it = m_oMessageList.begin(); it != m_oMessageList.end(); ++it)
    {
        m_pWidgetList.push_back(createMsgWidget(*it));
    }
}

QWidget *MessageWidget::createMsgWidget(Message omsg)
{  
    auto pMain = new QWidget(this);

    auto pIcon = new QLabel(pMain);
    pIcon->setPixmap(omsg.icon.scaled(50, 50, Qt::KeepAspectRatioByExpanding));
    pIcon->setFixedSize(50, 50);

    auto pNickName = new QLabel(omsg.nickName, pMain);
    pNickName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::lightGray);
    pNickName->setPalette(pa);

//    auto pLine = new QLabel(pMain);
//    pLine->setStyleSheet("background-color:black");
//    pLine->setFixedHeight(2);
    auto pUpLayout = new QHBoxLayout;
    pUpLayout->addWidget(pIcon);
    pUpLayout->addWidget(pNickName);

    auto pMainLayout = new QVBoxLayout(pMain);
    pMainLayout->addLayout(pUpLayout);
//    pMainLayout->addWidget(pLine);

    auto pMsg = new QLabel(pMain);    
    auto pPushBtn = new QPushButton(pMain);
    pPushBtn->setVisible(false);

    //消息体
    if (!omsg.textMsg.isEmpty() && !omsg.imageMsg.isNull())
    {                
        pPushBtn->setFixedSize(118, 59);
        pPushBtn->setObjectName(omsg.textMsg);
        pPushBtn->setStyleSheet(
                "QPushButton{ border-image: url(:/res/btn-voice.png); }"
                "QPushButton:hover{ border-image: url(:/res/btn-voiceclick.png); }"
                "QPushButton:pressed{ border-image: url(:/res/btn-voiceclick.png); }" ) ;

        connect(  pPushBtn, &QPushButton::clicked,
                  [ pPushBtn ]
        {
            auto file = pPushBtn->objectName().toLocal8Bit().toStdString();
            global_pc().post( [ file ]
            {
                media_play( file.c_str() ) ;
            } ) ;
        } ) ;

        pMainLayout->addWidget(pPushBtn);
        pPushBtn->setVisible(true);
    }
    else if (!omsg.textMsg.isEmpty())
    {
        pMsg->setText(omsg.textMsg);

        QFont ft;
        ft.setPointSize(12);
        ft.setBold(true);
        pMsg->setFont(ft);
        pMainLayout->addWidget(pMsg);
    }
    else if (!omsg.imageMsg.isNull())
    {
        QPixmap image = omsg.imageMsg.scaled(200, 200, Qt::KeepAspectRatioByExpanding);
        pMsg->setPixmap(omsg.imageMsg);
        pMainLayout->addWidget(pMsg);
    }

    pMain->setStyleSheet(".QWidget{background:light green; "
                         "border:2px groove gray;border-radius:10px;padding:2px 4px;}");
    //pMain->setStyleSheet(".QWidget{background:lightgreen; border:2px groove gray;border-radius:10px;padding:2px 4px;}");
    //pMain->setFixedHeight(pMain->height());
    return pMain;

//	auto msg = &omsg;
//    //头像
//    QLabel *lbIcon = new QLabel();
//    QPixmap pmImage = msg->icon.scaled(50, 50, Qt::KeepAspectRatioByExpanding);
//    lbIcon->setPixmap(msg->icon);
//    lbIcon->setFixedSize(50, 50);

//    //昵称
//    QLabel *lbNickName = new QLabel(msg->nickName);
//    lbNickName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

//    //消息标题
//    QHBoxLayout *lyMsgTitle = new QHBoxLayout(this);
//    lyMsgTitle->addWidget(lbIcon);
//    lyMsgTitle->addWidget(lbNickName);
//    lyMsgTitle->setContentsMargins(0, 0, 0, 0);

//    //分割线，就lable来假装一个
//    QLabel *lbLine = new QLabel(this);
//    lbLine->setStyleSheet("background-color:black");
//    lbLine->setFixedHeight(2);
//    QVBoxLayout *lyLine = new QVBoxLayout(this);
//    lyLine->addLayout(lyMsgTitle);
//    lyLine->addWidget(lbLine);
//    lyLine->setContentsMargins(0, 0, 0, 0);

//    QLabel *lbMsg = new QLabel(this);
//    //消息体
//    if (!msg->textMsg.isEmpty())
//    {
//        lbMsg->setText(msg->textMsg);
//    }
//    else if (!msg->imageMsg.isNull())
//    {
//        QPixmap image = msg->imageMsg.scaled(200, 200, Qt::KeepAspectRatioByExpanding);
//        lbMsg->setPixmap(msg->imageMsg);
//    }
//    //语音
//    // else if (!msg->)

//    lbMsg->setContentsMargins(0, 0, 0, 0);
//    lbMsg->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    QHBoxLayout *lyBody = new QHBoxLayout(this);
//    lyBody->addWidget(lbMsg);
//    lyBody->setContentsMargins(0, 0, 0, 0);
//    lyBody->setAlignment(Qt::AlignLeft | Qt::AlignTop);
//    QVBoxLayout *lyMain = new QVBoxLayout(this);
//    lyMain->addLayout(lyLine);
//    lyMain->addWidget(lbMsg);

//    QWidget *wdtMain = new QWidget(this);
//    wdtMain->setLayout(lyMain);
//    return wdtMain;
}

void MessageWidget::clear()
{
    ClearWidgetList();
    m_oMessageList.clear();
}

void MessageWidget::ClearWidgetList()
{
    for (auto it = m_pWidgetList.begin(); it != m_pWidgetList.end(); ++it)
    {
        delete *it;
    }
    m_pWidgetList.clear();
}

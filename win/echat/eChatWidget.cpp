#include "eChatWidget.h"
#include <QVBoxLayout>

eChatWidget::eChatWidget(QWidget *parent):
    QWidget(parent),
    m_pscrollWidget(nullptr),
    m_pwidget(nullptr)
{
    m_pscrollWidget = new MessageScrollWidget(this);
    m_pwidget = new Widget(this);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setSpacing( 0 );
	vLayout->setContentsMargins( 0, 0, 0, 0 ) ;
    vLayout->addWidget(m_pscrollWidget,3);
    vLayout->addWidget(m_pwidget,1);
    setLayout(vLayout);
}

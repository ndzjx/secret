#include <QMouseEvent>
#include <QBitmap>
#include <QPainter>
#include "GHYTitleBar.h"

GHYTitleBar::GHYTitleBar(QWidget *parent) :
    QWidget(parent),
    m_pLeftToolLayout(NULL),
    m_pRightToolLayout(NULL),
    m_pIconLabel(NULL),
    m_pMinimizeButton(NULL),
    m_pMaximizeButton(NULL),
    m_pCloseButton(NULL)
{
    if (parent)
    {
        parent->setWindowFlags(Qt::FramelessWindowHint | parent->windowFlags());
        parent->installEventFilter(this);
    }
}

GHYTitleBar::~GHYTitleBar()
{

}

void GHYTitleBar::init(int style)
{
    setFixedHeight(30);
    setMouseTracking(true);

    m_pIconLabel = new QLabel(this);
    m_pIconLabel->setFixedSize(25, 25);
    m_pIconLabel->setScaledContents(true);
    m_pIconLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pMinimizeButton = new QPushButton(this);
    m_pMinimizeButton->hide();
    m_pMinimizeButton->setToolTip(QStringLiteral("最小化"));
    m_pMinimizeButton->setFixedSize(QPixmap(":/res/ico-min.png").size());
    m_pMinimizeButton->setStyleSheet(
                "QPushButton{border-image: url(:/res/ico-min.png);}"
                "QPushButton:hover{border-image: url(:/res/ico-min.png);}"
                "QPushButton:pressed{border-image: url(:/res/ico-min.png);}");

    m_pMaximizeButton = new QPushButton(this);
    m_pMaximizeButton->hide();
    m_pMaximizeButton->setToolTip(QStringLiteral("最大化"));
    m_pMaximizeButton->setFixedSize(QPixmap(":/res/ico-max.png").size());
    m_pMaximizeButton->setStyleSheet(
                "QPushButton{border-image: url(:/res/ico-max.png);}"
                "QPushButton:hover{border-image: url(:/res/ico-max.png);}"
                "QPushButton:pressed{border-image: url(:/res/ico-max.png);}");

    m_pCloseButton = new QPushButton(this);
    m_pCloseButton->hide();
    m_pCloseButton->setToolTip(QStringLiteral("关闭"));
    m_pCloseButton->setFixedSize(QPixmap(":/res/ico-close.png").size());
    m_pCloseButton->setStyleSheet(
                "QPushButton{border-image: url(:/res/ico-close.png);}"
                "QPushButton:hover{border-image: url(:/res/ico-close.png);}"
                "QPushButton:pressed{border-image: url(:/res/ico-close.png);}");

    auto pLeftToolArea = new QFrame(this);
    m_pLeftToolLayout = new QHBoxLayout(pLeftToolArea);
    m_pLeftToolLayout->setSpacing(0);
    m_pLeftToolLayout->setContentsMargins(0, 1, 0, 1);

    auto pRightToolArea = new QFrame(this);
    m_pRightToolLayout = new QHBoxLayout(pRightToolArea);
    m_pRightToolLayout->setSpacing(0);
    m_pRightToolLayout->setContentsMargins(0, 1, 0, 1);

    auto pLayout = new QHBoxLayout(this);
    pLayout->setSpacing(10);
    pLayout->setContentsMargins(5, 0, 5, 0);
    pLayout->addWidget(m_pIconLabel);
    pLayout->addSpacing(20);
    pLayout->addWidget(pLeftToolArea);
    pLayout->addStretch();
    pLayout->addWidget(pRightToolArea);
    pLayout->addSpacing(20);

    if (style & Qt::WindowMinimizeButtonHint)
    {
        m_pMinimizeButton->show();
        pLayout->addWidget(m_pMinimizeButton);
    }

    if (style & Qt::WindowMaximizeButtonHint)
    {
        m_pMaximizeButton->show();
        pLayout->addWidget(m_pMaximizeButton);
    }

    if (style & Qt::WindowCloseButtonHint)
    {
        m_pCloseButton->show();
        pLayout->addWidget(m_pCloseButton);
    }

    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}

void GHYTitleBar::onClicked()
{
    if (window()->isTopLevel())
    {
        auto pButton = qobject_cast<QPushButton*>(sender());
        if (pButton == m_pMinimizeButton)
        {
            window()->showMinimized();
        }
        else if (pButton == m_pMaximizeButton)
        {
            window()->isMaximized() ? window()->showNormal() : window()->showMaximized();
        }
        else if (pButton == m_pCloseButton)
        {
            window()->close();
        }
    }
}

void GHYTitleBar::paintEvent(QPaintEvent*)
{
    // Draw Border.
    QPainter painter(this);
    const int lw = 1;
    painter.setPen(QColor("#DEDEDE"));
    painter.drawLine(0, 0, width() - 1, 0);
    painter.drawLine(0, 0, 0, height() - lw);
    painter.drawLine(width() - lw, 0, width() - lw, height() - lw);
    painter.drawLine(0, height() - lw, width() - lw, height() - lw);

    // Draw Title.
    QFont font;
    font.setFamily(QStringLiteral("微软雅黑"));
    font.setPointSize(12);
    painter.setPen(QColor(0, 0, 0));
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, windowTitle());
}

void GHYTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	return ;
    emit m_pMaximizeButton->clicked();
    event->accept();
}

bool GHYTitleBar::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
        case QEvent::WindowTitleChange:
        {
            auto pWidget = qobject_cast<QWidget*>(obj);
            if (pWidget)
            {
                setWindowTitle(pWidget->windowTitle());
                return true;
            }

            break;
        }

        case QEvent::WindowIconChange:
        {
            auto pWidget = qobject_cast<QWidget*>(obj);
            if (pWidget)
            {
                auto icon = pWidget->windowIcon();
                m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()));
                return true;
            }

            break;
        }

        case QEvent::WindowStateChange:
        case QEvent::Resize:
        {
            updateMaximize();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void GHYTitleBar::updateMaximize()
{
    if (window()->isTopLevel())
    {
       QBitmap bmp(window()->size());
       bmp.fill();
       QPainter p(&bmp);
       p.setPen(Qt::NoPen);
       p.setBrush(Qt::black);

        if (window()->isMaximized())
        {
            m_pMaximizeButton->setToolTip(QStringLiteral("还原"));
            p.drawRoundedRect(bmp.rect(), 0, 0);
        }
        else
        {
            m_pMaximizeButton->setToolTip(QStringLiteral("最大化"));
            p.drawRoundedRect(bmp.rect(), 3, 3);
        }

        window()->setMask(bmp);
    }
}

#include "myemotionwindow.h"
#include "myemotionwidget.h"
#include <QPropertyAnimation>
#include <QVBoxLayout>

MyEmotionWindow::MyEmotionWindow(QWidget *parent)
	: QWidget(parent)
	, m_smallEmotionWidget(NULL)
	, m_normalEmotionWidget(NULL)
{
	ui.setupUi(this);
	initWindow();
	initEmotion();
	this->setMouseTracking(true);
}

MyEmotionWindow::~MyEmotionWindow()
{}

void MyEmotionWindow::initWindow()
{
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setStyleSheet("QWidget#MyEmotionWindow{background:rgb(220, 240, 160);}");
}

void MyEmotionWindow::initEmotion()
{
	// 初始化小表情框;
    m_smallEmotionWidget = new MyEmotionWidget(this);

	m_smallEmotionWidget->setRowAndColumn(4, 4);
	m_smallEmotionWidget->setEmotionSize(QSize(32, 32));
	m_smallEmotionWidget->setEmotionMovieSize(QSize(24, 24));
	m_smallEmotionWidget->setMaxRow(4);
	m_smallEmotionWidget->initTableWidget();
    QString path = QCoreApplication::applicationDirPath();
    path = path + "/../win/echat/res/Resources/QQexpression/%1.gif";
	for (int i = 0; i < 10; i++)
	{
        QString &name= path.arg(i + 1);
        m_smallEmotionWidget->addEmotionItem(name, "");
	}

	// 初始化正常表情框;
    m_normalEmotionWidget = new MyEmotionWidget(this);
	m_normalEmotionWidget->setRowAndColumn(10, 14);
	m_normalEmotionWidget->setEmotionSize(QSize(32, 32));
	m_normalEmotionWidget->setEmotionMovieSize(QSize(24, 24));
	m_normalEmotionWidget->setMaxRow(6);
	m_normalEmotionWidget->initTableWidget();
	for (int i = 0; i < 132; i++)
	{
		m_normalEmotionWidget->addEmotionItem(path.arg(i + 1), "");
	}

	m_lableTitle = new QLabel;
	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->addWidget(m_lableTitle);
	vLayout->addWidget(m_smallEmotionWidget);
	vLayout->addWidget(m_normalEmotionWidget);
    this->setLayout(vLayout);
}

void MyEmotionWindow::showSmallEmotion(QPoint point)
{
	m_normalEmotionWidget->setVisible(false);
	m_lableTitle->setText("This is Small Emotion Window");
	this->setFixedSize(QSize(m_smallEmotionWidget->width() + 20, m_smallEmotionWidget->height() + 50));
	move(point);
	show();
}

void MyEmotionWindow::showNormalEmotion(QPoint point)
{
	m_smallEmotionWidget->setVisible(false);
	m_lableTitle->setText("This is Normal Emotion Window");
	this->setFixedSize(QSize(m_normalEmotionWidget->width() + 20, m_normalEmotionWidget->height() + 50));
	move(point);
	show();
}

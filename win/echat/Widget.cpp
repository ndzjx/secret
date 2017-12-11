#include <QFileInfo>
#include <QHostInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTemporaryFile>
#include <QDateTime>
#include "echat.h"
#include "Widget.h"
#include <qdialog.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
	m_pAudioForm = new AudioForm( this ) ;
	m_pAudioForm->hide() ;

    dlg = new Emotion(this);
    dlg->hide();

    //m_pemotion = new MyEmotionWindow(this);
    //m_pemotion->hide();
    openemotion = new QAction(QIcon(":/res/em.png"), QStringLiteral("表情"), this);
    connect(openemotion, &QAction::triggered, this, &Widget::showEmotionWin);

    openAction = new QAction(QIcon(":/res/sendimage.png"), QStringLiteral("图片"), this);
    //openAction->setShortcuts(QKeySequence::Open);
    //openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &Widget::openFile);

    saveAction = new QAction(QIcon(":/res/audiomode.png"), QStringLiteral("语音"), this);
    //saveAction->setShortcuts(QKeySequence::Save);
    //saveAction->setStatusTip(tr("Save a new file"));
    connect(saveAction, &QAction::triggered, this, &Widget::switchAudio);

    textEdit = new TextEdit(this);
	//textEdit->setFixedSize( 400, 150 ) ;
	connect(textEdit, &TextEdit::sigEnter, [=]
	{
		saveFile();
	});

	//m_pAudioForm->setFixedSize( 400, 150 ) ;

    m_pBtnSend = new QPushButton(QStringLiteral("发送(Ctrl + Enter)"), this);

    connect(m_pBtnSend, &QPushButton::clicked, this, &Widget::saveFile);

	QWidget *widget1 = new QWidget(this);
	//widget1->setFixedWidth(400);
    QHBoxLayout *hlayout = new QHBoxLayout(widget1);
    hlayout->addStretch();
    hlayout->addWidget(m_pBtnSend);
    widget1->setLayout(hlayout);
    //widget1->setContentsMargins(0, 0, 0, 0);
    hlayout->setContentsMargins(0,0,0,0);
    //widget1->setFixedHeight(button->height());

    QVBoxLayout *vlayout = new QVBoxLayout;

    QToolBar *toolbar = new QToolBar;
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->addAction(openAction);
    toolbar->addAction(saveAction);
    toolbar->addAction(openemotion);

    //widget->addAction(openAction);
    //widget->addAction(saveAction);

    vlayout->addWidget(toolbar);

    vlayout->addWidget(textEdit);
	vlayout->addWidget(m_pAudioForm);
    vlayout->addWidget(widget1);
    //vlayout->setStretch(0, 0.5);


    //vlayout->setStretch(1, 10);
    //vlayout->setStretch(2, 1);



    this->setLayout(vlayout);
	textEdit->setFocus() ;
   // setCentralWidget(widget);
	
	connect( this, &Widget::sigChangeEnable,
        this, &Widget::changeEnable, Qt::BlockingQueuedConnection) ;

}

Widget::~Widget()
{
}

void Widget::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".",
                                                tr("image(*.jpg);;image(*.png);;image(*.bmp)"));

    if(!path.isEmpty())
    {
        /*QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream in(&file);
        textEdit->setText(in.readAll());
        file.close();*/
        QClipboard *clipboard = QApplication::clipboard();
        QImage &image = QImage(path);
        clipboard->setImage(image);
        if(textEdit->canInsertFromMimeData(clipboard->mimeData()))
        {
            textEdit->insertImage(path, clipboard->mimeData());
        }

        bool bisSend = false;
        while(!bisSend)
        {
            bisSend = sendImage(path);
        }
        //东杰那边显示后，再清除
        textEdit->clear();
    }
    else
    {
        //QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void Widget::switchAudio()
{
	if ( m_pAudioForm->isHidden() )
	{
		textEdit->hide() ;
		m_pAudioForm->show() ;
		m_pAudioForm->setFocus() ;
	}

	else
	{
		m_pAudioForm->hide() ;
		textEdit->show() ;
		textEdit->setFocus() ;
	}
}

void Widget::saveFile()
{
    QTemporaryFile tmpfile;
    if ( !tmpfile.open() )
    {
        return;
    }
    //QString path1 = tmpfile.fileName();
    QString path = tmpfile.fileName();//QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Write File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        out << textEdit->toPlainText();
        file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }

    bool bisSend = false;
    while(!bisSend)
    {
        bisSend = sendFile(path);
    }
    textEdit->clear();
    textEdit->setFocus();
    tmpfile.setAutoRemove(false) ;
    tmpfile.close() ;

}

void Widget::changeEnable( bool enable )
{
	m_pBtnSend->setEnabled( enable ) ;
    textEdit->setEnabled( enable ) ;
	textEdit->clear() ;
    textEdit->setFocus() ;
}

void Widget::showEmotionWin()
{
    dlg->show();
}

void Widget::sendEmotion(QString &path)
{
    bool bisSend = false;
    while(!bisSend)
    {
        bisSend = sendImage(path);
    }
}

void Widget::keyPressEvent(QKeyEvent *event)
{
//     if(event->key() == Qt::Key_Control)
//     {
//        saveFile();
// 
//     }
}

bool Widget::sendFile(const QString &path)
{
    changeEnable( false ) ;

	global_pc().post( [=]
	{
		auto file = path.toLocal8Bit().toStdString() ;

		for ( auto&& dbnode : global_cloudnodes_sender() )
		{
			file_meta fm ;
			fm.id = ".chat?text?txt?" ;
			fm.id += QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ).toLocal8Bit().toStdString() ;
			fm.tag = QHostInfo::localHostName().toLocal8Bit().toStdString() ;

			if ( file_to_service(
				dbnode.to_meta(),
				fm,
				file.c_str(),
				global_cloudnodes_dist().c_str() ) == 200 )
			{
				break ;
			}
		}
		
		file_plan_remove( file.c_str() ) ;
		emit sigChangeEnable( true ) ;
	} ) ;

    return true ;
}

bool Widget::sendImage(const QString &path)
{
	changeEnable( false ) ;

	global_pc().post( [=]
	{
		auto file = path.toLocal8Bit().toStdString() ;
		auto ext = QFileInfo( path ).completeSuffix().toLocal8Bit().toStdString() ;

		for ( auto&& dbnode : global_cloudnodes_sender() )
		{
			file_meta fm ;
			fm.id = ".chat?image?" + ext + "?";
			fm.id += QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ).toLocal8Bit().toStdString() ;
			fm.tag = QHostInfo::localHostName().toLocal8Bit().toStdString() ;
			fm.bytes = file_size( file.c_str() ) ;

			if ( file_to_service(
				dbnode.to_meta(),
				fm,
				file.c_str(),
				global_cloudnodes_dist().c_str() ) == 200 )
			{
				break ;
			}
		}
		
		emit sigChangeEnable( true ) ;
	} ) ;

    return true ;
}

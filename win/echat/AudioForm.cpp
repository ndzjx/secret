
#include <QCoreApplication>
#include <QDir>
#include <QTemporaryFile>
#include <QHostInfo>
#include <QDateTime>
#include <QHBoxLayout>
#include "AudioForm.h"
#include "echat.h"
#include "mod_media.h"

AudioForm::AudioForm(QWidget *parent) : QWidget(parent)
{
	m_strEventName = QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ).toLocal8Bit().toStdString() ;
	m_strEventName += "AudioForm" ;
	m_hEvent = CreateEventA( NULL, FALSE, FALSE, m_strEventName.c_str() ) ;

	m_pLabelStart = new QLabel( this ) ;
	m_pLabelStart->setPixmap( QPixmap( ":/res/audio_start.png" ).scaled( 128, 128 ) ) ;

	m_pLabelStop = new QLabel( this ) ;
	m_pLabelStop->setPixmap( QPixmap( ":/res/audio_stop.png" ).scaled( 128, 128 ) ) ;

	auto pLayout = new QHBoxLayout( this ) ;
	pLayout->addStretch() ;
	pLayout->addWidget( m_pLabelStart ) ;
	pLayout->addWidget( m_pLabelStop ) ;
	pLayout->addStretch() ;

	this->setStatus( false ) ;
	this->setFixedHeight( 200 ) ;
	
	connect( this, &AudioForm::sigChangeEnable,
		this, &AudioForm::changeEnable, Qt::BlockingQueuedConnection ) ;
}

void AudioForm::setStatus( bool recording )
{
	m_pLabelStart->setVisible( !recording ) ;
	m_pLabelStop->setVisible( recording ) ;
}

void AudioForm::changeEnable( bool enable )
{
	auto id = boost::this_thread::get_id() ;
	m_pLabelStart->setEnabled( enable ) ;
	m_pLabelStop->setEnabled( enable ) ;
}

void AudioForm::playAudio( string file )
{
	global_pc().post( [ file ]
	{
		media_play( file.c_str() ) ;
		file_plan_remove( file.c_str() ) ;
	} ) ;
}

void AudioForm::keyPressEvent( QKeyEvent* event )
{
	if ( !m_pLabelStart->isEnabled() || !m_pLabelStop->isEnabled() )
	{
		return ;
	}

	if ( event->key() == Qt::Key_Space )
	{
		if ( !event->isAutoRepeat() && !m_bPressed )
		{
			m_bPressed = true ;
			this->setStatus( true ) ;
			this->startRecorder() ;
		}
	}
	
	QWidget::keyPressEvent( event ) ;
}

void AudioForm::keyReleaseEvent( QKeyEvent* event )
{
	if ( !m_pLabelStart->isEnabled() || !m_pLabelStop->isEnabled() )
	{
		return ;
	}

	if ( event->key() == Qt::Key_Space )
	{
		if ( !event->isAutoRepeat() && m_bPressed )
		{
			m_bPressed = false ;
			this->setStatus( false ) ;
			this->changeEnable( false ) ;
			this->stopRecorder() ;
		}
	}
	
	QWidget::keyReleaseEvent( event ) ;
}

void AudioForm::startRecorder()
{
	global_pc().post( [=]
	{
		string file ;
		{
			QTemporaryFile tmpfile ;
			tmpfile.setFileTemplate( tmpfile.fileTemplate() + ".aac" ) ;
			if ( tmpfile.open() )
			{
				file = tmpfile.fileName().toLocal8Bit().toStdString() ;
			
				tmpfile.setAutoRemove( false ) ;
				tmpfile.close() ;
			}
		}

		if ( !file.empty() )
		{
			if ( media_recorder( 0, file.c_str(), m_strEventName.c_str() ) == 200 )
			{
				for ( auto&& dbnode : global_cloudnodes_sender() )
				{
					file_meta fm ;
					fm.id = ".chat?audio?aac?" ;
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
			}
		}

		file_plan_remove( file.c_str() ) ;
		emit sigChangeEnable( true ) ;
	} ) ;
}

void AudioForm::stopRecorder()
{
	SetEvent( m_hEvent ) ;
}

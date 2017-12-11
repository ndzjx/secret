
#ifndef AUDIOFORM_H
#define AUDIOFORM_H

#include <QWidget>
#include <QKeyEvent>
#include <QLabel>
#include "mod_db.h"

class AudioForm : public QWidget
{
    Q_OBJECT
public:
    explicit AudioForm(QWidget *parent = 0);

signals:
	void sigChangeEnable( bool enable ) ;

public slots:
	void setStatus( bool recording ) ;
	void changeEnable( bool enable ) ;
	void playAudio( string file ) ;

private:
	void keyPressEvent( QKeyEvent* event ) override ;
	void keyReleaseEvent( QKeyEvent* event ) override ;

	void startRecorder() ;
	void stopRecorder() ;

private:
	bool	m_bPressed		= false		;
	QLabel* m_pLabelStart	= nullptr	;
	QLabel* m_pLabelStop	= nullptr	;
	string	m_strEventName				;
	HANDLE	m_hEvent		= NULL		;
};

#endif // AUDIOFORM_H

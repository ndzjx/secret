
#ifndef CUPLOADWIDGET_H
#define CUPLOADWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "ecloud.h"

class CUploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CUploadWidget(QWidget *parent = 0);
	~CUploadWidget();

signals:
	void tableItemStatusChanged( int item, int status ) ;
	void working( bool work ) ;
	int appendItem( QString tooltip, QString text, qint64 size ) ;
	void dirItemStatusChanged( QString dir, int status ) ;

public slots:
	void setTableItemStatus( int item, int status ) ;
	void setWorking( bool work ) ;
	int itemAppend( QString tooltip, QString text, qint64 size ) ;
	void SetDirItemStatusChanged( QString dir, int status ) ;

protected:  
    void dragEnterEvent( QDragEnterEvent* e ) ;
    void dropEvent( QDropEvent* e ) ;

private:
	void addTableItems( const vector<QString>& items, std::shared_ptr<void> fina ) ;
	bool addDir( const QString& dir ) ;

private:
	QListWidget*	m_pListDirs		;
	QTableWidget*	m_pTableUpload	;
	QPushButton*	m_pBtnSync		;
	QLabel*			m_pLabelMV		;
};

#endif // CUPLOADWIDGET_H

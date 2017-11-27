
#ifndef CUPLOADWIDGET_H
#define CUPLOADWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include "ecloud.h"

class CUploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CUploadWidget(QWidget *parent = 0);

signals:
	void tableItemStatusChanged( int item, int status ) ;

public slots:
	void setTableItemStatus( int item, int status ) ;

protected:  
    void dragEnterEvent( QDragEnterEvent* e ) ;
    void dropEvent( QDropEvent* e ) ;

private:
	void addTableItems( const vector<QString>& items ) ;

private:
	QTableWidget* m_pTable ;
};

#endif // CUPLOADWIDGET_H

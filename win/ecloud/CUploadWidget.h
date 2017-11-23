
#ifndef CUPLOADWIDGET_H
#define CUPLOADWIDGET_H

#include <mod_std.h>
#include <QWidget>
#include <QTableWidget>

class CUploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CUploadWidget(QWidget *parent = 0);

signals:

public slots:

protected:  
    void dragEnterEvent( QDragEnterEvent* e ) ;
    void dropEvent( QDropEvent* e ) ;

private:
	void addTableItems( const vector<QString>& items ) ;
	void setTableItemStatus( int item, int status ) ;

private:
	QTableWidget* m_pTable ;
};

#endif // CUPLOADWIDGET_H

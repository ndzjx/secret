#ifndef CBROWSERWIDGET_H
#define CBROWSERWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <map>
#include <string>
using namespace std;

class CTreeAnalysis;

class CBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CBrowserWidget(QWidget *parent = 0);

	void update();

signals:
	void sigInitData(map< pair<string, string>, string > &data);
	void sigUpdated() ;
	void sigDownloadStatusChanged( int item, int status ) ;

public slots:
	void slotInitData(map< pair<string, string>, string > &data);
	void slotUpdated() ;
	void slotDownloadStatusChanged( int item, int status ) ;

private:
    QListWidget *m_pLstDirs;
	QTableWidget* m_pTableDownload = nullptr ;
    CTreeAnalysis *m_pCurNode;
    CTreeAnalysis *m_pTreeNode;
	map< pair<string, string>, string > m_oDirs;

    void initData();
    void showDirs();
    void buildPath(CTreeAnalysis *node, const QString &dir);
    void clearTree(CTreeAnalysis *node);
	int appendDownload( QString tooltip, QString text ) ;
};

#endif // CBROWSERWIDGET_H

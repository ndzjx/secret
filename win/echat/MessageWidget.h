#ifndef MessageWidget_H
#define MessageWidget_H

#include <QWidget>
#include <QStringList>
#include <vector>
#include <QVBoxLayout>

using namespace std;

struct Message
{
    QPixmap icon;
    QString nickName;
    QString textMsg;
    QPixmap imageMsg;
};

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(QWidget* parent = Q_NULLPTR);
    ~MessageWidget();

    void addImageMsg(QPixmap icon, QString nickName, QPixmap msg);
    void addTextMsg(QPixmap icon, QString nickName, QString msg);
    void addVoiceMsg(QPixmap icon, QString nickName, QString msg);
private:
    void addMsgWidget(Message msg);

    void paintWidgetList();
    void refreshWidgetList();
    QWidget *createMsgWidget(Message msg);

    void clear();
    void ClearWidgetList();

signals:

public slots:

private:
    QVBoxLayout *m_pmsgLayout;
    vector<Message> m_oMessageList;
    vector<QWidget *> m_pWidgetList;
};

#endif // MessageWidget_H

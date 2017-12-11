#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QClipboard>
#include <TextEdit.h>
#include "AudioForm.h"
#include "Emotion.h"

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
	void sigChangeEnable( bool enable ) ;

private slots:
    void openFile();
    void saveFile();
	void switchAudio();
	void changeEnable( bool enable ) ;
    void showEmotionWin();
    void sendEmotion(QString &path);
private:
    virtual void keyPressEvent(QKeyEvent* event);
private:
    QAction *openAction;
    QAction *saveAction;
    QAction *openemotion;
    TextEdit *textEdit;
	QPushButton* m_pBtnSend = nullptr;
	AudioForm* m_pAudioForm = nullptr;
    Emotion *dlg = nullptr;



public slots:
    bool sendFile(const QString &path);
    bool sendImage(const QString &path);
};

#endif

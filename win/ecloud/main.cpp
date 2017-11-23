#include <QApplication>
#include "CUploadWidget.h"
#include "CBrowserWidget.h"
#include "CSettingWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CUploadWidget oWidget;
//    CBrowserWidget oWidget;
//    CSettingWidget oWidget;

    oWidget.show();


    return app.exec();
}

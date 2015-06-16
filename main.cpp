#include "gui/mainwindow.h"
#include <QApplication>

#include <QMessageBox>
#include <iostream>
#include <QProcess>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <QTextCodec>
#include "core/board.h"
#include "gui/qgameboard.h"
#include "gui/qgameoverwindow.h"
#include "SelfUpdate.h"

void executeUpdateExe(QString pitchUrl, QString latestUrl)
{
    QProcess p;
    QString program = "update.exe";
    QStringList argv;
    argv << pitchUrl << latestUrl;
    qDebug() << argv;
    p.startDetached(program, argv);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 检测是否需要升级
    SelfUpdate selfUpdate;
    selfUpdate.getUpdateSoftWare();
    if(selfUpdate.needUpdate())
    {
        qDebug() << "Need Update";
        if (QMessageBox::information(
                    NULL,
                    QStringLiteral("升级提示"),
                    ("当前版本：" + SelfUpdate::getLocalVer()+ "，最新版本：" + selfUpdate.netVer() + "。是否需要升级？！"),
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            executeUpdateExe(selfUpdate.pitchUrl(), selfUpdate.latestUrl());
            return 0;
        }
    }


    srand(time(NULL));
    QGameBoard board;
    board.show();

    return a.exec();
}

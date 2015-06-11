#ifndef SELFUPDATE_H
#define SELFUPDATE_H

#include <QObject>
#include <QString>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

typedef struct _FileDif_
{
    QString ver;
    QString url;
}FileDif;

typedef struct _UpdateSoftWare_
{
    QString ver;
    QString url;
    QList<FileDif> fileDif;
    int error;
}UpdateSoftWare;

class SelfUpdate : public QObject
{
    Q_OBJECT
public:
    explicit SelfUpdate(QObject *parent = 0);
    ~SelfUpdate();
    enum NetError
    {
        NoError,
        GettingGainData = 1000,
        NotObject,
        NotArray,
        UnknowError,
        GetDataTimeOut,
        NullString,
    };

    static QString getLocalVer();
    QString netVer();
    QString pitchUrl();
    QString latestUrl();
    void getUpdateSoftWare();
    bool needUpdate();
signals:
    void getUpdateSoftWareError(int error);
public slots:
    void getUpdateSoftWareReply();
private:
    static QString m_localVer;
    QNetworkAccessManager *m_netWorker;
    QNetworkReply *m_replySoftWare;     // 软件升级信息

    QEventLoop m_loopSoftwareUpdate;
    UpdateSoftWare m_updateSoftWare;
};

#endif // SELFUPDATE_H

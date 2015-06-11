#include "SelfUpdate.h"

QString SelfUpdate::m_localVer = "0.99";
SelfUpdate::SelfUpdate(QObject *parent) : QObject(parent)
{
    m_netWorker = new QNetworkAccessManager(this);
}

SelfUpdate::~SelfUpdate()
{

}

QString SelfUpdate::getLocalVer()
{
    return m_localVer;
}

void SelfUpdate::getUpdateSoftWare()
{
    qDebug() << "update net";
    QNetworkRequest req;
    QString url = "http://lcqlcq.sinaapp.com/2048Update.php";
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_replySoftWare = m_netWorker->post(req, QByteArray("{}"));
#if 1
    connect(m_replySoftWare, &QNetworkReply::finished, &m_loopSoftwareUpdate, &QEventLoop::quit);
    connect(m_replySoftWare, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=](QNetworkReply::NetworkError err){
        if(err != QNetworkReply::NoError)
        {
            m_loopSoftwareUpdate.quit();
        }
    });

    QTimer *timer = new QTimer(this);
    timer->setInterval(2000);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=](){
        if(m_loopSoftwareUpdate.isRunning())
            m_loopSoftwareUpdate.quit();
        timer->deleteLater();
    });
    timer->start();
    m_loopSoftwareUpdate.exec();

    getUpdateSoftWareReply();
#else
        connect(m_replySoftWare, &QNetworkReply::finished, this, &SelfUpdate::getUpdateSoftWareReply);
#endif
}

void SelfUpdate::getUpdateSoftWareReply()
{
    int error;
    if (m_replySoftWare->error() == QNetworkReply::NoError)
    {
        QByteArray ba = m_replySoftWare->readAll();
        qDebug() << ba;

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &jsonError);

        jsonError.error = QJsonParseError::NoError;
        if(jsonError.error == QJsonParseError::NoError)
        {
            if(jsonDoc.isObject())
            {
                QJsonObject obj = jsonDoc.object();
                m_updateSoftWare.ver = obj.take("version").toString();
                m_updateSoftWare.url = obj.take("url").toString();

                QJsonValue fileDifValue = obj.take("fileDifference");
                if(fileDifValue.isArray())
                {
                    QJsonArray array = fileDifValue.toArray();
                    int size = array.size();
                    for(int i=0; i<size; i++)
                    {
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            FileDif fileDifInfo;
                            QJsonObject fileDifValue = value.toObject();
                            fileDifInfo.ver = fileDifValue.take("version").toString();
                            fileDifInfo.url = fileDifValue.take("url").toString();
                            m_updateSoftWare.fileDif.append(fileDifInfo);
                        }
                    }
                    error = NoError;
                }
                else
                {
                    error = NotArray;
                }
            }
            else
            {
                error = NotObject;
            }
        }
        else
        {
            error = jsonError.error;
        }
    }
    else
    {
        error = m_replySoftWare->error();
    }

    m_updateSoftWare.error = error;
    emit getUpdateSoftWareError(error);
    m_replySoftWare->deleteLater();
}

bool SelfUpdate::needUpdate()
{
    if(m_updateSoftWare.error == NoError)
    {
        if(m_localVer < m_updateSoftWare.ver)
        {
            return true;
        }
    }

    return false;
}

QString SelfUpdate::netVer()
{
    return m_updateSoftWare.ver;
}

QString SelfUpdate::pitchUrl()
{
    QString url;
    FileDif dif;
    foreach (dif, m_updateSoftWare.fileDif)
    {
        if(dif.ver == m_localVer)
        {
            url = dif.url;
            break;
        }
    }

    return url;
}

QString SelfUpdate::latestUrl()
{
    return m_updateSoftWare.url;
}

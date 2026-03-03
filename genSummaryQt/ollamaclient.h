#ifndef OLLAMACLIENT_H
#define OLLAMACLIENT_H

#include <QObject>
#include <QUrl>

class QNetworkReply;
class QNetworkAccessManager;

class OllamaClient : public QObject
{
    Q_OBJECT

public:
    OllamaClient(QObject *parent = nullptr);

    static QUrl url;
    static QString model;

    int request(const QString &systemMsg, const QString &userMsg, QJsonObject* jsonFormat=0);

signals:
    void response(int promptId, const QString &content, const QString &thoughts);
    void error(int promptId, const QString &errMsg);

private:
    void processResponse(QNetworkReply *reply);

private:
    int m_promptCounter = 0;
    QNetworkAccessManager *m_nam = nullptr;
};
#endif // OLLAMACLIENT_H

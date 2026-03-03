#include "ollamaclient.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

QUrl OllamaClient::url = QUrl("http://localhost:11434/api/chat");
QString OllamaClient::model = "llama3.2:3b";

OllamaClient::OllamaClient(QObject *parent) : QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
}

int OllamaClient::request(const QString &systemMsg, const QString &userMsg, QJsonObject* jsonFormat)
{
    QNetworkRequest request(OllamaClient::url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto msg = [](const QString &role, const QString &content) -> QJsonObject {
        return { { "role", role }, { "content", content } };
    };

    QJsonObject json(
            { { "model", OllamaClient::model },
              { "stream", false },
              { "messages", QJsonArray({ msg("system", systemMsg), msg("user", userMsg) }) } });
    if (jsonFormat!=0){
        json["format"]=*jsonFormat;
    }
    const QJsonDocument doc(json);
    const QByteArray data = doc.toJson();
    // qDebug("POST Data: %s\n\n", data.constData());

    QNetworkReply *reply = m_nam->post(request, data);

    if (reply) {
        const int promptId = ++m_promptCounter;
        reply->setProperty("#promptId", promptId);

        connect(reply, &QNetworkReply::finished, [this, reply]() {
            this->processResponse(reply);
            reply->deleteLater();
        });
        return promptId;
    }

    return -1;
}

void OllamaClient::processResponse(QNetworkReply *reply)
{
    if (reply == nullptr)
        return;

    const int promptId = reply->property("#promptId").toInt();

    const QByteArray bytes = reply->readAll();
    // qDebug("POST Response: %s\n\n", bytes.constData());

    QJsonParseError jsonParseError;
    const QJsonDocument resDoc = QJsonDocument::fromJson(bytes, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError) {
        emit error(promptId, jsonParseError.errorString());
        return;
    }

    const QJsonObject resDocObj = resDoc.object();
    if (resDocObj.contains("error")) {
        emit error(promptId, resDocObj.value("error").toString());
        return;
    }

    if (!resDocObj.contains("message")) {
        emit error(promptId, "Ollama response does not contain any message.");
    }

    const QJsonObject resObj = resDocObj.value("message").toObject();
    if (resObj.isEmpty() || resObj.value("role") != "assistant") {
        emit error(promptId, "Invalid response received from Ollama.");
        return;
    }

    const QString content = resObj.value("content").toString();
    const QString thoughts = resObj.value("thinking").toString();
    if (!content.isEmpty()) {
        emit response(promptId, content, thoughts);
        return;
    }

    if (!reply->errorString().isEmpty()) {
        emit error(promptId, reply->errorString());
        return;
    }

    emit error(promptId, "Something went wrong, not sure what it is.");
}

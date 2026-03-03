#include <QFile>
#include "abstracttask.h"
#include "ollamaclient.h"
#include <stdio.h>

AbstractTask::AbstractTask() : m_type{AbstractTask::Type::SceneSummary} {}

void AbstractTask::setPrompt(const QString &val)
{
    if (m_prompt == val || m_busy){
        qWarning() << "The prompt was not changed.";
        return;
    }

    m_prompt = val;
    emit promptChanged();
}

void AbstractTask::setScene(const Fountain::Body &scene){
    m_scene=scene;
}


void AbstractTask::onOllamaError(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = QString();
    m_sceneSummaryError = text;

    qWarning() << "Error: " << text;

    emit error(text, m_scene);
    emit finished();
    emit taskFinished(this);

    ollama->deleteLater();

    this->setBusy(false);
}

void AbstractTask::onOllamaResponse(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = text;
    m_sceneSummaryError = QString();

    char file_name[20];
    snprintf(file_name, 20, "./out/%d_out.txt", m_id);

    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << text;

    }
    file.close();

    Fountain::Element summaryElement;
    summaryElement.type = Fountain::Element::Synopsis;
    summaryElement.text = text;

    auto it = std::find_if(m_scene.begin(), m_scene.end(), [](const Fountain::Element &element) {
        return element.type == Fountain::Element::SceneHeading;
    });

    if (it == m_scene.end())
        m_scene.prepend(summaryElement);
    else {
        ++it;
        m_scene.insert(it, summaryElement);
    }

    const QByteArray sceneText =
        Fountain::Writer(m_scene, Fountain::Writer::StrictSyntaxOption).toByteArray();
    // qDebug("Summary: %s\n", qPrintable(text));

    // qApp->clipboard()->setText(sceneText);
    // qDebug("The sumarized scene is copied to clipboard.");
    emit summary(text, m_scene);
    emit finished();
    emit taskFinished(this);

    ollama->deleteLater();
    this->setBusy(false);
}

void AbstractTask::setBusy(bool val)
{
    if (m_busy == val)
        return;

    m_busy = val;
    emit busyChanged();
}



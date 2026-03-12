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

void AbstractTask::setBusy(bool val)
{
    if (m_busy == val)
        return;

    m_busy = val;
    emit busyChanged();
}



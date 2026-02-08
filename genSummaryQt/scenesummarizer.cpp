#include "scenesummarizer.h"
#include "ollamaclient.h"

#include <QFile>

SceneSummarizer::SceneSummarizer(QObject *parent) : QObject(parent)
{
    QFile file(":/prompts/scene.txt");
    if (file.open(QFile::ReadOnly))
        m_prompt = file.readAll();
    else
        m_prompt = "Summarize this scene for me please.";
}

SceneSummarizer::~SceneSummarizer() { }

void SceneSummarizer::setPrompt(const QString &val)
{
    if (m_prompt == val || m_busy)
        return;

    m_prompt = val;
    emit promptChanged();
}

bool SceneSummarizer::summarize(const Fountain::Body &scene)
{
    if (m_busy)
        return false;

    this->setBusy(true);

    m_scene = scene;
    m_sceneSummary = QString();

    QString sceneText;
    auto it = std::find_if(m_scene.begin(), m_scene.end(), [](const Fountain::Element &element) {
        return !(element.type >= Fountain::Element::SceneHeading
                 && element.type <= Fountain::Element::Transition);
    });
    if (it != m_scene.end()) {
        Fountain::Body sceneCopy;
        std::copy_if(m_scene.begin(), m_scene.end(), std::back_inserter(sceneCopy),
                     [=](const Fountain::Element &element) {
                         return (element.type >= Fountain::Element::SceneHeading
                                 && element.type <= Fountain::Element::Transition);
                     });

        sceneText = Fountain::Writer(sceneCopy, Fountain::Writer::NoOption).toString();
    } else {
        sceneText = Fountain::Writer(m_scene, Fountain::Writer::NoOption).toString();
    }

    OllamaClient *ollama = new OllamaClient(this);
    connect(ollama, &OllamaClient::response, this, &SceneSummarizer::onOllamaResponse);
    connect(ollama, &OllamaClient::error, this, &SceneSummarizer::onOllamaError);
    m_ollamaPromptId = ollama->request(m_prompt, sceneText);

    return m_ollamaPromptId >= 0;
}

void SceneSummarizer::setBusy(bool val)
{
    if (m_busy == val)
        return;

    m_busy = val;
    emit busyChanged();
}

void SceneSummarizer::onOllamaError(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = QString();
    m_sceneSummaryError = text;
    emit error(text, m_scene);
    emit finished();

    ollama->deleteLater();

    this->setBusy(false);
}

void SceneSummarizer::onOllamaResponse(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = text;
    m_sceneSummaryError = QString();
    emit summary(text, m_scene);
    emit finished();

    this->setBusy(false);
}

#include <QFile>
#include "scenesummarizer2.h"
#include "ollamaclient.h"

SceneSummarizer2::SceneSummarizer2(const Fountain::Body &scene, int id){
    m_scene=scene;
    m_type=AbstractTask::Type::SceneSummary;
    m_id=id;

    QFile file(":/prompts/scene.txt");
    if (file.open(QFile::ReadOnly))
        m_prompt = file.readAll();
    else
        m_prompt = "Summarize this scene for me please.";

    m_jsonFormat=0;
}



void SceneSummarizer2::onOllamaResponse(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = text;
    m_sceneSummaryError = QString();

    QString fileName="./out/sceneSummaries.txt";

    QFile file(fileName);

    QString toAdd;

    if (m_id==0){
        toAdd.push_back('{');
    }
    else{
        file.resize(file.size()-1);
        toAdd.push_back(',');
        toAdd.push_back('\n');
    }
    toAdd.push_back('"');
    for (char c:std::to_string(m_id)) toAdd.push_back(c);
    toAdd.push_back('"');
    toAdd.push_back(':');
    toAdd.push_back(' ');
    toAdd.push_back('"');
    for (QChar c:m_sceneSummary) toAdd.push_back(c);
    toAdd.push_back('"');

    toAdd.push_back('}');

        if (file.open(QIODeviceBase::ReadWrite | QIODeviceBase::Append | QIODevice::Text)){
           QTextStream out(&file);
            out<<toAdd;
        }

        file.close();

    //     const QJsonDocument doc2(json);
    //     const QByteArray data = doc2.toJson();

    //     out << data.constData();

    // file.close();

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

bool SceneSummarizer2::run()
{
    if (isBusy())
        return false;

    setBusy(true);

    m_sceneSummary = QString();

    //sceneText contains the text to summarize
    QString sceneText;

    //it represents whether the scene contains unwanted elements
    auto it = std::find_if(m_scene.begin(), m_scene.end(), [](const Fountain::Element &element) {
        return !(element.type >= Fountain::Element::SceneHeading
                 && element.type <= Fountain::Element::Transition);
    });

    //if the scene contains unwanted elements they are not counted towards sceneText
    if (it!=m_scene.end()) {
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
    connect(ollama, &OllamaClient::response, this, &AbstractTask::onOllamaResponse);
    connect(ollama, &OllamaClient::error, this, &AbstractTask::onOllamaError);
    m_ollamaPromptId = ollama->request(m_prompt, sceneText);

    return m_ollamaPromptId >= 0;
}

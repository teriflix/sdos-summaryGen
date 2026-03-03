#include <QFile>
#include "storysummarizer.h"
#include "ollamaclient.h"

StorySummarizer::StorySummarizer(const Fountain::Body &scene, int id){
    m_scene=scene;
    m_type=AbstractTask::Type::SceneSummary;
    m_id=id;

    QFile file(":/prompts/story.txt");
    if (file.open(QFile::ReadOnly))
        m_prompt = file.readAll();
    else
        m_prompt = "Summarize this story for me please.";
    m_jsonFormat=0;
}


bool StorySummarizer::run()
{
    if (isBusy())
        return false;

    setBusy(true);

    m_sceneSummary = QString();

    //sceneText contains the text to summarize
    QString sceneText;


    //flag represents whether the given scene contains unwanted elements if so, they are not counted towards sceneText
    bool flag=false;
    for (auto it=m_scene.begin(); it!= m_scene.end(); it++){
        switch(it->type){
        case Fountain::Element::None:{
            flag=true;
        }break;
        case Fountain::Element::Unknown:{
            flag=true;
        }break;
        case Fountain::Element::PageBreak:{
            flag=true;
        }break;
        case Fountain::Element::LineBreak:{
            flag=true;
        }break;
        case Fountain::Element::Section:{
            flag=true;
        }break;
        case Fountain::Element::Synopsis:{
            flag=true;
        }break;
        default:{

        }
        }
    }

    if (flag) {
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

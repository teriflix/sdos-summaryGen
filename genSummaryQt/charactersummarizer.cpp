#include <QFile>
#include "charactersummarizer.h"
#include "ollamaclient.h"

CharacterSummarizer::CharacterSummarizer(const Fountain::Body &scene, int id, QString charName){
    m_scene=scene;
    m_type=AbstractTask::Type::SceneSummary;
    m_id=id;
    m_charName=charName;
    QFile file(":/prompts/character.txt");
    m_prompt=QString("Act as a script analyst. Analyze the provided script segment and summarize the character %1.").arg(charName);

    if (file.open(QFile::ReadOnly))
        m_prompt.append(file.readAll());

    // 1. Create individual property objects
    QJsonObject charProp;
    charProp.insert("title", "Character");
    charProp.insert("type", "string");

    QJsonObject goalProp;
    goalProp.insert("title", "Goal");
    goalProp.insert("type", "string");

    QJsonObject motivProp;
    motivProp.insert("title", "Motivation");
    motivProp.insert("type", "string");

    QJsonObject conflictProp;
    conflictProp.insert("title", "Conflict");
    conflictProp.insert("type", "string");

    QJsonObject epiphanyProp;
    epiphanyProp.insert("title", "Epiphany");
    epiphanyProp.insert("type", "string");

    QJsonObject summaryProp;
    summaryProp.insert("title", "Summary");
    summaryProp.insert("type", "string");

    // 2. Assemble the "properties" container
    QJsonObject properties;
    properties.insert("character", charProp);
    properties.insert("goal", goalProp);
    properties.insert("motivation", motivProp);
    properties.insert("conflict", conflictProp);
    properties.insert("epiphany", epiphanyProp);
    properties.insert("summary", summaryProp);

    // 3. Create the "required" array
    QJsonArray required;
    required.append("character");
    required.append("goal");
    required.append("motivation");
    required.append("conflict");
    required.append("epiphany");
    required.append("summary");

    // 4. Assemble the root object
    QJsonObject* root= new QJsonObject;
    root->insert("properties", properties);
    root->insert("required", required);
    root->insert("title", "CharacterSummary");
    root->insert("type", "object");

    m_jsonFormat=root;
}


bool CharacterSummarizer::run()
{
    if (isBusy())
        return false;

    setBusy(true);

    m_sceneSummary = QString();

    //sceneText contains the text to summarize
    QString sceneText;
    // qDebug()<<"OK\n";

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
    m_ollamaPromptId = ollama->request(m_prompt, sceneText, m_jsonFormat);

    return m_ollamaPromptId >= 0;
}

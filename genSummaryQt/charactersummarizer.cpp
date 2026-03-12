#include <QFile>
#include "charactersummarizer.h"
#include "ollamaclient.h"
#include <sys/stat.h>
#include <filesystem>

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

void CharacterSummarizer::onOllamaResponse(int promptId, const QString &text)
{
    OllamaClient *ollama = qobject_cast<OllamaClient *>(this->sender());
    if (ollama == nullptr || m_ollamaPromptId != promptId)
        return;

    m_sceneSummary = text;
    m_sceneSummaryError = QString();


    qDebug("Summary: %s\n", qPrintable(text));

    // qApp->clipboard()->setText(sceneText);
    // qDebug("The sumarized scene is copied to clipboard.");

    ollama->deleteLater();
    this->setBusy(false);
}

bool CharacterSummarizer::run()
{
    if (isBusy())
        return false;

    setBusy(true);

    m_sceneSummary = QString();

    std::string path="./out/";
    struct stat sb;
    //sceneText contains the text to summarize
    QString sceneText;

    QList<std::string> sceneSummaries;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {

        // Converting the path to const char * in the subsequent lines
        std::filesystem::path outfilename = entry.path();
        std::string outfilename_str = outfilename.string();
        const char* path = outfilename_str.c_str();
        // char* fileName= (char*) malloc(50);
        // strlcpy(fileName, path, 50);
        // Testing whether the path points to a
        // non-directory or not If it does, displays path
        if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
            sceneSummaries.push_back(outfilename_str);

    }
    // sort so that scene summaries are read in increasing order
    std::sort(sceneSummaries.begin(), sceneSummaries.end());

    int l=sceneSummaries.size();

    for (int i=0; i<l; i+=5){
        QString groupedSceneSummary="";
        for (int j=i; j<std::min(l,i+5); j++){

            QFile file(QString::fromStdString(sceneSummaries[j]));
            if (file.open(QFile::ReadOnly))
                groupedSceneSummary.append(file.readAll());
        }
        OllamaClient *ollama = new OllamaClient(this);
        connect(ollama, &OllamaClient::response, this, &AbstractTask::onOllamaResponse);
        connect(ollama, &OllamaClient::error, this, &AbstractTask::onOllamaError);
        m_ollamaPromptId = ollama->request(m_prompt, groupedSceneSummary, m_jsonFormat);

    }
    // qDebug()<<"OK\n";

    //flag represents whether the given scene contains unwanted elements if so, they are not counted towards sceneText
    // bool flag=false;
    // for (auto it=m_scene.begin(); it!= m_scene.end(); it++){
    //     switch(it->type){
    //     case Fountain::Element::None:{
    //         flag=true;
    //     }break;
    //     case Fountain::Element::Unknown:{
    //         flag=true;
    //     }break;
    //     case Fountain::Element::PageBreak:{
    //         flag=true;
    //     }break;
    //     case Fountain::Element::LineBreak:{
    //         flag=true;
    //     }break;
    //     case Fountain::Element::Section:{
    //         flag=true;
    //     }break;
    //     case Fountain::Element::Synopsis:{
    //         flag=true;
    //     }break;
    //     default:{

    //     }
    //     }
    // }

    // if (flag) {
    //     Fountain::Body sceneCopy;
    //     std::copy_if(m_scene.begin(), m_scene.end(), std::back_inserter(sceneCopy),
    //                  [=](const Fountain::Element &element) {
    //                      return (element.type >= Fountain::Element::SceneHeading
    //                              && element.type <= Fountain::Element::Transition);
    //                  });

    //     sceneText = Fountain::Writer(sceneCopy, Fountain::Writer::NoOption).toString();
    // } else {
    //     sceneText = Fountain::Writer(m_scene, Fountain::Writer::NoOption).toString();
    // }




    return m_ollamaPromptId >= 0;
}

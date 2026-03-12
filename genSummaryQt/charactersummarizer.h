#ifndef CHARACTERSUMMARIZER_H
#define CHARACTERSUMMARIZER_H

#include "abstracttask.h"

class CharacterSummarizer: public AbstractTask
{
public:
    CharacterSummarizer(const Fountain::Body &scene, int id, QString char_name);
    bool run();
    void onOllamaResponse(int promptId, const QString &text);

private:
    QString m_charName;

};


#endif // CHARACTERSUMMARIZER_H

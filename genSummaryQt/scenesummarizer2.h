#ifndef SCENESUMMARIZER2_H
#define SCENESUMMARIZER2_H

#include "abstracttask.h"

class SceneSummarizer2: public AbstractTask
{
public:
    SceneSummarizer2(const Fountain::Body &scene, int id);
    bool run();
    void onOllamaResponse(int promptId, const QString &text);
};

#endif // SCENESUMMARIZER2_H

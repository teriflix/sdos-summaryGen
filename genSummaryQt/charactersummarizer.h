#ifndef CHARACTERSUMMARIZER_H
#define CHARACTERSUMMARIZER_H

#include "abstracttask.h"

class CharacterSummarizer2: public AbstractTask
{
public:
    CharacterSummarizer2(const Fountain::Body &scene, int id);
    bool run();
};


#endif // CHARACTERSUMMARIZER_H

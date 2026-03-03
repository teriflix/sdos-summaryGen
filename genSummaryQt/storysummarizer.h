#ifndef STORYSUMMARIZER_H
#define STORYSUMMARIZER_H

#include "abstracttask.h"

class StorySummarizer: public AbstractTask
{
public:
    StorySummarizer(const Fountain::Body &scene, int id);
    bool run();
};

#endif // STORYSUMMARIZER_H

#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <QObject>
#include <abstracttask.h>
#include <QQueue>

class TaskQueue : public QObject
{
    Q_OBJECT
public:
    bool enqueue(AbstractTask* task);
    void run();

signals:
    void taskQueueFinished();

public slots:
    void onTaskFinished(AbstractTask* task);
    void onTaskQueueFinished();

private:
    QQueue<AbstractTask*> m_queue;
};

#endif // TASKQUEUE_H


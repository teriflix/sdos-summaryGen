#include <taskqueue.h>

bool TaskQueue::enqueue(AbstractTask * task){
    if (task!=nullptr) {m_queue.enqueue(task); return 1;}
    else return 0;
}

void TaskQueue::run(){
    if (m_queue.empty()) {
        return;
    }
    AbstractTask* task=m_queue.dequeue();

    connect(task, &AbstractTask::taskFinished, this, &TaskQueue::onTaskFinished);

    task->run();

}

void TaskQueue::onTaskFinished(AbstractTask* task){
    delete task;
    run();
}

void TaskQueue::onTaskQueueFinished(){
    ;
}

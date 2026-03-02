#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H

#include <QObject>
#include "fountain.h"

class AbstractTask : public QObject
{
    Q_OBJECT

public:
    AbstractTask();
    virtual bool run() =0;
    virtual ~AbstractTask()= default;

    //getter methods
    QString errorMessage() const { return m_busy ? QString() : m_sceneSummaryError; }
    QString sceneSummary() const { return m_busy ? QString() : m_sceneSummary; }
    QString prompt() const { return m_prompt; }
    bool isBusy() const { return m_busy; }
    Fountain::Body scene() const { return m_scene; }

    //setter methods
    void setPrompt(const QString &val);
    void setBusy(bool val);
    void setScene(const Fountain::Body &scene);

    enum class Type {
        SceneSummary,
        CharacterSummary,
        StorySummary,
    };

    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt NOTIFY promptChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY finished)
    Q_PROPERTY(QString sceneSummary READ sceneSummary NOTIFY finished)

signals:
    void error(const QString &error, const Fountain::Body &scene);
    void summary(const QString &text, const Fountain::Body &scene);
    void taskFinished(AbstractTask* task);
    void busyChanged();
    void promptChanged();
    void finished();

public slots:
    void onOllamaError(int promptId, const QString &text);
    void onOllamaResponse(int promptId, const QString &text);

protected:
    int m_ollamaPromptId = -1;
    bool m_busy = false;
    QString m_prompt;
    QString m_sceneSummary;
    QString m_sceneSummaryError;
    Fountain::Body m_scene;
    Type m_type;
    int m_id;
};

#endif // ABSTRACTTASK_H

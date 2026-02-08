#ifndef SCENESUMMARIZER_H
#define SCENESUMMARIZER_H

#include <QObject>

#include "fountain.h"

class SceneSummarizer : public QObject
{
    Q_OBJECT

public:
    SceneSummarizer(QObject *parent = nullptr);
    ~SceneSummarizer();

    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    bool isBusy() const { return m_busy; }
    Q_SIGNAL void busyChanged();

    Q_PROPERTY(QString prompt READ prompt WRITE setPrompt NOTIFY promptChanged)
    void setPrompt(const QString &val);
    QString prompt() const { return m_prompt; }
    Q_SIGNAL void promptChanged();

    bool summarize(const Fountain::Body &scene);

    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY finished)
    QString errorMessage() const { return m_busy ? QString() : m_sceneSummaryError; }

    Q_PROPERTY(QString sceneSummary READ sceneSummary NOTIFY finished)
    QString sceneSummary() const { return m_busy ? QString() : m_sceneSummary; }

    Fountain::Body scene() const { return m_scene; }

signals:
    void error(const QString &error, const Fountain::Body &scene);
    void summary(const QString &text, const Fountain::Body &scene);
    void finished();

private:
    void setBusy(bool val);
    void onOllamaError(int promptId, const QString &text);
    void onOllamaResponse(int promptId, const QString &text);

private:
    int m_ollamaPromptId = -1;
    bool m_busy = false;
    QString m_prompt;
    QString m_sceneSummary;
    QString m_sceneSummaryError;
    Fountain::Body m_scene;
};

#endif // SCENESUMMARIZER_H

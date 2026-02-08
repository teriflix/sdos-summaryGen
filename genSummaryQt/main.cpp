#include <QFile>
#include <QtDebug>
#include <QClipboard>
#include <QGuiApplication>
#include <QCommandLineParser>

#include "ollamaclient.h"
#include "scenesummarizer.h"

int main(int argc, char **argv)
{
    QGuiApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("A simple command-line screenplay analysis tool to generate "
                                     "scene summary of a specific scene.");
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("url", "The URL to connect to.", "url"));
    parser.addOption(QCommandLineOption("model", "The model to use.", "model"));
    parser.addOption(
            QCommandLineOption("promptFile", "File path to prompt instructions.", "promptFile"));
    parser.addOption(QCommandLineOption("scene", "The scene number to load.", "scene"));
    parser.addOption(QCommandLineOption("file", "The file path to save or load data.", "file"));

    parser.process(a);

    // file and scene number are mandatory, others are optional.
    if (!parser.isSet("file") || !parser.isSet("scene")) {
        qWarning() << "Error: Both --file and --scene options are required.";
        parser.showHelp(1);
        return -1;
    }

    // Parse the file as a fountain file and report error if any.
    const QString fileName = parser.value("file");
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Cannot open file reading: " << fileName;
        return -1;
    }

    const Fountain::Body screenplay = Fountain::Parser(&file).body();
    if (screenplay.isEmpty()) {
        qWarning() << "No screenplay found in file: " << fileName;
        return -1;
    }

    // Look for the given scene number.
    const QString sceneNumberArg = parser.value("scene");
    bool sceneNumberIsInt = false;
    int sceneNumber = sceneNumberArg.toInt(&sceneNumberIsInt);
    Fountain::Body scene;
    if (sceneNumberIsInt && sceneNumber > 0) {
        for (int i = 0; i < screenplay.size(); i++) {
            const Fountain::Element element = screenplay.at(i);
            if (element.type == Fountain::Element::SceneHeading) {
                --sceneNumber;
            }

            if (sceneNumber == 0 && element.type != Fountain::Element::Synopsis)
                scene.append(element);
            else if (sceneNumber < 0)
                break;
        }
    } else {
        qWarning() << "Invalid scene number specified: " << sceneNumberArg;
        return -1;
    }

    if (scene.isEmpty()) {
        qWarning() << "The specified scene number was not found: " << sceneNumberArg;
        return -1;
    }

    const QString url = parser.value("url");
    if (!url.isEmpty()) {
        QUrl _url = QUrl(url);
        if (_url.isValid()) {
            OllamaClient::url = _url;
        }
    }
    qDebug() << "Using Ollama at " << OllamaClient::url.toString();

    const QString model = parser.value("model");
    if (!model.isEmpty()) {
        OllamaClient::model = model;
    }
    qDebug() << "Using model " << OllamaClient::model;

    SceneSummarizer summarizer;

    const QString promptFile = parser.value("promptFile");
    if (!promptFile.isEmpty()) {
        QFile file(promptFile);
        if (file.open(QFile::ReadOnly)) {
            const QString prompt = file.readAll();
            if (!prompt.isEmpty()) {
                summarizer.setPrompt(prompt);
                qDebug() << "Using prompt from: " << promptFile;
            }
        }
    }

    const QByteArray inputScene = Fountain::Writer(scene, Fountain::Writer::NoOption).toByteArray();
    qDebug("Scene Text: \n\n%s\n\n", inputScene.constData());

    QObject::connect(&summarizer, &SceneSummarizer::summary, [&scene](const QString &text) {
        Fountain::Element summaryElement;
        summaryElement.type = Fountain::Element::Synopsis;
        summaryElement.text = text;

        auto it = std::find_if(scene.begin(), scene.end(), [](const Fountain::Element &element) {
            return element.type == Fountain::Element::SceneHeading;
        });
        if (it == scene.end())
            scene.prepend(summaryElement);
        else {
            ++it;
            scene.insert(it, summaryElement);
        }

        const QByteArray sceneText =
                Fountain::Writer(scene, Fountain::Writer::StrictSyntaxOption).toByteArray();
        qDebug("Summary: %s\n", qPrintable(text));

        qApp->clipboard()->setText(sceneText);
        qDebug("The sumarized scene is copied to clipboard.");
    });
    QObject::connect(&summarizer, &SceneSummarizer::error,
                     [=](const QString &text) { qWarning() << "Error: " << text; });
    QObject::connect(&summarizer, &SceneSummarizer::finished, &a, &QCoreApplication::quit);

    if (summarizer.summarize(scene)) {
        qDebug() << "Generating scene summary ...";
    } else {
        qWarning() << "Couldn't generate scene summary!";
        return -1;
    }

    return a.exec();
}

#include "Transcriber.h"
#include "TranscriberSetting.h"
#include "TranscriberElement.h"
#include "../io/UtauLibrary.h"

#include <QTextStream>

using namespace stand::synthesis;

Transcriber::Transcriber(const TranscriberSetting *s, QObject *parent) :
    QThread(parent)
{
    setting = new TranscriberSetting();
    setting->numThreads = s->numThreads;
    setting->codec = s->codec;
    setting->root = s->root;
    for(int i = 0; i < s->libraries.size(); i++)
    {
        setting->libraries.push_back(s->libraries.at(i));
    }
}

Transcriber::~Transcriber()
{
    for(int i = 0; i < setting->libraries.size();i ++)
    {
        delete setting->libraries.at(i).body;
    }
    for(int i = 0; i < elements.size(); i++)
    {
        elements.at(i)->finishTranscription();
        elements.at(i)->wait();
        elements.at(i)->disconnect();
        delete elements.at(i);
    }
    delete setting;
}

void Transcriber::run()
{
    qDebug("Transcriber::run();");

    if(!_writeSettings())
    {
        emit finish(false);
        return;
    }

    QMutex mutex;
    mutex.lock();
    elements.clear();
    for(int i = 0; i < setting->numThreads && i < setting->libraries.at(0).body->size(); i++)
    {
        TranscriberElement *e = new TranscriberElement(i, setting, &mutex, this);
        elements.push_back(e);
        e->start();
        currentIndex = i;
    }
    currentIndex++;
    mutex.unlock();

    currentFinished = 0;
    waitMutex.lock();
    condition.wait(&waitMutex);
    waitMutex.unlock();

    qDebug(" Done::Transcriber::run();");
    emit finish(true);
}

void Transcriber::elementFinished(TranscriberElement *e)
{
    currentFinished++;
    if(currentIndex < setting->libraries.at(0).body->size())
    {
        e->setIndex(currentIndex);
        currentIndex++;
    }
    else
    {
        e->finishTranscription();
    }

    if(currentFinished == setting->libraries.at(0).body->size())
    {
        cancel();
    }

    emit progressChanged(currentFinished);
}

void Transcriber::cancel()
{
    for(int i = 0; i < elements.size(); i++)
    {
        elements.at(i)->finishTranscription();
    }
    condition.wakeAll();
}

bool Transcriber::_writeSettings()
{
    QDir current = setting->root;
    for(int i = 0; i < setting->libraries.size() - 1; i++)
    {
        QString filename = current.absoluteFilePath("vConnect.ini");
        QString base = current.relativeFilePath(setting->libraries.at(i).body->directory().absolutePath()) + QDir::separator();
        QString append = current.relativeFilePath(setting->libraries.at(i + 1).body->directory().absolutePath()) + QDir::separator();
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug(" Could not open file; %s", filename.toUtf8().data());
            return false;
        }
        QTextStream ofs;
        ofs.setDevice(&file);
        ofs.setCodec(setting->codec);
        ofs << "[BaseLibrary]\n"
            << "Enable=1\n"
            << "Brightness=" << setting->libraries.at(i).brightness << "\n"
            << "NoteNumber=" << setting->libraries.at(i).note << "\n"
            << "Directory=" << base << "\n"
            << "[BrightnessLibrary]\n"
            << "Enable=1\n"
            << "Brightness=" << setting->libraries.at(i + 1).brightness << "\n"
            << "NoteNumber=" << setting->libraries.at(i + 1).note << "\n"
            << "Directory=" << append << "\n"
               ;
        file.close();
        // 処理するディレクトリを移動する．
        current = QDir(current.absoluteFilePath(append));
    }

    QString filename = setting->root.absoluteFilePath("character.txt");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug(" Could not open file; %s", filename.toUtf8().data());
        return false;
    }
    QTextStream ofs;
    ofs.setDevice(&file);
    ofs.setCodec(setting->codec);
    ofs << "name=" << setting->libraryName << "\n"
        << "image=" << QFileInfo(setting->libraryIconPath).fileName() << "\n"
        << "author=" << setting->libraryAuthor << "\n"
        << "web=" << setting->libraryWeb << "\n"
           ;

    stand::io::UtauLibrary *rootLib = setting->libraries.at(0).body;
    rootLib->changeDirectory(setting->root);
    return rootLib->write(setting->root.absoluteFilePath("oto.ini"));
}

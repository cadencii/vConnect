#include "Transcriber.h"
#include "TranscriberSetting.h"
#include "TranscriberElement.h"
#include "../io/UtauLibrary.h"

using namespace stand::synthesis;

Transcriber::Transcriber(const TranscriberSetting *s, QObject *parent) :
    QThread(parent)
{
    setting = new TranscriberSetting();
    setting->numThreads = s->numThreads;
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

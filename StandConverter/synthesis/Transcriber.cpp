#include "Transcriber.h"
#include "TranscriberSetting.h"
#include "TranscriberElement.h"
#include "../io/UtauLibrary.h"

using namespace stand::synthesis;

Transcriber::Transcriber(const TranscriberSetting *s, QObject *parent) :
    QThread(parent)
{
    setting = s;
}

Transcriber::~Transcriber()
{
    delete setting->base.lib;
    for(int i = 0; i < setting->optionals.size();i ++)
    {
        delete setting->optionals.at(i).lib;
    }
    for(int i = 0; i < elements.size(); i++)
    {
        elements.at(i)->finishTranscription();
        elements.at(i)->wait();
        elements.at(i)->disconnect();
        delete elements.at(i);
    }
}

void Transcriber::run()
{
    qDebug("Transcriber::run();");

    QMutex mutex;
    mutex.lock();
    elements.clear();
    for(int i = 0; i < setting->numThreads && i < setting->base.lib->size(); i++)
    {
        TranscriberElement *e = new TranscriberElement(i, setting, &mutex, this);
        elements.push_back(e);
        e->start();
        currentIndex = i;
    }
    mutex.unlock();

    currentIndex = currentFinished = 0;
    waitMutex.lock();
    condition.wait(&waitMutex);
    waitMutex.unlock();

    qDebug(" Done::Transcriber::run();");
    emit finish(true);
}

void Transcriber::elementFinished(TranscriberElement *e)
{
    currentFinished++;
    if(currentIndex < setting->base.lib->size())
    {
        e->setIndex(currentIndex);
        currentIndex++;
    }
    else
    {
        e->finishTranscription();
    }
    // SafeGuard (要るかな？)
    if(currentFinished == setting->base.lib->size() - 1)
    {
        for(int i = 0; i < setting->numThreads; i++)
        {

        }
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

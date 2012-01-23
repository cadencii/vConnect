#include "Converter.h"

#include <QMessageBox>
#include <QTextCodec>

#include "../utility/Utility.h"
#include "../io/UtauLibrary.h"
#include "UtauPhonemeAnalyzer.h"

using namespace stand::synthesis;

Converter::Converter(const ConverterSetting &s, QWidget *parent)
    : QThread(parent)
{
    setting = s;
    isCanceled = false;
    currentFinished = currentPosition = 0;
    analyzers = NULL;
    rightBlanks = new float[s.library->size()];
    ignoreFlag = 0;
}

Converter::~Converter()
{
    for(unsigned int i = 0; i < setting.numThreads; i++)
    {
        analyzers[i]->converterFinished();
        analyzers[i]->disconnect();
        analyzers[i]->wait();
        delete analyzers[i];
    }
    this->disconnect();
    delete setting.library;
    delete[] analyzers;
    delete[] rightBlanks;
}

void Converter::cancel()
{
    for(unsigned int i = 0; i < setting.numThreads; i++)
    {
        analyzers[i]->converterFinished();
    }
    isCanceled = true;
    condition.wakeAll();
}

void Converter::recieveRightBlank(int index, float rightBlank)
{
    rightBlanks[index] = rightBlank;
}

void Converter::_initializeAnalyzers()
{
    mutex.lock();
    currentPosition = 0;
    analyzers = new UtauPhonemeAnalyzer*[setting.numThreads];
    for(unsigned int i = 0; i < setting.numThreads; i++)
    {
        if(i < setting.library->size())
        {
            analyzers[i] = new UtauPhonemeAnalyzer(i, setting, this, &mutex);
            currentPosition++;
            analyzers[i]->start();
        }
        else
        {
            analyzers[i] = NULL;
        }
    }
    mutex.unlock();
}

void Converter::analyzerFinished(UtauPhonemeAnalyzer *p, bool f)
{
    unsigned int i;
    currentFinished++;
    for(i = 0; i < setting.numThreads; i++)
    {
        if(analyzers[i] == p)
        {
            break;
        }
    }
    // 分析終了した分析器は次があれば次を分析させる．
    if(i != setting.numThreads)
    {
        if(currentPosition < setting.library->size())
        {
            analyzers[i]->setIndex(currentPosition);
            if(f)
            {
                currentPosition++;
            }
        }
        else
        {
            analyzers[i]->converterFinished();
        }
    }
    if(currentFinished == setting.library->size())
    {
        for(int i = 0; i < setting.numThreads; i++)
        {
            if(!isCanceled)
            {
                analyzers[i]->converterFinished();
            }
        }
        condition.wakeOne();
    }
    emit progressChanged(currentPosition);
}

void Converter::run()
{
    qDebug("Converter::run()");
    if(!setting.library || setting.library->size() == 0)
    {
        qDebug(" Error; no UtauLibrary exists.");
        emit finish(false);
        return;
    }

    // Initialize Analyzers
    _initializeAnalyzers();

    waitMutex.lock();
    condition.wait(&waitMutex);
    waitMutex.unlock();

    _writeOtoIni();

    emit finish(true);
    return;
}

void Converter::_writeOtoIni()
{
    stand::io::UtauLibrary out("", setting.outCodec);

    for(unsigned int i = 0; i < setting.library->size(); i++)
    {
        const stand::io::UtauPhoneme *p = setting.library->at(i);
        stand::io::UtauPhoneme target;
        target = *p;
        target.filename.setNum(i + 1);
        target.filename = target.filename + QString(".vvd");
        if(setting.normalizeUtauLibrary)
        {
            target.preUtterance = setting.preUtterance;
            target.rightBlank =  -setting.phonemeLength;
        }
        else
        {
            target.rightBlank = rightBlanks[i];
        }
        out.push_back(target);
    }
    QString filename = setting.outDir.absolutePath() + QDir::separator() + "oto.ini";
    out.write(filename, setting.outCodec);
}


#include "TranscriberElement.h"
#include "TranscriberSetting.h"
#include "Transcriber.h"
#include "../io/StandFile.h"
#include "../io/UtauLibrary.h"

#include <QVector>
#include <QFileInfo>
#include <QMutex>

using namespace stand::synthesis;

TranscriberElement::TranscriberElement(unsigned int index, const TranscriberSetting *s, QMutex *m, Transcriber *t)
{
    this->index = index;
    this->setting = s;
    this->mutex = m;
    this->transcriber = t;

    for(int i = 0; i < s->libraries.size(); i++)
    {
        items.push_back(s->libraries.at(i));
    }
    isFinished = false;
}

void TranscriberElement::setIndex(int index)
{
    this->index = index;
}

void TranscriberElement::finishTranscription()
{
    isFinished = true;
}

void TranscriberElement::run()
{
    qDebug("TranscriberElement::run();");

    do
    {
        _analyze();
        mutex->lock();
        transcriber->elementFinished(this);
        mutex->unlock();
    }
    while(!isFinished);

    qDebug(" Done::TranscriberElement::run();");
}

void TranscriberElement::_analyze()
{
    stand::io::StandFile *prev, *current = NULL;
    prev = new stand::io::StandFile();
    QString filename = items.at(0).body->directory().absolutePath() + QDir::separator() + items.at(0).body->at(index)->filename;
    // 簡単だけど拡張子チェック
    if(filename.contains(".vvd") && !prev->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()))
    {
        delete prev;
        prev = NULL;
    }
    for(int i = 1; i < items.size(); i++)
    {
        const stand::io::UtauPhoneme *phoneme = items.at(i).body->find(items.at(0).body->at(index)->pronounce);
        if(phoneme)
        {
            filename = items.at(i).body->directory().absolutePath() + QDir::separator() + phoneme->filename;
            current = new stand::io::StandFile();
            // 現在のファイルを読み込む→現在のファイルがよめていて，前回もファイルが読めていた場合だけ分析を行う．
            if(filename.contains(".vvd") && current->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()) && prev)
            {
                // 写像関数の計算と保存
                stand::io::StandFile::matching(prev, current);
                current->write(QDir::toNativeSeparators(filename).toLocal8Bit().data());
            }
            else
            {
                delete current;
                current = NULL;
            }
        }
        delete prev;
        prev = current;
    }
    delete current;
}

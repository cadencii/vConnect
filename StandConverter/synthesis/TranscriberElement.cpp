#include "TranscriberElement.h"
#include "TranscriberSetting.h"
#include "Transcriber.h"
#include "../io/StandFile.h"
#include "../io/UtauLibrary.h"

#include <QVector>
#include <QFileInfo>
#include <QMutex>

using namespace stand::synthesis;

TranscriberElement::TranscriberElement(unsigned int index, const TranscriberSetting *s, QMutex *m, Transcriber *t) :
    QThread(t)
{
    this->index = index;
    this->setting = s;
    this->mutex = m;
    this->transcriber = t;

    items.push_back(s->base);
    for(int i = 0; i < s->optionals.size(); i++)
    {
        items.push_back(s->optionals.at(i));
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
        stand::io::StandFile *prev, *current = NULL;
        prev = new stand::io::StandFile();
        QString filename = items.at(0).lib->directory().absolutePath() + QDir::separator() + items.at(0).lib->at(index)->filename;
        // 簡単だけど拡張子チェック
        if(filename.contains(".vvd") && !prev->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()))
        {
            qDebug("%s is not vvd file!", filename);
            delete prev;
            prev = NULL;
        }
        for(int i = 1; i < items.size(); i++)
        {
            const stand::io::UtauPhoneme *phoneme = items.at(i).lib->find(items.at(0).lib->at(index)->pronounce);
            if(phoneme)
            {
                filename = items.at(i).lib->directory().absolutePath() + QDir::separator() + phoneme->filename;
                current = new stand::io::StandFile();
                // 現在のファイルを読み込む→現在のファイルがよめていて，前回もファイルが読めていた場合だけ分析を行う．
                if(filename.contains(".vvd") && current->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()) && prev)
                {
                    // ToDo:: 写像関数の計算と保存
                    stand::io::StandFile::matching(prev, current);
                    // ファイルへ保存．
                    current->write(QDir::toNativeSeparators(filename).toLocal8Bit().data());
                }
                else
                {
                    qDebug("%s is not vvd file!", filename);
                    delete current;
                    current = NULL;
                }
            }
            delete prev;
            prev = current;
        }
        delete current;
        mutex->lock();
        transcriber->elementFinished(this);
        mutex->unlock();
    }
    while(!isFinished);

    qDebug(" Done::TranscriberElement::run();");
}

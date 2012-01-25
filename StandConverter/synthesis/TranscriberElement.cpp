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

    libs.push_back(s->base);
    for(int i = 0; i < s->optionals.size(); i++)
    {
        libs.push_back(s->optionals.at(i));
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
        QString filename = libs.at(0)->directory().absolutePath() + QDir::separator() + libs.at(0)->at(index)->filename;
        // 簡単だけど拡張子チェック
        if(filename.contains(".vvd") && !prev->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()))
        {
            qDebug("%s is not vvd file!", filename);
            delete prev;
            prev = NULL;
        }
        for(int i = 1; i < libs.size(); i++)
        {
            const stand::io::UtauPhoneme *phoneme = libs.at(i)->find(libs.at(0)->at(index)->pronounce);
            if(phoneme)
            {
                filename = libs.at(i)->directory().absolutePath() + QDir::separator() + phoneme->filename;
                current = new stand::io::StandFile();
                // 現在のファイルを読み込む→現在のファイルがよめていて，前回もファイルが読めていた場合だけ分析を行う．
                if(filename.contains(".vvd") && current->read(QDir::toNativeSeparators(filename).toLocal8Bit().data()) && prev)
                {
                    // ToDo:: 写像関数の計算と保存
                    stand::io::StandFile::matching(prev, current);
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

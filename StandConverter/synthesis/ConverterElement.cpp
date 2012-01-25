#include "ConverterElement.h"

#include "../io/UtauLibrary.h"
#include "../io/WaveFile.h"
#include "../io/StandFile.h"
#include "../utility/Utility.h"

#include "ConverterSetting.h"
#include "Converter.h"

#include "../math/World.h"
#include "../math/MFCCSet.h"
#include "../math/FFTSet.h"

#include <QFileInfo>

using namespace stand::synthesis;

// min, max macro
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

ConverterElement::ConverterElement(unsigned int index, const ConverterSetting &s, Converter *c, QMutex *m)
    : QThread(c)
{
    setting = s;
    _done = false;
    setIndex(index);
    mutex = m;
    directory = s.library->directory().absolutePath();
    converter = c;
}

void ConverterElement::converterFinished()
{
    _done = true;
}

void ConverterElement::setIndex(unsigned int index)
{
    const stand::io::UtauPhoneme *p = setting.library->at(index);
    if(p)
    {
        target = *p;
    }
    else
    {
        target.filename = "";
    }
    this->index = index;
}

void ConverterElement::run()
{
    while(!_done)
    {
        bool ret = true;
        if(target.filename != "")
        {
            _writeDebugString();
            _analyze();
        }
        else
        {
            qDebug(" Target was null.");
            ret = false;
        }

        if(mutex)
        {
            mutex->lock();
        }
        converter->analyzerFinished(this, ret);
        if(mutex)
        {
            mutex->unlock();
        }
    }
    qDebug(" Done :ConverterElement::run().");
}

void ConverterElement::_writeDebugString()
{
    qDebug("\nConverterElement::run()\n"
           " Analysis parameters\n"
           "  FileName: %s\n"
           "  Normalize option: %s\n"
           , target.filename
           , setting.normalizeUtauLibrary ? "true" : "false"
                         );
}

void ConverterElement::_analyze()
{
    // File duplication check
    QString outFileName;
    outFileName.setNum(index + 1);
    outFileName = setting.outDir.absolutePath() + QDir::separator() + outFileName + ".vvd";
    QFileInfo info(outFileName);
    if(info.exists() && setting.overwrite ==  false)
    {
        qDebug("  File already exists; %s", outFileName);
        return;
    }

    // Read WaveFile
    QString filename = (directory + QDir::separator() + target.filename);
    filename = QDir::toNativeSeparators(filename);
    stand::io::WaveFile wave(filename.toLocal8Bit().data());
    if(wave.empty())
    {
        qDebug("  Wave is empty!");
        return;
    }
    qDebug(" UtauPhonemeAnalzer::analyze(wave); length = %d", wave.length());

    // Cut wave.
    float leftBlank, rightBlank;
    if(setting.normalizeUtauLibrary)
    {
        leftBlank = target.leftBlank + target.preUtterance - setting.preUtterance;
        rightBlank = -setting.phonemeLength;
    }
    else
    {
        leftBlank = target.leftBlank;
        rightBlank = target.rightBlank;
    }

    // 変数準備
    stand::math::world::World::WorldSetting s;
    s.framePeriod = setting.framePeriod;
    s.fs = wave.header()->samplesPerSecond;

    // 波形を切って正規化
    int xLen = stand::utility::lengthForUtauSetting(wave.length(), s.fs, leftBlank, rightBlank);
    double *x = new double[xLen];

    for(int i = 0; i < xLen; i++)
    {
        int index = i + leftBlank / 1000.0 * wave.header()->samplesPerSecond;
        index = max(0, min(index, wave.length() - 1));
        x[i] = wave.data()[index];
    }
    stand::utility::normalizeByAnchor(x, xLen);

    // 波形から 1 ファイルを計算．
    stand::io::StandFile stf;
    stf.compute(x, xLen, s.fs, s.framePeriod, setting.cepstrumLength, -1, -1, setting.vorbisBitrate * 1024);

    delete[] x;

    if(mutex)
    {
        mutex->lock();
    }
    // 右ブランクを送る．ただし単位はミリ秒．
    converter->recieveRightBlank(index, (float)xLen / s.fs * 1000.0);
    if(mutex)
    {
        mutex->unlock();
    }

    if(!stf.write(QDir::toNativeSeparators(outFileName).toLocal8Bit().data()))
    {
        qDebug("ConverterElement::analyze(); // failed writing.");
    }
}

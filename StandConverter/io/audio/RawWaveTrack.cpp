#include "RawWaveTrack.h"

#include "../WaveFile.h"

using namespace stand::io::audio;

RawWaveTrack::RawWaveTrack(const stand::io::WaveFile *w, const QAudioFormat &format, QObject *parent) :
    AudioTrack(format, parent)
{
    _wave = w;
}

RawWaveTrack::~RawWaveTrack()
{
}

void RawWaveTrack::generateBuffer(double msPos, double *buffer, qint64 &length)
{
    qint64 samplePos = msPos / 1000.0 * format.sampleRate();
    generateBuffer(samplePos, buffer, length);
}

void RawWaveTrack::generateBuffer(qint64 samplePos, double *buffer, qint64 &length)
{
    qint64 bufferPos = 0;
    samplePos = samplePos * _wave->header()->samplesPerSecond / format.sampleRate();
    if(_wave->header()->samplesPerSecond == format.sampleRate())
    {
        qint64 endIndex = samplePos + length;
        if(endIndex > _wave->length())
        {
            endIndex = _wave->length();
            length = _wave->length() - samplePos;
        }
        for(int index = samplePos; index < endIndex; index++)
        {
            for(int i = 0; i < format.channels(); i++)
            {
                buffer[bufferPos] = _wave->data()[index];
                bufferPos++;
            }
        }
    }
    // ToDo:: Sampling 周波数変換をマシな式に変えるのは今後の課題．
    else
    {
        qint64 endIndex = samplePos + length * _wave->header()->samplesPerSecond / format.sampleRate();
        if(endIndex > _wave->length())
        {
            endIndex = _wave->length();
            length = (_wave->length() - samplePos) * format.sampleRate() / _wave->header()->samplesPerSecond;
        }
        for(int i = 0; i < length; i++)
        {
            int index = samplePos + i * _wave->header()->samplesPerSecond / format.sampleRate();
            for(int j = 0; j < format.channels(); j++)
            {
                buffer[bufferPos] = _wave->data()[index];
                bufferPos++;
            }
        }
    }
}

#include "AudioTrack.h"

using namespace stand::io::audio;

AudioTrack::AudioTrack(const QAudioFormat &format, QObject *parent) :
    QObject(parent)
{
    setFormat(format);
}

AudioTrack::~AudioTrack()
{
}

void AudioTrack::setFormat(const QAudioFormat &format)
{
    this->format = format;
}

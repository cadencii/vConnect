#include "AudioMixer.h"

using namespace stand::io::audio;

AudioMixer::AudioMixer(const QAudioFormat &format, QObject *parent)
    : QObject(parent)
{
    _output = NULL;
    _device = NULL;
    _pos = 0;
    setFormat(format);
}

AudioMixer::~AudioMixer()
{
    _destroy();
}

void AudioMixer::_destroy()
{
    if(_output)
    {
        _output->disconnect();
    }
    if(_device)
    {
        _device->disconnect();
    }
    delete _device;
    delete _output;
    _device = NULL;
    _output = NULL;
    _pos = 0;
}

void AudioMixer::setFormat(const QAudioFormat &format)
{
    QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if(!deviceInfo.isFormatSupported(format))
    {
        qWarning("AudioMixer::setFormat(); // Format not supported.");
        this->_format = deviceInfo.nearestFormat(format);
    }
    else
    {
        this->_format = format;
    }
    _destroy();
    _output = new QAudioOutput(this->_format, this);
    _device = new StreamDevice(this->_format, this->_format.sampleRate(), this);
    _device->connect(_device, SIGNAL(requireBuffer(qint64)), this, SLOT(mixTracks(qint64)));
    qDebug("AudioMixer::setFormat();");
    qDebug(" Format::sampleRate = %d [Hz]", _format.sampleRate());
    qDebug(" Format::sampleSize = %d [bits]", _format.sampleSize());
    qDebug(" Format::channels   = %d", _format.channels());
}

void AudioMixer::start()
{
    if(_device && _output)
    {
        _device->start();
        _output->start(_device);
    }
}

void AudioMixer::stop()
{
    if(_device && _output)
    {
        _output->stop();
        _device->stop();
    }
    _pos = 0;
}

void AudioMixer::addTrack(AudioTrack *t, int volume, int pan)
{
    t->setFormat(_format);
    Track track = {t, volume, pan};
    _tracks.push_back(track);
    // ToDo::connect SIGNAL and SLOT
}

void AudioMixer::removeTrack(AudioTrack *t)
{
    for(int i = 0; i < _tracks.size(); i++)
    {
        if(t == _tracks[i].t)
        {
            _tracks.remove(i);
            return;
        }
    }
}

void AudioMixer::mixTracks(qint64 length)
{
    qint64 bufferLength = length * 8 / _format.sampleSize();
    double *readBuffer = new double[bufferLength];
    double *mixedBuffer = new double[bufferLength];
    char *writeBuffer = new char[length];
    for(int i = 0; i < bufferLength; i++)
    {
        mixedBuffer[i] = 0.0;
    }
    for(int i = 0; i < _tracks.size(); i++)
    {
        qint64 sampleLength = bufferLength / _format.channels();
        _tracks[i].t->generateBuffer(_pos, readBuffer, sampleLength);
        for(int j = 0; j < sampleLength; j++)
        {
            // 後でボリューム付け足す．
            double vol = 1.0;
            int index = j * _format.channels();
            for(int k = 0; k < _format.channels(); k++, index++)
            {
                mixedBuffer[index] += readBuffer[index] * vol;
            }
        }
    }
    for(qint64 i = 0; i < bufferLength; i++)
    {
        int index = i * _format.sampleSize() / 8;
        if(mixedBuffer[i] < -1.0)
        {
            mixedBuffer[i] = -1.0;
        }
        if(mixedBuffer[i] > 1.0)
        {
            mixedBuffer[i] = 1.0;
        }
        switch(_format.sampleSize())
        {
        case 8:
            writeBuffer[index] = 127 * mixedBuffer[i];
            break;
        case 16:
            qint16 val = mixedBuffer[i] * ((1 << 15) - 1);
            qint8 *p = (qint8 *)(&val);
            writeBuffer[index] = p[0];
            writeBuffer[index + 1] = p[1];
            break;
        }
    }
    // 時間を進める
    quint64 writeLength = _device->write(writeBuffer, length);
    _pos += writeLength * 8 / _format.sampleSize() / _format.channels();
    delete[] readBuffer;
    delete[] mixedBuffer;
    delete[] writeBuffer;
}

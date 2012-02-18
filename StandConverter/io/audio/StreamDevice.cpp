#include "StreamDevice.h"

using namespace stand::io::audio;

// 最小のバッファ長．短いかも知らん．
const int StreamDevice::MINIMUM_LENGTH = 4096;

StreamDevice::StreamDevice(const QAudioFormat &format, qint64 length, QObject *parent) :
    QIODevice(parent)
{
    if(length < MINIMUM_LENGTH)
    {
        length = MINIMUM_LENGTH;
    }
    _buffer = NULL;
    _create(format, length);
}

StreamDevice::~StreamDevice()
{
    _destroy();
}

void StreamDevice::_destroy()
{
    delete[] _buffer;
    _length = 0;
    _bytesAvailable = 0;
    _pos = 0;
}

void StreamDevice::_create(const QAudioFormat &format, qint64 length)
{
    if(length <= 0)
    {
        return;
    }
    _destroy();
    _length = length * format.channels() * format.sampleSize() / 8;
    _buffer = new qint16[_length];
    _pos = 0;
    for(int i = 0; i < _length; i++)
    {
        _buffer[i] = 0;
    }
    _format = format;
}

void StreamDevice::start()
{
    open(QIODevice::ReadWrite);
}

void StreamDevice::stop()
{
    close();
    _pos = 0;
    _bytesAvailable = 0;
}

qint64 StreamDevice::readData(char *data, qint64 maxlen)
{
    if(_bytesAvailable <= _length / 4)
    {
        emit requireBuffer(_length - _bytesAvailable);
    }
    int blockSize = _format.sampleSize() / 8 * _format.channels();
    maxlen = (maxlen / blockSize) * blockSize;
    if(maxlen > _bytesAvailable)
    {
        maxlen = _bytesAvailable;
    }
    for(int i = 0; i < maxlen; i++)
    {
        data[i] = _buffer[(_pos + i) % _length];
    }

    _pos = (_pos + maxlen) % _length;
    _bytesAvailable -= maxlen;

    return maxlen;
}

qint64 StreamDevice::writeData(const char *data, qint64 len)
{
    int blockSize = _format.sampleSize() / 8 * _format.channels();
    len = (len / blockSize) * blockSize;
    if(len > (_length - _bytesAvailable))
    {
        len = (_length - _bytesAvailable);
    }
    int startAt = _pos + _bytesAvailable;
    for(int i = 0; i < len; i++)
    {
        _buffer[(startAt+ i) % _length] = data[i];
    }
    _bytesAvailable += len;
    return len;
}

qint64 StreamDevice::bytesAvailable()
{
    return _bytesAvailable;
}

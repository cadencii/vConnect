#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <QtGlobal>
#include <stdio.h>

namespace stand
{
namespace io
{
class WaveFile
{
public:
    struct Header
    {
        qint16 formatID;
        qint32 chunksize;
        qint16 formatTag;
        quint16 numChannnels;
        quint32 samplesPerSecond;
        quint32 bytesPerSecond;
        quint16 blockAlign;
        quint16 bitsPerSample;
    };

    WaveFile(unsigned int l = 0);
    WaveFile(const double *w, unsigned int l);
    WaveFile(const char* filename);
    ~WaveFile();

    bool read(const char* filename);
    bool write(const char* filename);

    void setHeader(const Header &h)
    {
        _header = h;
    }

    void setWave(const double *w, unsigned int l);

    // 原則どおり今のところは外から書けないようにしておく．
    const double *data() const
    {
        return _data;
    }

    unsigned int length() const
    {
        return _length;
    }

    bool empty() const
    {
        return (!_data || _length == 0);
    }

    const Header *header() const
    {
        return &_header;
    }

    void setLength(unsigned int l)
    {
        _createBuffer(l);
    }

    double normalize();
    const static Header DEFAULT_WAVE_FORMAT;
private:
    void _createBuffer(unsigned int l);
    void _destroy();
    bool _readHeader(FILE *fp);
    bool _readData(FILE *fp);

    void _readData8(qint8 *p);
    void _readData16(qint8 *p);
    void _readData24(qint8 *p);
    void _readData32(qint8 *p);

    void _writeData8(qint8 *p);
    void _writeData16(qint8 *p);
    void _writeData24(qint8 *p);
    void _writeData32(qint8 *p);

    Header _header;
    double *_data;
    unsigned int _length;
};

}
}


#endif // WAVEFILE_H

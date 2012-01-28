#include "WaveFile.h"

#include <math.h>
#include <errno.h>
#include <string.h>

using namespace stand::io;

const WaveFile::Header WaveFile::DEFAULT_WAVE_FORMAT =
{
    1,
    0,
    1,
    1,
    44100,
    88200,
    2,
    16,
};

WaveFile::WaveFile(const char *filename)
{
    _data = NULL;
    _length = 0;
    setHeader(DEFAULT_WAVE_FORMAT);
    read(filename);
}

WaveFile::WaveFile(const double *w, unsigned int l)
{
    _data = NULL;
    _length = 0;
    setHeader(DEFAULT_WAVE_FORMAT);
    setWave(w, l);
}

WaveFile::WaveFile(unsigned int l)
{
    _data = NULL;
    setHeader(DEFAULT_WAVE_FORMAT);
    if(l)
    {
        setLength(l);
    }
    else
    {
        _length = 0;
    }
}

WaveFile::~WaveFile()
{
    _destroy();
}

void WaveFile::_destroy()
{
    delete[] _data;
    _data = NULL;
    _length = 0;
}

bool WaveFile::read(const char *filename)
{
    qDebug("WaveFile::read(%s)", filename);
    FILE *fp = fopen(filename, "rb");
    if(!fp)
    {
        qDebug(" Could not open; %s", filename);
        qDebug(" --%s\n",strerror(errno));
        return false;
    }

    if(!_readHeader(fp))
    {
        qDebug(" Invalid header.");
        fclose(fp);
        return false;
    }

    if(!_readData(fp))
    {
        qDebug(" Invalid data.");
        fclose(fp);
        return false;
    }
    qDebug(" Succeeded reading WaveFile: %s", filename);
    fclose(fp);
    return true;
}

bool WaveFile::_readHeader(FILE *fp)
{
    qDebug("  WaveFile::readHeader()");
    qint8 buf[4];
    qint32 size;
    fread(buf, 1, 4, fp);
    if(buf[0] != 'R' || buf[1] != 'I' || buf[2] != 'F' || buf[3] != 'F')
    {
        qDebug("  This file is not written in RIFF format.");
        return false;
    }
    fread(&size, 4, 1, fp);  // File Size (we don't need this)
    fread(buf, sizeof(qint8), 4, fp);
    if(buf[0] != 'W' || buf[1] != 'A' || buf[2] != 'V' || buf[3] != 'E')
    {
        qDebug("  This file is not WAVE File.");
        return false;
    }
    fread(buf, sizeof(qint8), 4, fp);
    if(buf[0] != 'f' || buf[1] != 'm' || buf[2] != 't' || buf[3] != ' ')
    {
        qDebug("  Could not find format chunk.");
        return false;
    }

    // read header information
    fread(&size, 4, 1, fp); // Chunk Size
    fread(&_header.formatID, 2, 1, fp);
    fread(&_header.numChannnels, 2, 1, fp);
    fread(&_header.samplesPerSecond, 4, 1, fp);
    fread(&_header.bytesPerSecond, 4, 1, fp);
    fread(&_header.blockAlign, 2, 1, fp);
    fread(&_header.bitsPerSample, 2, 1, fp);
    for(size -= 16; size > 0; size--)
    {
        fgetc(fp);
    }
    if(_header.formatID != 1)
    {
        qDebug("  Unsupported Format ID; %08x", (int)(_header.formatID));
        return false;
    }
    return true;
}

bool WaveFile::_readData(FILE *fp)
{
    qDebug("  WaveFile::readData");
    int c;
    qint32 size;
    while(1)
    {
        c = fgetc(fp);
        if(c == EOF)
        {
            qDebug("  Could not find 'data' chunk.");
            return false;
        }
        if(c == 'd' && fgetc(fp) == 'a' && fgetc(fp) == 't' && fgetc(fp) == 'a')
        {
            break;
        }
    }
    fread(&size, 4, 1, fp);
    // 実際は double 型だから　INT32_MAX / sizeof(int) くらいで制限かけるべきなのか？
    if(size <= 0)
    {
        qDebug("  Invalid file size; %d[bytes]", size);
        return false;
    }
    qint8 *buf = new qint8[size];
    if(!buf)
    {
        qDebug("  Could not allocate memory");
        return false;
    }

    // read All data in the file.
    fread(buf, 1, size, fp);
    _createBuffer(size / _header.numChannnels / (_header.bitsPerSample / 8));

    // 量子化ビット数で場合わけ．
    switch(_header.bitsPerSample)
    {
    case 8:
        _readData8(buf);
        break;
    case 16:
        _readData16(buf);
        break;
    case 24:
        _readData24(buf);
        break;
    case 32:
        _readData32(buf);
        break;
    default:
        qDebug("  Unsupported bitrate; %d", _header.bitsPerSample);
        delete[] buf;
        return false;
    }

    delete[] buf;
    return true;
}

void WaveFile::_createBuffer(unsigned int l)
{
    _destroy();
    _length = l;
    _data = new double[l];
}

void WaveFile::_readData8(qint8 *p)
{
    for(unsigned int i = 0, w = 0; w < _length; i += _header.numChannnels, w++)
    {
        _data[w] = (p[i] - 128) / 128.0;
    }
}

void WaveFile::_readData16(qint8 *p)
{
    qint16 *p16 = (qint16 *)p;
    for(unsigned int i = 0, w = 0; w < _length; i += _header.numChannnels, w++)
    {
        _data[w] = p16[i] / (double)(2 << 15);
    }
}

void WaveFile::_readData24(qint8 *p)
{
    for(unsigned int i = 0, w = 0; w < _length; i += _header.numChannnels * 3, w++)
    {
        qint32 val = (p[i] << 8) + (p[i + 1] << 16) + (p[i + 2] << 24);
        _data[w] = val / (double)(2 << 23);
    }
}

void WaveFile::_readData32(qint8 *p)
{
    qint32 *p32 = (qint32 *)p;
    for(unsigned int i = 0, w = 0; w < _length; i += _header.numChannnels, w++)
    {
        _data[w] = p32[i] / (double)(2 << 31);
    }
}

void WaveFile::setWave(const double *w, unsigned int l)
{
    if(!w || !l)
    {
        qDebug("WaveFile::setWave(%08x, %ud); // invalid args", w, l);
        return;
    }
    _createBuffer(l);
    for(unsigned int i = 0; i < l; i++)
    {
        _data[i] = w[i];
    }
}

bool WaveFile::write(const char *filename)
{
    qDebug("WaveFile::write(%s)", filename);
    if(empty())
    {
        qDebug(" This WaveFile class does not contain any data.");
        return false;
    }
    FILE *fp = fopen(filename, "wb");
    if(!fp)
    {
        qDebug(" Could not open File; %s", filename);
        return false;
    }
    qint32 waveSize = _length * (_header.bitsPerSample / 8);
    qint8 *buf = new qint8[waveSize];
    qint32 size = 44 + waveSize;
    _header.chunksize = 16;

    switch(_header.bitsPerSample)
    {
    case 8:
        _writeData8(buf);
        break;
    case 24:
        _writeData24(buf);
        break;
    case 32:
        _writeData32(buf);
        break;
    default:
        qDebug(" Unsupported bitrate; %d", _header.bitsPerSample);
        _header.bitsPerSample = 16;
    case 16:
        _writeData16(buf);
    }

    fprintf(fp, "RIFF");
    fwrite(&size, 4, 1, fp);

    fprintf(fp, "WAVEfmt ");
    fwrite(&_header.chunksize, 4, 1, fp);
    fwrite(&_header.formatID, 2, 1, fp);
    fwrite(&_header.numChannnels, 2, 1, fp);
    fwrite(&_header.samplesPerSecond, 4, 1, fp);
    fwrite(&_header.bytesPerSecond, 4, 1, fp);
    fwrite(&_header.blockAlign, 2, 1, fp);
    fwrite(&_header.bitsPerSample, 2, 1, fp);

    fprintf(fp, "data");
    fwrite(&waveSize, 4, 1, fp);
    fwrite(buf, sizeof(qint8), waveSize, fp);

    fclose(fp);
    delete[] buf;

    return true;
}

void WaveFile::_writeData8(qint8 *p)
{
    for(unsigned int i = 0; i < _length; i++)
    {
        p[i] = (qint8)(_data[i] * 127.0) + 127;
    }
}

void WaveFile::_writeData16(qint8 *p)
{
    qint16 *p16 = (qint16 *)p;
    for(unsigned int i = 0; i < _length; i++)
    {
        p16[i] = _data[i] * ((2 << 15) - 1);
    }
}

void WaveFile::_writeData24(qint8 *p)
{
    for(unsigned int i = 0; i < _length; i++)
    {
        qint32 val = _data[i] * ((2 << 23) - 1);
        qint8 *v = (qint8 *)(&val);
        p[i*3] = v[1];
        p[i*3+1] = v[2];
        p[i*3+2] = v[3];
    }
}

void WaveFile::_writeData32(qint8 *p)
{
    qint32 *p32 = (qint32 *)p;
    for(unsigned int i = 0; i < _length; i++)
    {
        p32[i] = _data[i] * ((2 << 31) - 1);
    }
}

double WaveFile::normalize()
{
    if(!_data)
    {
        return 0.0;
    }
    double maxVal = 0.0;
    for(int i = 0; i < _length; i++)
    {
        double val = fabs(_data[i]);
        if(val > maxVal)
        {
            maxVal = val;
        }
    }
    if(maxVal == 0.0)
    {
        return 0.0;
    }
    for(int i = 0; i < _length; i++)
    {
        _data[i] /= maxVal;
    }
    return maxVal;
}

#include "SpecgramImage.h"

#include <float.h>
#include <math.h>

using namespace stand::io;

SpecgramImage::SpecgramImage()
{
    _tLen = _fftl = _fs = 0;
}

SpecgramImage::SpecgramImage(double **specgram, int tLen, int fftl, int fs)
{
    set(specgram, tLen, fftl, fs);
}

void SpecgramImage::set(double **specgram, int tLen, int fftl, int fs)
{
    if(!specgram || tLen <= 0 || fftl <= 0 || fs <= 0)
    {
        return;
    }
    _image = SpecgramImage::imageFromSpecgram(specgram, tLen, fftl);
    _tLen = tLen;
    _fftl = fftl;
    _fs = fs;
}


const QImage &SpecgramImage::image()
{
    return _image;
}

QImage SpecgramImage::imageFromSpecgram(double **specgram, int tLen, int fftl)
{
    if(!specgram)
    {
        qDebug("SpecgramPixmap::setSpecgram(%d, %d %d); // Invalid args", specgram, tLen, fftl);
        return QImage();
    }

    QImage image(tLen, fftl, QImage::Format_ARGB32);

    // 最小の値と最大の値を見つけてくる．
    double maxValue = -DBL_MAX;
    for(int x = 0; x < tLen; x++)
    {
        for(int y = 0; y < fftl; y++)
        {
            maxValue = (specgram[x][y] > maxValue) ? specgram[x][y] : maxValue;
        }
    }

    // 45 dB決め打ち．でいいのかな．
    // 点描するよ！
    for(int x = 0; x < tLen; x++)
    {
        for(int y = 0; y < fftl; y++)
        {
            double dB = log10(specgram[x][y]) - log10(maxValue);
            int r, g, b, c = (dB + 10) / 10.0 * 255.0 * 3;
            r = (c >= 512) ? (c - 512) : 0;
            g = (c >= 512) ? 255 : (c >= 256) ? (c - 256) : 0;
            b = (c >= 256) ? 255 : (c >= 0) ? c : 0;
            image.setPixel(x, fftl - 1 - y, qRgb(r, g, b));
        }
    }
    return image;
}

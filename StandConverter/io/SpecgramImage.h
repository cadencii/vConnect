#ifndef SPECGRAMIMAGE_H
#define SPECGRAMIMAGE_H

#include <QImage>

namespace stand
{
namespace io
{

class SpecgramImage
{
public:
    explicit SpecgramImage();
    explicit SpecgramImage(double **specgram, int tLen, int fftl, int fs);

    void set(double **specgram, int tLen, int fftl, int fs);

    const QImage &image();

    static QImage imageFromSpecgram(double **specgram, int tLen, int fftl);
private:
    int _tLen;
    int _fftl;
    int _fs;
    QImage _image;
};

}
}

#endif // SPECGRAMIMAGE_H

#ifndef SPECGRAMVIEWER_H
#define SPECGRAMVIEWER_H

#include <QWidget>

class QImage;

namespace stand
{
namespace gui
{

class SpecgramViewer : public QWidget
{
    Q_OBJECT
public:
    explicit SpecgramViewer(QWidget *parent = NULL);
    ~SpecgramViewer();

    void setSpecgram(double **specgram, int tLen, int fftl);
    //void paintEvent(QPaintEvent *e);
signals:

public slots:
    //void specgramDeleted();

private:
    void _destroy();
    QImage *_image;
};

}
}

#endif // SPECGRAMVIEWER_H

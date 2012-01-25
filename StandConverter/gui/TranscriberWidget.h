#ifndef TRANSCRIBERWIDGET_H
#define TRANSCRIBERWIDGET_H

#include <QWidget>

class QTextCodec;

namespace Ui {
class TranscriberWidget;
}

namespace stand
{
namespace gui
{

class TranscriberWidget : public QWidget
{
    Q_OBJECT
public slots:
    void openDirDialog();

public:
    explicit TranscriberWidget(QWidget *parent = 0, int index = 0ULL);
    ~TranscriberWidget();

    QString dir();
    int bri();
    int note();
    QTextCodec *codec();

private:
    Ui::TranscriberWidget *ui;
};

}
}

#endif // TRANSCRIBERWIDGET_H

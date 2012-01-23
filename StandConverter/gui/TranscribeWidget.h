#ifndef TRANSCRIBEWIDGET_H
#define TRANSCRIBEWIDGET_H

#include <QWidget>

namespace Ui {
class TranscribeWidget;
}

namespace stand
{
namespace gui
{

class TranscribeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranscribeWidget(QWidget *parent = 0);
    ~TranscribeWidget();

    QString dir();
    int bri();
    int note();

private:
    Ui::TranscribeWidget *ui;
};

}
}

#endif // TRANSCRIBEWIDGET_H

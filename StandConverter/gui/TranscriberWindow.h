#ifndef TRANSCRIBERWINDOW_H
#define TRANSCRIBERWINDOW_H

#include <QMainWindow>

namespace Ui {
class TranscriberWindow;
}

namespace stand
{
namespace synthesis
{
class Transcriber;
class TranscriberSetting;
}
namespace gui
{

class TranscriberWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void sendCancelToTranscriber();
public slots:
    void addTab();
    void removeTab();

    void pushAnalyze();
    void transcriptionFinished(bool);
public:
    explicit TranscriberWindow(QWidget *parent = 0);
    ~TranscriberWindow();

    void closeEvent(QCloseEvent *e);

private:
    void _beginAnalyze();
    void _cancelAnalyze();
    void _setItemEnabled();

    bool _createSetting(stand::synthesis::TranscriberSetting &s);

    Ui::TranscriberWindow *ui;
    stand::synthesis::Transcriber *current;

    bool isAnalyzing;
};

}
}

#endif // TRANSCRIBERWINDOW_H

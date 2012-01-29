#ifndef TRANSCRIBERWINDOW_H
#define TRANSCRIBERWINDOW_H

#include <QMainWindow>
#include <QVector>

#include "MappingView.h"

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
    void mappingChanged(QVector<stand::gui::MappingView::Item> &mapping);
public slots:
    void addTab();
    void removeTab();

    void pushAnalyze();
    void pushRootDir();
    void pushIcon();
    void settingChanged();
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
    bool _checkSettingAvailability();

    Ui::TranscriberWindow *ui;
    stand::synthesis::Transcriber *current;

    bool isAnalyzing;
};

}
}

#endif // TRANSCRIBERWINDOW_H

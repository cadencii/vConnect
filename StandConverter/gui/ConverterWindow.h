/*!
 * @file ConverterWindow.h
 * @author HAL
 */
#ifndef CONVERTERWINDOW_H
#define CONVERTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class ConverterWindow;
}

namespace stand
{
namespace synthesis
{
class ConverterSetting;
class Converter;
}
namespace gui
{

/// <summary>
/// UTAU 音源→ v.Connect 音源変換ツールウィンドウ
/// </summary>
class ConverterWindow : public QMainWindow
{
    Q_OBJECT

signals:
    /// <summary> 変換器にキャンセルを通知します． </summary>
    void sendCancelToConverter();

public:
    explicit ConverterWindow(QWidget *parent = 0, Qt::WindowFlags f = Qt::Window);
    ~ConverterWindow();

    /// <summary> オーバーライドされた closeEvent ．ユーザが Window を閉じようとしたときに呼ばれます． </summary>
    void closeEvent(QCloseEvent *e);

    /// <summary> 編集可能か不可能かを設定します． </summary>
    /// <param name = "e"> trueで編集可能，falseで編集不可能にします． </param>
    /// <returns> 無し． </returns>
    void setEditorEnabled(bool e);

    /// <summary> 変換を開始します． </summary>
    void beginConvert();
    /// <summary> 変換を中断します． </summary>
    void cancelConvert();

public slots:
    /// <summary> 正規化オプションに変更が生じた際に呼ばれるスロット． </summary>
    void normalizeEnabledChanged();

    /// <summary> 分析元フォルダ検索ボタンが押されたときに呼ばれるスロット． </summary>
    void sourceFinderClicked();

    /// <summary> 保存フォルダ検索ボタンが押されたときに呼ばれるスロット． </summary>
    void destFinderClicked();

    /// <summary> 変換ボタンが押された際に呼ばれるスロット． </summary>
    void convertButtonClicked();

    /// <summary> 変換が終了した際に呼ばれるスロット． </summary>
    void converterFinished(bool f);

private:
    bool _setConverterSetting(stand::synthesis::ConverterSetting &setting);
    void _errorMessage(const QString &text);

    Ui::ConverterWindow *ui;
    stand::synthesis::Converter *currentConverter;

    bool isAnalyzing; /// <summary> 今分析中かどうかフラグ． </summary>
};
}
}

#endif // CONVERTERWINDOW_H

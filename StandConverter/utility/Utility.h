#ifndef UTILITY_H
#define UTILITY_H

#include <QDir>

class QWidget;

namespace stand
{
namespace utility
{
/// <summay> ディレクトリを作成します．作成したいディレクトリがある場合は何もしません． </summary>
/// <param name = "w"> エラー時，メッセージを出すウィジェット． </param>
/// <param name = "dir"> 作成したいディレクトリパス． </param>
/// <param name = "confirm"> 指定したパスが存在しない場合作成の許可をユーザに求めるかどうかのフラグ． </param>
/// <returns> true: 作成が成功したか，ディレクトリが存在した． false: ディレクトリが存在せず作成もできなかった． </returns>
bool makeDirectory(QWidget *w, const QDir &dir, bool confirm = false);

/// <summary> UTAU 音源の原音設定からサンプル数を計算します． </summary>
/// <param "waveLength"> 波形全体の長さ． </param>
/// <param "samplingFrequency"> 標本化周波数． </param>
/// <param "leftBlank"> 原音設定の左ブランク． </param>
/// <param "rightBlank"> 原音設定の右ブランク． </param>
/// <return> 原音設定に対応する信号のサンプル数． </return>
int lengthForUtauSetting(int waveLength, int samplingFrequency, float leftBlank, float rightBlank);

/// <summary> 信号波形を端点で正規化します． </summary>
/// <param "x"> 正規化する信号列． </param>
/// <param "length"> 信号長． </param>
void normalizeByAnchor(double *x, unsigned int length);

}
}

#endif // UTILITY_H

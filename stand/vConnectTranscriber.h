#ifndef __vConnectTranscriber_h__
#define __vConnectTranscriber_h__

#include "stand.h"
#include <time.h>

class vConnectPhoneme;

class vConnectTranscriber
{
public:
    /// <summary>
    /// 与えられた二つの UTAU 原音設定から時間伸縮関数を生成します．
    /// </summary>
    /// <param name="src_path">転写元ディレクトリパス．</param>
    /// <param name="dst_path">転写先ディレクトリパス．</param>
    /// <returns>転写に成功したときは true，失敗時は false を返します．</returns>
    static bool transcribe(string &src_path, string &dst_path, const char *codepage);
private:

    static void _transcribe_compressed(vConnectPhoneme *src, vConnectPhoneme *dst);
    static void _transcribe_raw(vConnectPhoneme *src, vConnectPhoneme *dst);

    static void _calculate_compressed_env(double *dst, vConnectPhoneme *src, int length);
};

#endif


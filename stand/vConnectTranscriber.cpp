#include "vConnectTranscriber.h"

#include "mb_text.h"
#include "utauVoiceDB/UtauDB.h"
#include "vConnectPhoneme.h"


// 転写システム
bool vConnectTranscriber::transcribe(string_t &src_path, string_t &dst_path, const char *codepage)
{
    bool ret = false;
    UtauDB src, dst;

    src.read(src_path, codepage);
    dst.read(dst_path, codepage);
    map_t<string_t, int> analyzedItems;

    for(int i = 0; i < src.size(); i++)
    {
        string s;      // 表示用
        utauParameters src_param, dst_param;
        src.getParams(src_param, i);
        mb_conv(src_param.lyric, s);
        cout << "Begin analysis @ " << s.c_str() << endl;

        map_t<string_t, int>::iterator itr = analyzedItems.find(src_param.fileName);
        if(itr == analyzedItems.end())
        {
            cout << " Already analyzed." << endl;
            continue;
        }

        if(dst.getParams(dst_param, src_param.lyric) != 1)
        {
            continue;
        }
        if(dst_param.isWave != src_param.isWave)
        {
            mb_conv(src_param.fileName, s);
            cout << "error; conflting format in " << s.c_str() << endl;
            continue;
        }

        analyzedItems.insert(make_pair(src_param.fileName, i));
    }
    return ret;
}

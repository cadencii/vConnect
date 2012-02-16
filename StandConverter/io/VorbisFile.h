#ifndef VORBISFILE_H
#define VORBISFILE_H

#include <QtGlobal>
#include <vorbis/vorbisfile.h>

namespace stand
{
namespace io
{

struct VorbisBuffer;

class VorbisFile
{
public:
    VorbisFile();
    virtual ~VorbisFile();

    /// <summary>
    /// 今保持している音素片の励起信号 Ogg ストリームを開きます．
    /// </summary>
    /// <param name="ovf">開きたい Ogg ストリーム．</param>
    /// <returns>成功したときは true，失敗したときは false を返します．</returns>
    bool open(OggVorbis_File *ovf);

    static void close(OggVorbis_File *ovf);

protected:

    /// <summary>波形を OggVorbis 形式へエンコードします．</summary>
    /// <param name="x">エンコードする波形．</param>
    /// <param name="xLen">波形長．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="minBitrate"> 最小ビットレートの指定 </param>
    /// <param name="maxBitrate"> 最大ビットレートの指定 </param>
    /// <param name="averageBitrate"> 平均ビットレートの指定 </param>
    void encodeToVorbis(const double *x, int xLen, int fs, int minBitrate, int maxBitrate, int averageBitrate);

    /// <summary>波形を OggVorbis 形式へエンコードします．バッファはnewで確保されます．</summary>
    /// <param name="dst">OggVorbisバッファが返ります．</param>
    /// <param name="size">バッファサイズが返ります．</param>
    /// <param name="wave">エンコードする波形．</param>
    /// <param name="length">波形長．</param>
    /// <param name="fs">標本化周波数．</param>
    /// <param name="minBitrate"> 最小ビットレートの指定 </param>
    /// <param name="maxBitrate"> 最大ビットレートの指定 </param>
    /// <param name="averageBitrate"> 平均ビットレートの指定 </param>
    static void encodeToVorbis(
            char **dst,
            int *size,
            const double *wave,
            int length,
            int fs,
            int minBitrate,
            int maxBitrate,
            int averageBitrate
            );

    struct VorbisContent
    {
        char *data;
        qint32 size;
    };
    VorbisContent vorbis;

private:

    struct _VFILE
    {
        char *p;
        int pos;
        int size;
    };

    const static int bufSize;
    const static char ENCODER_TAG[];

    static size_t _read(void *dst, size_t size, size_t maxCount, void *vp);
    static int _seek(void *vp, ogg_int64_t offset, int flag);
    static int _close(void *vp);
    static long _tell(void *vp);
};

}
}

#endif // VORBISFILE_H

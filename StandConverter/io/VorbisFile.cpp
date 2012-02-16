#include "VorbisFile.h"

#include <list>
#include <math.h>
#include <time.h>
#include <string.h>

#include <vorbis/vorbisenc.h>

using namespace stand::io;

// OggVorbis のエンコードに必要な定数群
const char VorbisFile::ENCODER_TAG[] = "Stand Library";
const int VorbisFile::bufSize = 4096;

// VorbisFile の操作に必要な構造体
struct stand::io::VorbisBuffer
{
    char *p;
    int size;
};

size_t VorbisFile::_read(void *dst, size_t size, size_t maxCount, void *vp)
{
    _VFILE *p = (_VFILE*)vp;

    size_t ret = (p->pos + maxCount * size >= p->size) ? p->size - p->pos : maxCount * size;
    memcpy(dst, p->p + p->pos, ret);
    p->pos += ret;

    return ret / size;
}

int VorbisFile::_seek(void *vp, ogg_int64_t offset, int flag)
{
    _VFILE *p = (_VFILE*)vp;

    switch(flag) {
        case SEEK_CUR:
            p->pos += offset;
            break;
        case SEEK_END:
            p->pos = p->size + offset;
            break;
        case SEEK_SET:
            p->pos = offset;
            break;
        default:
            return -1;
    }

    if(p->pos > p->size) {
        p->pos = p->size;
        return -1;
    } else if(p->pos < 0) {
        p->pos = 0;
        return -1;
    }

    return 0;
}

long VorbisFile::_tell(void *vp)
{
    return ((_VFILE*)vp)->pos;
}

int VorbisFile::_close(void *vp)
{
    _VFILE *p = (_VFILE *)vp;
    delete p;
    return 0;
}

VorbisFile::VorbisFile()
{
    vorbis.data = NULL;
    vorbis.size = 0;
}

VorbisFile::~VorbisFile()
{
    delete vorbis.data;
}

bool VorbisFile::open(OggVorbis_File *ovf)
{
    ov_callbacks callbacks = {
        _read,
        _seek,
        _close,
        _tell
    };
    _VFILE *fp = new _VFILE;
    fp->p = vorbis.data;
    fp->size = vorbis.size;
    fp->pos = 0;

    if(ov_open_callbacks(fp, ovf, 0, 0, callbacks) != 0)
    {
        delete fp;
        return false;
    }
    return true;
}

void VorbisFile::close(OggVorbis_File *ovf)
{
    ov_clear(ovf);
}

void VorbisFile::encodeToVorbis(const double *x, int xLen, int fs, int minBitrate, int maxBitrate, int averageBitrate)
{
    encodeToVorbis(&vorbis.data, &vorbis.size, x, xLen, fs, minBitrate, maxBitrate, averageBitrate);
}

void VorbisFile::encodeToVorbis(
        char **dst,
        int *size,
        const double *wave,
        int length,
        int fs,
        int minBitrate,
        int maxBitrate,
        int averageBitrate
        )
{
    if(!dst || !size || !wave || length <= 0){ return; }

    std::list<VorbisBuffer*> bufferList;
    VorbisBuffer *p;
    int eos = 0;
    ogg_stream_state streamState;
    ogg_page         page;
    ogg_packet       packet, header, headerComment, headerCode;

    vorbis_info      info;
    vorbis_comment   comment;

    vorbis_dsp_state dspState;
    vorbis_block     block;

    vorbis_info_init(&info);

    int retval;
    if(retval = vorbis_encode_init(&info, 1, fs, maxBitrate, averageBitrate, minBitrate))
    {
        vorbis_info_clear(&info);
        return;
    }

    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "ENCODER", (char *)ENCODER_TAG);

    vorbis_analysis_init(&dspState, &info);
    vorbis_block_init(&dspState, &block);

    srand(time(NULL));
    ogg_stream_init(&streamState, rand());

    vorbis_analysis_headerout(&dspState, &comment, &header, &headerComment, &headerCode);
    ogg_stream_packetin(&streamState, &header);
    ogg_stream_packetin(&streamState, &headerComment);
    ogg_stream_packetin(&streamState, &headerCode);

    while(!eos){
        int bytes = ogg_stream_flush(&streamState, &page);
        if(bytes == 0){ break; }

        p = new VorbisBuffer;
        p->size = page.header_len + page.body_len;
        p->p = new char[p->size];
        memcpy(p->p, page.header, page.header_len);
        memcpy(p->p + page.header_len, page.body, page.body_len);
        bufferList.push_back(p);
    }

    long i, pos = 0;
    while(!eos){
        float **buffer = vorbis_analysis_buffer(&dspState, bufSize);
        for(i = 0; i < bufSize && pos < length; i++, pos++){
            buffer[0][i] = wave[pos];
        }
        vorbis_analysis_wrote(&dspState, i);

        while(vorbis_analysis_blockout(&dspState, &block) == 1){
            vorbis_analysis(&block, NULL);
            vorbis_bitrate_addblock(&block);

            while(vorbis_bitrate_flushpacket(&dspState, &packet)){
                ogg_stream_packetin(&streamState, &packet);

                while(!eos){
                    int result = ogg_stream_pageout(&streamState, &page);
                    if(result == 0)break;

                    p = new VorbisBuffer;
                    p->size = page.header_len + page.body_len;
                    p->p = new char[p->size];
                    memcpy(p->p, page.header, page.header_len);
                    memcpy(p->p + page.header_len, page.body, page.body_len);
                    bufferList.push_back(p);

                    if(ogg_page_eos(&page)){
                        eos = 1;
                    }
                }
            }
        }
    }

    ogg_stream_clear(&streamState);
    vorbis_block_clear(&block);
    vorbis_dsp_clear(&dspState);
    vorbis_comment_clear(&comment);
    vorbis_info_clear(&info);

    *size = 0;
    for(std::list<VorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        *size += (*i)->size;
    }
    *dst = new char[*size];
    char *cur = *dst;
    for(std::list<VorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        // Endian 依存．
        memcpy(cur, (*i)->p, (*i)->size);
        cur += (*i)->size;
        delete[] (*i)->p;
        delete (*i);
    }
}

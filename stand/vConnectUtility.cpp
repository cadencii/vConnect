#include "vConnectUtility.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisenc.h>

#include <list>
using namespace std;

const int maxBitrate = -1;
const int minBitrate = -1;
const int averageBitrate = 80000;

const int bufSize = 4096;

#define ENCODER_TAG "vConnet"

struct vorbisBuffer {
    char *p;
    int size;
};

void vConnectUtility::extractMelCepstrum(double *dst, int fftl, float *src, int cepl, fftw_complex *in, double *out, fftw_plan plan, int fs)
{
    int i;
    for(i = 0; i < cepl; i++)
    {
        in[i][0] = src[i];
        in[i][1] = 0.0;
    }
    for(; i <= fftl / 2; i++)
    {
        in[i][0] = in[i][1] = 0.0;
    }

    fftw_execute(plan);

    for(i = 0; i <= fftl / 2; i++)
    {
        out[i] = exp(out[i]);
    }

    vConnectUtility::stretchFromMelScale(dst, out, fftl / 2 + 1, fs / 2);
}

void vConnectUtility::stretchToMelScale(double *melSpectrum, const double *spectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getFrequency((double)i / (double)spectrumLength * tmp) / (double)maxFrequency * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            melSpectrum[i] = interpolateArray(dIndex, spectrum);
        }else{
            melSpectrum[i] = spectrum[spectrumLength - 1];
        }
    }
}

void vConnectUtility::stretchFromMelScale(double *spectrum, const double *melSpectrum, int spectrumLength, int maxFrequency)
{
    double tmp = getMelScale(maxFrequency);
    for(int i = 0; i < spectrumLength; i++)
    {
        double dIndex = getMelScale((double)i / (double)spectrumLength * (double)maxFrequency) / tmp * (double)spectrumLength;
        if(dIndex <= spectrumLength-1.0){
            spectrum[i] = interpolateArray(dIndex, melSpectrum);
        }else{
            spectrum[i] = melSpectrum[spectrumLength - 1];
        }
    }
}

double vConnectUtility::interpolateArray( double x, const double *p )
{
    int t = (int)x;
    double r = x - (double)t;
    return ( p[t] * ( 1.0 - r ) + p[t+1] * r );
}

double  vConnectUtility::getMelScale(double freq){
    double ret = 1127.01048 * log(1 + freq / 700);
    return ret;
}

double  vConnectUtility::getFrequency(double melScale){
    double ret = 700 * (exp(melScale / 1127.01048) - 1);
    return ret;
}

void vConnectUtility::extractResidual(fftw_complex *dst, const double *src, int fftLength)
{
    dst[0][0] = src[0];
    dst[0][1] = 0.0;
    for(int i = 1; i < fftLength / 2; i++) {
        dst[i][0] = src[2*i-1];
        dst[i][1] = src[2*i];
    }
    dst[fftLength/2][0] = src[fftLength-1];
    dst[fftLength/2][1] = 0.0;
}

void vConnectUtility::newOggVorbis(char **dst, int *size, const double *wave, int length, int fs)
{
    if(!dst || !size || !wave || length <= 0){ return; }

    list<vorbisBuffer*> bufferList;
    vorbisBuffer *p;
    int eos = 0;
    ogg_stream_state streamState;
    ogg_page         page;
    ogg_packet       packet, header, headerComment, headerCode;

    vorbis_info      info;
    vorbis_comment   comment;

    vorbis_dsp_state dspState;
    vorbis_block     block;

    vorbis_info_init(&info);

    if(vorbis_encode_init(&info, 1, fs, maxBitrate, averageBitrate, minBitrate))
    {
        vorbis_info_clear(&info);
        return;
    }

    vorbis_comment_init(&comment);
    vorbis_comment_add_tag(&comment, "ENCODER", ENCODER_TAG);

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

        p = new vorbisBuffer;
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

                    p = new vorbisBuffer;
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
    for(list<vorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        *size += (*i)->size;
    }
    *dst = new char[*size];
    char *cur = *dst;
    for(list<vorbisBuffer*>::iterator i = bufferList.begin(); i != bufferList.end(); i++) {
        // Endian 依存．
        memcpy(cur, (*i)->p, (*i)->size);
        cur += (*i)->size;
        delete[] (*i)->p;
        delete (*i);
    }
}

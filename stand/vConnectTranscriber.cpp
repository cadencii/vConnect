#include "vConnectTranscriber.h"

#include "mb_text.h"
#include "utauVoiceDB/UtauDB.h"
#include "vConnectPhoneme.h"
#include "vConnectUtility.h"


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
        cout << "Begin analysis : " << s.c_str() << endl;

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
            cout << " error; conflting format : " << s.c_str() << endl;
            continue;
        }

        if(dst_param.isWave == false)
        {
            // 圧縮形式のマッチング．
            vConnectPhoneme src_phoneme, dst_phoneme;

            string_t tmp_path;
            src.getDBPath(tmp_path);
            tmp_path += src_param.fileName;
            mb_conv(tmp_path, s);
            if(!src_phoneme.readPhoneme(s.c_str()))
            {
                cout << " error; can't read file : " << s.c_str() << endl;
                continue;
            }

            dst.getDBPath(tmp_path);
            tmp_path += dst_param.fileName;
            mb_conv(tmp_path, s);
            if(!dst_phoneme.readPhoneme(s.c_str()))
            {
                cout << " error; can't read file : " << s.c_str() << endl;
                continue;
            }

            _transcribe_compressed(&src_phoneme, &dst_phoneme);

            src.getDBPath(tmp_path);
            tmp_path += src_param.fileName;
            mb_conv(tmp_path, s);
            src_phoneme.writePhoneme(s.c_str());

            dst.getDBPath(tmp_path);
            tmp_path += dst_param.fileName;
            mb_conv(tmp_path, s);
            dst_phoneme.writePhoneme(s.c_str());
        }
        else
        {
            vConnectPhoneme src_phoneme, dst_phoneme;
            /* ここは生波形用 */
        }

        analyzedItems.insert(make_pair(src_param.fileName, i));
    }
    return ret;
}

void vConnectTranscriber::_transcribe_compressed(vConnectPhoneme *src, vConnectPhoneme *dst)
{
    int src_len, dst_len;
    double *src_env, *dst_env;
    double *src_to_dst, *dst_to_src;

    src_len = src->getTimeLength();
    dst_len = dst->getTimeLength();

    src_env = new double[src_len];
    dst_env = new double[dst_len];
    src_to_dst = new double[src_len];
    dst_to_src = new double[dst_len];

    _calculate_compressed_env(src_env, src, src_len);
    _calculate_compressed_env(dst_env, dst, dst_len);

    vConnectUtility::calculateMatching(dst_to_src, src_to_dst, src_env, dst_env, src_len);

    src->setTimeFunction(src_to_dst, src_len);
    dst->setTimeFunction(dst_to_src, dst_len);

    delete[] src_to_dst;
    delete[] dst_to_src;
    delete[] src_env;
    delete[] dst_env;
}

void vConnectTranscriber::_calculate_compressed_env(double *dst, vConnectPhoneme *src, int length)
{
    int fftl = 2048;
    fftw_complex *in = new fftw_complex[fftl];
    double      *out = new double[fftl];
    double      *pow_spec = new double[fftl];

    double      *res_wave = new double[fftl];
    fftw_complex *res_spec = new fftw_complex[fftl];
    fftw_complex *spectrum = new fftw_complex[fftl];
    fftw_complex *cepstrum = new fftw_complex[fftl];

    fftw_plan res_forward = fftw_plan_dft_r2c_1d(fftl, res_wave, res_spec, FFTW_ESTIMATE);
    fftw_plan forward = fftw_plan_dft_1d(fftl, spectrum, cepstrum, FFTW_FORWARD,  FFTW_ESTIMATE);
    fftw_plan inverse = fftw_plan_dft_1d(fftl, cepstrum, spectrum, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_plan inverse_c2r = fftw_plan_dft_c2r_1d(fftl, spectrum, out, FFTW_ESTIMATE);

    OggVorbis_File ovf;
    src->vorbisOpen(&ovf);
    float **pcm_channels;

    for(int i = 0; i < length; i++)
    {
        int mel_len;
        int c;
        float *mel_cep;
        double sum = 0.0;

        // ケプストラムからパワースペクトルを計算．
        mel_cep = src->getMelCepstrum(i, &mel_len);
        vConnectUtility::extractMelCepstrum(pow_spec, fftl, mel_cep, mel_len, in, out, inverse, fs);
        getMinimumPhaseSpectrum(pow_spec, spectrum, cepstrum, fftl, forward, inverse);

        // Ogg ストリームから残差波形をデコード．
        for(c = 0; c < fftl; )
        {
            int bitStream;
            long samples = ov_read_float(&ovf, &pcm_channels, fftl - c, &bitStream);
            if(samples <= 0)
            {
                break;
            }
            for(int j = 0, k = c; j < samples && k < fftl; j++, k++)
            {
                res_wave[k] = pcm_channels[0][j];
            }
            c += samples;
        }

        // 残差スペクトルの計算．
        fftw_execute(res_forward);

        for(int k = 0; k <= fftl / 2; k++)
        {
            double real = spectrum[k][0] * res_spec[k][0] - spectrum[k][1] * res_spec[k][1];
            double imag = spectrum[k][1] * res_spec[k][0] + spectrum[k][0] * res_spec[k][1];
            spectrum[k][0] = real;
            spectrum[k][1] = imag;
        }

        fftw_execute(inverse_c2r);

        for(int j = 0; j < fftl; j++)
        {
            sum += out[j] * out[j];
        }
        dst[i] = sum;
    }

    fftw_destroy_plan(inverse_c2r);
    fftw_destroy_plan(forward);
    fftw_destroy_plan(inverse);
    fftw_destroy_plan(res_forward);

    src->vorbisClose(&ovf);
    delete[] spectrum;
    delete[] cepstrum;
    delete[] res_spec;
    delete[] res_wave;
    delete[] pow_spec;
    delete[] in;
    delete[] out;
}


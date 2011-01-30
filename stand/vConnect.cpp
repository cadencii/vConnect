/*
 *
 *    vConnect.cpp
 *                        (c) HAL 2010-
 *
 *  This files is a part of v.Connect.
 * vConnect class is a main class that connects UTAU and WORLD.
 * It is consisted of spectral, pitch and dynamics calculation.
 *
 * These files are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#include "vConnect.h"
#include "stand.h"
#include <time.h>

#define TRANS_MAX 4096
double temporary1[TRANS_MAX];
double temporary2[TRANS_MAX];
double temporary3[TRANS_MAX];

double vConnect::noteFrequency[NOTE_NUM];
double vConnect::vibrato[VIB_NUM];

__stnd_thread_start_retval __stnd_declspec calculateSpecgram( void *arg );

struct vConnectArg{
    long beginFrame;
    long frameLength;
    long offset;
    int  fftLength;
    int  aperiodicityLength;
    vsqFileEx *vsq;
    standSpecgram *specgram;
    vector<double> *dynamics;
    vector<vector<standBP> > *controlCurves;
    runtimeOptions *options;
    vector<corpusManager*> *managers;
};

double    vConnect::getPitchFluctuation( double second )
{
    double result = 1.0 + ( sin( 12.7 * ST_PI * second ) + sin ( 7.1 * ST_PI * second ) + sin( 4.7 * ST_PI * second ) / 3.0 ) / 300.0;

    return result;
}

bool vConnect::createWspFile( string_t v_path, string_t output, string_t alias, runtimeOptions options )
{
    utauVoiceDataBase utauDB;
    utauParameters parameters;
    standData *data;
    standFrame frame;

    // 強制的に platinum 使用とする．
    options.fast = true;

    // 初期化
    if( utauDB.readUtauVoiceDataBase( v_path, options.encodingOtoIni.c_str() ) != 1 )
        return false;

    utauDB.getUtauParameters( parameters, alias );

    // 読み込み
    manager.setVoiceDB( &utauDB, options );
    data = manager.getStandData( alias, options );

    if( data == NULL ) // 失敗
        return false;

    // 成功
    double *spectrum = new double[2048];
    double *aperiodicity = new double[4];
    double f0;
    int index, begin, end, c = 0;

    memset( spectrum, 0, sizeof(double) * 2048 );
    memset( aperiodicity, 0, sizeof(double) * 4 );
    // 固定長以下 100 フレームが代表点．
    begin = (int)( parameters.msFixedLength / framePeriod );
    end = begin + 100;
    if( end >= data->specgram->getTimeLength() )
        end = data->specgram->getTimeLength() - 1;

    double w = (double)( end - begin );
    f0 = 0.0;

    for( index = begin; index < end; index++ ){
        data->specgram->getFramePointer( index, frame );
        if( !( *frame.f0 ) )
            continue;
        c++;
        for( int i = 0; i < 1024; i++ )
            spectrum[i] += frame.spectrum[i];
        for( int i = 0; i < 4; i ++ )
            aperiodicity[i] += frame.aperiodicity[i];
        f0 += *( frame.f0 ) / w;
    }
    for( int i = 0; i < 1024; i++ )
        spectrum[i] /= (double)c;
    for( int i = 0; i < 4; i++ )
        aperiodicity[i] /= (double)c;
    for( int i = 1024; i < 2048; i++ )
        spectrum[i] = frame.spectrum[i];
    // spectrum, aperiodicity, f0 の順に格納．
    char buf[2048];
    sprintf( buf, "%s", output.c_str() );  // うーん....
    FILE *fp = fopen( buf, "w" );          // うーん．．．．
    if( fp ){
        for( int i = 0; i < 2048; i++ )
            fprintf( fp, "%.10e\n", spectrum[i] );
        for( int i = 0; i < 4; i++ )
            fprintf( fp, "%.10e\n", aperiodicity[i] );
        fprintf( fp, "%lf\n", f0 );
    } else {
        SAFE_DELETE_ARRAY( spectrum );
        SAFE_DELETE_ARRAY( aperiodicity );
        return false;
    }

    fclose( fp );

    SAFE_DELETE_ARRAY( spectrum );
    SAFE_DELETE_ARRAY( aperiodicity );

    return true;
}

vConnect::vConnect()
{
    for( int i = 0; i < NOTE_NUM; i++ )
        noteFrequency[i] = A4_PITCH * pow( 2.0, (double)( i - A4_NOTE ) / 12.0 );
    vibrato[0] = 0.0;
    for( int i = 1; i < VIB_NUM; i++ ){
        double period = exp( 5.24 - 1.07e-2 * i ) * 2.0 / 1000.0;
        vibrato[i] = 2.0 * ST_PI / period;
    }

    time_t timer;
    time( &timer );
    srand( (unsigned int)timer );
    fluctTheta = 2.0 * (double)rand() / (double)RAND_MAX * ST_PI;
}

vConnect::~vConnect()
{
    for( unsigned int i = 0; i < managers.size(); i++ )
        SAFE_DELETE( managers[i] );
}

void    vConnect::emptyPath( double secOffset, string_t output )
{
    waveFileEx wave;
    wave.setOffset( secOffset );
    string toutput;
    mb_conv( output, toutput );
    wave.writeWaveFile( toutput );
    return;
}

bool vConnect::synthesize( string_t input, string_t output, runtimeOptions options ){
#ifdef _DEBUG
    cout << "vConnect::synthesize; calling vsq.readVsqFile...";
#endif
    // 読み込みこけたら帰る
    if( vsq.readVsqFile( input, options ) != 1 ){
#ifdef _DEBUG
        cout << " done, failed" << endl;
#endif
        return false;
    }
#ifdef _DEBUG
        cout << " done, successed" << endl;
#endif

    options.fast = false;

    // 空のときは空の wave を出力して終了
    if( vsq.events.eventList.empty() ){
        emptyPath( vsq.getEndSec(), output );
        return true;
    }

    long beginFrame, frameLength, waveLength;
    int  fftLength, aperiodicityLength;
    standSpecgram specgram;
    double *wave;

    vector<utauVoiceDataBase*> *pDBs = vsq.getVoiceDBs();
    for( unsigned int i = 0; i < pDBs->size(); i++ ){
        corpusManager *p = new corpusManager;
        p->setVoiceDB( (*pDBs)[i], options );
        managers.push_back( p );
    }

    // 準備１．先行発音などパラメータの適用，及びコントロールカーブをフレーム時刻へ変換
    this->calculateVsqInfo();

    // 準備２．合成に必要なローカル変数の初期化
    beginFrame = vsq.events.eventList[0]->beginFrame;
    frameLength = endFrame - beginFrame;

    specgram.setFrameLength( frameLength, options.fast );
    fftLength = specgram.getFFTLength();
    aperiodicityLength = specgram.getAperiodicityLength();
    vector<double> dynamics( frameLength, 0.0 );

    // 準備３．振幅・基本周波数・時刻 t を計算する．
    this->calculateF0( specgram, dynamics );

    vConnectArg arg1, arg2;

    arg1.aperiodicityLength = aperiodicityLength;
    arg1.beginFrame = beginFrame;
    arg1.dynamics = &dynamics;
    arg1.fftLength = fftLength;
    arg1.managers = &managers;
    arg1.offset = 0;
    arg1.frameLength = frameLength;
    arg1.options = &options;
    arg1.vsq = &vsq;
    arg1.specgram = &specgram;
    arg1.controlCurves = &controlCurves;
#ifdef STND_MULTI_THREAD
#ifdef _DEBUG
    cout << "vConnect::synthesize; STND_MULTI_THREAD" << endl;
#endif
    thread_t hThread[2];
    hMutex = stnd_mutex_create();// CreateMutex(NULL, FALSE, NULL);
    hFFTWMutex = stnd_mutex_create();// CreateMutex(NULL, FALSE, NULL);
#ifdef _DEBUG
    cout << "vConnect::synthesize; mutex created: hFFTWMutex" << endl;
#endif
    memcpy(&arg2, &arg1, sizeof(vConnectArg));
    arg1.frameLength /= 2;
    arg2.offset = arg1.frameLength;

    hThread[0] = stnd_thread_create( calculateSpecgram, &arg1 );
    hThread[1] = stnd_thread_create( calculateSpecgram, &arg2 );

    stnd_thread_join( hThread[0] );
    stnd_thread_join( hThread[1] );

    stnd_thread_destroy( hThread[0] );
    stnd_thread_destroy( hThread[1] );
    stnd_mutex_destroy( hMutex );
    hMutex = NULL;

#else
#ifdef _DEBUG
    cout << "vConnect::synthesize; not STND_MULTI_THREAD" << endl;
#endif
    calculateSpecgram(&arg1);
#endif

    // 二段階に分けた．
    wave = specgram.synthesizeWave( &waveLength );
    /* 波形自体の編集をここに書く． */
    //calculateDynamics( dynamics, wave, waveLength, options.volumeNormalization );

    specgram.writeWaveFile( output, beginFrame, NULL);// &dynamics );

#ifdef STND_MULTI_THREAD
    // FFTW 用の mutex は合成時にも使うので最後に解放．
    stnd_mutex_destroy( hFFTWMutex );
    hFFTWMutex = NULL;
#endif

    return true;
}

/**
 * 固定長以降の位置を計算する．
 * position = actualPosition - consonantEndPosition
 */
int calculatePhoneticTime(float msFixedLength, int tLen, int position, bool fast)
{
    // うーん．．．
    //return (int)(msFixedLength/framePeriod);

    // 真ん中を繰り返し使うようにしよう．
    tLen -= 20;  // 後半 20[frames] は信用しない
    int ret;
    int fixedLength = (int)(msFixedLength / framePeriod);
    int segment = (tLen - fixedLength) / 2;
    int times = position / segment;
    int rest = position % segment;

    if(times == 0){
        ret = fixedLength + rest;
    }else{
        if(times % 2){
            ret = fixedLength + segment + rest;
        }else{
            ret = fixedLength + segment * 2 - rest;
        }
    }

    return ret;
}

__stnd_thread_start_retval __stnd_declspec calculateSpecgram(void *arg)
{
    vConnectArg* p = (vConnectArg*)arg;
    long beginFrame, frameLength, index, position;
    int  fftLength, aperiodicityLength;
    vector<double> *dynamics;
    vector<vector<standBP> > *controlCurves;
    vector<corpusManager*> *managers;
    runtimeOptions options;

    //// 引数のコピー．
    //
    options.encodingOtoIni = p->options->encodingOtoIni;
    options.encodingVoiceTexture = p->options->encodingVoiceTexture;
    options.encodingVowelTable = p->options->encodingVowelTable;
    options.encodingVsqText = p->options->encodingVsqText;
    options.f0Transform = p->options->f0Transform;
    options.fast = p->options->fast;
    options.volumeNormalization = p->options->volumeNormalization;
    options.wspMode = p->options->wspMode;

    fftLength = p->fftLength;
    aperiodicityLength  = p->aperiodicityLength;
    beginFrame = p->beginFrame;
    vsqFileEx *vsq = p->vsq;
    standSpecgram *specgram = p->specgram;
    index = p->offset;
    dynamics = p->dynamics;
    controlCurves = p->controlCurves;
    frameLength = p->frameLength;
    managers = p->managers;
    //
    //// コピーここまで．

    double *aperiodicity;
    double *spectrum;
    double *trans = new double[fftLength];
    double *temporary = new double[fftLength];

    standData *present, *next = NULL;
    standFrame target, presentFrame, nextFrame;

    int briIndex, breIndex, cleIndex, genIndex, opeIndex;
    double presentVelocity, nextVelocity, consonantEndFrame, nextConsonantEndFrame, tmp;
    double breRate, genRate, briRate, cleRate;

    briIndex = breIndex = cleIndex = genIndex = opeIndex = 0;

    fftw_plan inverseFFT, forwardFFT;
    fftw_complex *melSpectrum = new fftw_complex[fftLength/2];
    fftw_complex *melCepstrum = new fftw_complex[fftLength/2];

    standComplex *thisMel, *nextMel;
    int thisMelLen, nextMelLen;
#ifdef STND_MULTI_THREAD
    if(hFFTWMutex)
        stnd_mutex_lock(hFFTWMutex);
#endif
    inverseFFT = fftw_plan_dft_1d(fftLength, melSpectrum, melCepstrum, FFTW_BACKWARD, FFTW_ESTIMATE);
    forwardFFT = fftw_plan_dft_1d(fftLength, melCepstrum, melSpectrum, FFTW_FORWARD, FFTW_ESTIMATE);
#ifdef STND_MULTI_THREAD
    if(hFFTWMutex)
        stnd_mutex_unlock(hFFTWMutex);
#endif

    // 合成開始．長くなってしまうのはどうにもならんのか
    int num_events = vsq->events.eventList.size();
    vsqEventEx *itemNext = NULL;

    itemNext = vsq->events.eventList[0];
    int beginItemNumber;

    // 合成開始するノート番号を計算．
    for( int i = 0; i < num_events; i++ ){
        if( index < vsq->events.eventList[i]->endFrame - beginFrame ){
            itemNext = vsq->events.eventList[i];
            beginItemNumber = i;
            break;
        }
    }
    for( int i = beginItemNumber; i < num_events && index < frameLength; i++ ){
        vsqEventEx *itemThis = itemNext;
        int morphBeginFrame = INT_MAX;
        if( i + 1 < num_events ){
            itemNext = vsq->events.eventList[i + 1];
        }

        // 合成範囲外は合成範囲になるまで無音を与える
        if( itemThis->beginFrame > beginFrame + index ){
            for( ; index < itemThis->beginFrame - beginFrame; index++ ){
                specgram->getFramePointer( index, target );
                (*dynamics)[index] = 0.0;
                *(target.f0) = -1.0;
            }
        }
        // 合成可能範囲内なので wav ファイルを検索，適宜 WORLD で分析．
        if( 0 <= itemThis->singerIndex && itemThis->singerIndex < managers->size() ){
            present = (*managers)[itemThis->singerIndex]->getStandData( itemThis->lyricHandle.getLyric(), options );

            if(!present || !present->specgram){
                // 見つからない場合は次に行こう．
                continue;
            }
            // 固定長が分析長より長い場合は分析長にしておく．
            if(itemThis->utauSetting.msFixedLength / framePeriod >= present->specgram->getTimeLength()){
                itemThis->utauSetting.msFixedLength = (double)(present->specgram->getTimeLength()-1) * framePeriod;
            }
            presentVelocity = pow( 2.0, (double)(64 - itemThis->velocity) / 64.0);
            consonantEndFrame = itemThis->utauSetting.msFixedLength * presentVelocity / framePeriod;
        }else{
            // ありえない歌手番号の際は次に行ってしまう．
            continue;
        }
        if( itemThis->isContinuousBack ){
            if( 0 <= itemNext->singerIndex && itemNext->singerIndex < managers->size() ){
                next = (*managers)[itemNext->singerIndex]->getStandData( itemNext->lyricHandle.getLyric(), options );
            }
            if( next && next->specgram ){
                // 固定長が分析長より長い場合は分析長にしておく．
                if(itemNext->utauSetting.msFixedLength / framePeriod >= next->specgram->getTimeLength()){
                    itemNext->utauSetting.msFixedLength = (double)(next->specgram->getTimeLength()-1) * framePeriod;
                }
            }
            nextVelocity = pow( 2.0, (double)( 64 - itemNext->velocity ) / 64.0 );
            nextConsonantEndFrame = itemNext->utauSetting.msFixedLength * nextVelocity / framePeriod;
            morphBeginFrame = max(itemThis->beginFrame, itemNext->beginFrame) - beginFrame;
        }

        for( ; index < itemThis->endFrame - beginFrame && index < frameLength; index++ ){
            double morphRate = 0.0;
            specgram->getFramePointer( index, target );
            spectrum = target.spectrum;
            aperiodicity = target.aperiodicity;
            nextMel = NULL;

            position = index - itemThis->beginFrame + beginFrame;

            if( position < consonantEndFrame ){
                position = (long)((double)position / presentVelocity);
            }else{
                position = calculatePhoneticTime(itemThis->utauSetting.msFixedLength, present->specgram->getTimeLength(), position - consonantEndFrame, options.fast);
            }

            present->specgram->getFramePointer( position, presentFrame );
            thisMel = present->melCepstrum.getMelCepstrum(position * framePeriod, &thisMelLen);

            // 現在注目しているフレームが無声音の場合それを優先する．
            if( *(presentFrame.f0) == 0.0 ){
                *(target.f0) = 0.0;
                nextFrame.f0 = presentFrame.f0;
                nextFrame.spectrum = presentFrame.spectrum;
                nextFrame.aperiodicity = presentFrame.aperiodicity;

                // 音符末尾の場合 morphRateを計算．
            }else if( morphBeginFrame <= index ){
                position = index - itemNext->beginFrame + beginFrame;

                if( position < nextConsonantEndFrame ){
                    position = (long)((double)position / nextVelocity);
                }else{
                    position = calculatePhoneticTime(itemNext->utauSetting.msFixedLength, next->specgram->getTimeLength(), position - nextConsonantEndFrame, options.fast);
                }
                next->specgram->getFramePointer( position, nextFrame );
                nextMel = next->melCepstrum.getMelCepstrum(position * framePeriod, &nextMelLen);

                morphRate = (double)(index - morphBeginFrame) / (double)(itemThis->endFrame - beginFrame - morphBeginFrame);
                morphRate = 0.5 - 0.5 * cos( ST_PI * morphRate );

                if( *(nextFrame.f0) == 0.0 && options.fast ){
                    morphRate = 1.0;
                    *(target.f0) = 0.0;
                }
            }else{          // これはエラー防止用．
                nextFrame.f0 = presentFrame.f0;
                nextFrame.spectrum = presentFrame.spectrum;
                nextFrame.aperiodicity = presentFrame.aperiodicity;
            }

            // morphRate にしたがって spectrum 計算．
            for(int k = 0; k <= fftLength / 2; k++){
                spectrum[k] = pow(presentFrame.spectrum[k], 1.0 - morphRate) * pow(nextFrame.spectrum[k],morphRate);
            }
            for(int k = 0; k < aperiodicityLength; k++){
                aperiodicity[k] = presentFrame.aperiodicity[k] * (1.0 - morphRate) + nextFrame.aperiodicity[k] * morphRate;
            }

            /* ここに周波数変換 */
            if( *(target.f0) ){                // 無声音の場合とりあえず何もしない

                // 表情系のコントロールはここにまとめておこう．
                while( index + beginFrame > (*controlCurves)[BRIGHTNESS][briIndex].frameTime ){ briIndex++; }
                while( index + beginFrame > (*controlCurves)[BRETHINESS][breIndex].frameTime ){ breIndex++; }
                while( index + beginFrame > (*controlCurves)[CLEARNESS][cleIndex].frameTime ){ cleIndex++; }
                while( index + beginFrame > (*controlCurves)[GENDER][genIndex].frameTime ){ genIndex++; }

                // いまはまだはやい
                // transform_F0( temp_texture, spectrum, *(target.f0), *(presentFrame.f0), fftLength, 1.0 );

                /* BRE / BRI / CLE */
                breRate = (double)(*controlCurves)[BRETHINESS][breIndex].value / 128.0;
                briRate = (double)((*controlCurves)[BRIGHTNESS][briIndex].value - 64 ) / 64.0;
                cleRate = (double)(*controlCurves)[CLEARNESS][cleIndex].value / 128.0;
                if( options.fast ){
                    // WORLD 0.0.1 における非周期性パラメタはシグモイド関数のパラメタであり，
                    // それぞれ以下のパラメタを決定する．
                    // ap[0] = u :: 非周期性指標のダイナミックレンジ
                    // ap[1] = b :: 非周期性指標の最大値 max = 1.0 - b で決定する．
                    // ap[2] = fc:: 非周期性指標の値が(u + b) / 2になる f の値
                    // ap[3] = w :: シグモイド関数の傾き
                    // 値は再考の余地大有り．
                    aperiodicity[0] = 0.1 + 0.4 * breRate;
                    aperiodicity[1] = 0.9 - 0.7 * breRate;
                    aperiodicity[2] *= pow(2.0, briRate); // うーん...
                    aperiodicity[3] *= pow(4.0, 0.5 - breRate);
                    //
                    // なお非周期性指標は周波数軸上で
                    // noiseSpectrum[i] = spectrum[i] * aperiodicityRatio[i]
                    // の形でスペクトル中に含まれるノイズの量を決定する
                }else {
                    // WORLD0.0.4 における非周期性パラメタは STAR スペクトルから計算した最小位相応答システムのスペクトルを
                    // ピッチマークに基づき切り出した信号のスペクトルで各成分ごとに除算したもののようだ．
                    // なお，0 番目と奇数番目が実数成分である．
                    double amp = pow(2.0, breRate);
                    for( int k = 1; k < aperiodicityLength; k+=2 ){
                        //aperiodicity[k] *= amp;
                    }
                }

                if(options.fast && thisMel){
                    stretchToMelScale(melSpectrum, spectrum, fftLength / 2 + 1, fs / 2);
                    for(int k = 0; k <= fftLength / 2; k++){
                        melSpectrum[k][0] = log(melSpectrum[k][0] + 1.0e-17) / (double)(fftLength / 2);
                    }
                    fftw_execute(inverseFFT);
                    if(morphRate > 0 && nextMel){
                        for(int k = 0; k < thisMelLen; k++){
                            melCepstrum[k][0] = melCepstrum[k][0] * briRate + (1.0 - briRate) * (thisMel[k].re * (1.0 - morphRate) + nextMel[k].re);
                            melCepstrum[k][1] = melCepstrum[k][1] * briRate + (1.0 - briRate) * (thisMel[k].im * (1.0 - morphRate) + nextMel[k].im);
                            melCepstrum[fftLength/2-k-1][0] = melCepstrum[k][0];
                            melCepstrum[fftLength/2-k-1][1] = -melCepstrum[k][1];
                        }
                    }else{
                        for(int k = 0; k < thisMelLen; k++){
                            melCepstrum[k][0] = melCepstrum[k][0] * briRate + (1.0 - briRate) * thisMel[k].re;
                            melCepstrum[k][1] = melCepstrum[k][1] * briRate + (1.0 - briRate) * thisMel[k].im;
                            melCepstrum[fftLength/2-k-1][0] = melCepstrum[k][0];
                            melCepstrum[fftLength/2-k-1][1] = -melCepstrum[k][1];
                        }
                    }
                    fftw_execute(forwardFFT);
                    stretchFromMelScale(spectrum, melSpectrum, fftLength / 2, fs / 2);
                    for(int k = 0; k < fftLength / 2; k++){
                        spectrum[k] = exp(spectrum[k]);
                    }
                }else{
                    for(int k = 0; k <= fftLength / 2; k++){
                        double freq = (double)k / (double)fftLength * (double)fs;
                        if(freq < *target.f0 * 2){
                            spectrum[k] /= pow(4.0, briRate);
                        }else if(freq > *target.f0 * 4){
                            spectrum[k] *= pow(4.0, briRate);
                        }else{
                            spectrum[k] /= pow(4.0, briRate * cos((freq - *target.f0 * 2.0) / *target.f0 * ST_PI));
                        }
                    }
                }

                /* GEN 適用 */
                genRate = pow( 2.0, (double)((*controlCurves)[GENDER][genIndex].value - 64) / 64.0 );

                for( int k = 0; k <= fftLength / 2; k++ ){
                    tmp = genRate * (double)k;
                    if( tmp > fftLength / 2 ){
                        temporary[k] = spectrum[fftLength / 2 - 1];
                    }else{
                        temporary[k] = interpolateArray( tmp, spectrum );
                    }
                }

                memcpy( spectrum, temporary, sizeof(double)*( fftLength / 2 + 1 ) );
                // 不必要な部分はゼロ詰め．
                for(int k = fftLength / 2 + 1; k < fftLength; k++){
                    spectrum[k] = 0.0;
                }
            }
        }
    }

    SAFE_DELETE_ARRAY( melSpectrum );
    SAFE_DELETE_ARRAY( melCepstrum );
    SAFE_DELETE_ARRAY( temporary );
    SAFE_DELETE_ARRAY( trans );

#ifdef STND_MULTI_THREAD
#ifndef USE_PTHREADS
    _endthreadex( 0 );
#endif
    return NULL;
#endif
}

void vConnect::calculateVsqInfo( void )
{
    // 書きづらいので
    vector<vsqEventEx*>* events = &( vsq.events.eventList );
    string_t temp;
    vector<utauVoiceDataBase*> *pDBs = vsq.getVoiceDBs();
    utauVoiceDataBase* voiceDB;

    float msPreUtterance, msVoiceOverlap;
    int singerIndex = 0;

    endFrame = 0;

    /////////
    // 前から後ろをチェック
    for( unsigned int i = 0; i < events->size(); i++ ){
        vsqEventEx* itemi = vsq.events.eventList[i];
        
        // タイプ判定
        while( itemi->type == _T( "Singer" ) ){
            // 歌手なら歌手番号拾ってきて
            singerIndex = vsq.getSingerIndex( itemi->iconHandle.getIDS() );

            // 自分を消して
            vector<vsqEventEx*>::iterator it = events->begin();
            int j = 0;
            while( it != events->end() ){
                if( itemi == (*it) ) break;
                j++;
                it++;
            }
            if( it != events->end() ){
                events->erase( it );
                SAFE_DELETE( itemi );
            }

            // （ i 番目今消しちゃったから次に進んでるのと一緒だから ）
            if( i >= events->size() )
                break;
            // 次の音符へ
            itemi = vsq.events.eventList[i];
        }
        if( singerIndex < 0 || singerIndex >= pDBs->size() )
            continue;
        voiceDB = (*pDBs)[singerIndex];
        // 原音設定の反映
        temp = itemi->lyricHandle.getLyric();
        msPreUtterance = itemi->utauSetting.msPreUtterance;
        msVoiceOverlap = itemi->utauSetting.msVoiceOverlap;
        voiceDB->getUtauParameters( itemi->utauSetting, temp );
        itemi->utauSetting.msPreUtterance = msPreUtterance;
        itemi->utauSetting.msVoiceOverlap = msVoiceOverlap;

        // 空白文字が存在したときはVCV音素片
        itemi->isVCV = ( temp.find( _T(" ") ) != string_t::npos );

        // 休符の文字はとりあえず 'R', 'r' を対象にしてUTAUパラメタを初期化しておこう．
        itemi->isRest = ( temp.compare( _T("R") ) == 0 || temp.compare( _T("r") ) == 0);
        if(itemi->isRest){
            itemi->utauSetting.msPreUtterance = itemi->utauSetting.msVoiceOverlap = 0.0;
        }

        // 開始位置の計算
        itemi->beginFrame = (long)( (
                        vsq.vsqTempoBp.tickToSecond( itemi->tick ) * 1000.0 - itemi->utauSetting.msPreUtterance
                        * pow( 2.0, ( 64.0 - itemi->velocity ) / 64.0 ) ) / framePeriod );
        // ポルタメントが０％の場合適当な値を入れておく
        if( itemi->portamentoLength < 2 )
            itemi->portamentoLength = 2;
        itemi->singerIndex = singerIndex;
    }

    /////////
    // 後ろから前をチェック
    for( unsigned int i = 0; i < events->size(); i++ ){
        // まず Tick 時刻から終了時刻を計算
        (*events)[i]->endFrame = (long)(
            ( vsq.vsqTempoBp.tickToSecond( vsq.events.eventList[i]->tick + vsq.events.eventList[i]->length ) * 1000.0 ) / framePeriod );

        // 一個前の音符がある場合，連続性のチェック
        if( i ){
            // i 番目の音符が，i - 1 番目の音符が終わる前に始まる場合連続とみなす
            (*events)[i-1]->isContinuousBack = ( (*events)[i]->beginFrame <= (*events)[i-1]->endFrame );

            ////* 連続時のオーバーラップの設定 */
            
            if( (*events)[i-1]->isContinuousBack ){

                // i 番目が CV 音素片の場合
                if( !(*events)[i]->isVCV ){
                    // まず i 番目の先行発音を i - 1 番目に適用する
                    (*events)[i-1]->endFrame -= (long)( (*events)[i]->utauSetting.msPreUtterance
                                                        * pow( 2.0, ( 64.0 - (*events)[i]->velocity ) / 64.0 ) / framePeriod );
                    // さらにオーバーラップも適用する
                    (*events)[i-1]->endFrame += (long)( (*events)[i]->utauSetting.msVoiceOverlap / framePeriod );
                } else
                    if( (*events)[i-1]->endFrame - (*events)[i-1]->beginFrame > 20 )
                        (*events)[i-1]->endFrame -= 20;
                    else
                        (*events)[i-1]->endFrame = (*events)[i-1]->beginFrame;

                // 最後にセーフガードとして短くなりすぎないようにチェック
                if( (*events)[i-1]->endFrame < (*events)[i-1]->beginFrame )
                        (*events)[i-1]->endFrame = (*events)[i-1]->beginFrame;
            }
        }

    }

    for( unsigned int i = 0; i < events->size(); i++ )
        if( endFrame < (*events)[i]->endFrame )
            endFrame = (*events)[i]->endFrame;

    // コントロールカーブは vsq 管理クラスにやってもらう
    controlCurves.resize( vsq.controlCurves.size() );
    for( unsigned int i = 0; i < controlCurves.size(); i++ )
        vsq.controlCurves[i].getList( controlCurves[i] );
}

void    vConnect::calculateF0( standSpecgram& dst, vector<double>& dynamics )
{
    standFrame frame;
    double *f0, *t, pitch_change, tmp, vibratoTheta = 0.0, vibratoRate, vibratoDepth;
    long beginFrame = vsq.events.eventList[0]->beginFrame;
    long frameLength = endFrame - beginFrame;
    long index = 0;
    long portamentoBegin, portamentoLength;
	long previousEndFrame = LONG_MIN, vibratoBeginFrame = 0, noteBeginFrame;
    int  pitIndex = 0, pbsIndex = 0, dynIndex = 0; // ControlCurve Index

    // 仕様依存．f0 と t は連続した配列でなければならない．
    dst.getFramePointer( 0, frame );
    f0 = frame.f0;
    t = frame.t;

    for( unsigned int i = 0; i < vsq.events.eventList.size(); i++ ){
        vsqEventEx *itemi = vsq.events.eventList[i];

        // デフォルト値で埋める
        for( ; index < itemi->beginFrame - beginFrame; index++ ){
            f0[index] = -1.0;//0.0;
            t[index] = (double)index * framePeriod / 1000.0;
            dynamics[index] = 0.0;
        }

        // 後続のノートがあるかどうか
        if( !itemi->isContinuousBack ){
            // ないなら，ポルタメントの開始位置を設定
            portamentoBegin = itemi->endFrame - 50;
            if( portamentoBegin < itemi->beginFrame )
                portamentoBegin = itemi->beginFrame;
            portamentoBegin -= beginFrame;
        }else{
            // ありえない開始位置にしておくよ！！
            portamentoBegin = LONG_MAX;
        }

		// vibrato 開始位置は元々の音符開始位置近辺から計算する．そこまで厳密じゃなくていいよね．
		if( previousEndFrame > itemi->beginFrame - beginFrame ){
			vibratoBeginFrame = previousEndFrame;
			vibratoBeginFrame += (long)( 1000.0 * vsq.vsqTempoBp.tickToSecond( itemi->vibratoDelay ) /framePeriod );
		} else {
			vibratoBeginFrame = itemi->beginFrame - beginFrame;
			vibratoBeginFrame += (long)( 1000.0 * vsq.vsqTempoBp.tickToSecond( itemi->vibratoDelay ) /framePeriod );
		}

        // ノート・ビブラート・微細振動を書く
        for( ; index < itemi->endFrame - beginFrame; index++ ){
            // ピッチetcカーブに格納されている値の内どれを使うか？
            while( index + beginFrame > controlCurves[PITCH_BEND][pitIndex].frameTime ){
                pitIndex++;
            }
            while( index + beginFrame > controlCurves[PITCH_BEND_SENS][pbsIndex].frameTime ){
                pbsIndex++;
            }
            while( index + beginFrame > controlCurves[DYNAMICS][dynIndex].frameTime ){
                dynIndex++;
            }
            pitch_change = pow( 2, (double)controlCurves[PITCH_BEND][pitIndex].value / 8192.0 * (double)controlCurves[PITCH_BEND_SENS][pbsIndex].value / 12.0 );
            f0[index] = noteFrequency[itemi->note] * pitch_change * getPitchFluctuation( (double)index * framePeriod / 1000.0 );
            t[index] = (double)index * framePeriod / 1000.0;
            dynamics[index] = (double)controlCurves[DYNAMICS][dynIndex].value / 64.0;
            if( index > portamentoBegin ){
                dynamics[index] *= 1.0 - (double)( index - portamentoBegin ) / 50.0;
            }
            
            /* Vibrato */
            if( index > vibratoBeginFrame ){
				double pos = (double)(index - vibratoBeginFrame ) / (double)( itemi->endFrame - beginFrame - vibratoBeginFrame );
                vibratoRate = vibrato[itemi->vibratoHandle.getVibratoRate( pos )];
                vibratoDepth = (double)itemi->vibratoHandle.getVibratoDepth( pos ) * 2.5 / 127.0 / 2.0;
                vibratoTheta += vibratoRate * framePeriod / 1000.0;
                f0[index] *= pow( 2.0, 1.0 / 12.0 * vibratoDepth * sin( vibratoTheta ) );
                if( vibratoTheta > 2.0 * ST_PI ){
                    vibratoTheta -= 2.0 * ST_PI;
                }
            }else{
                vibratoTheta = 0.0;
            }
		}
		previousEndFrame = itemi->endFrame - beginFrame;
    }

    previousEndFrame = LONG_MIN;

    // ポルタメントを描きます．（ビブラートとは実は順番依存）
    for( unsigned int i = 0; i < vsq.events.eventList.size(); i++ ){
        vsqEventEx *itemi = vsq.events.eventList[i];

        // 後続がいる場合は必要なパラメータを計算してポルタメントを書く
        if( itemi->isContinuousBack ){
            if( previousEndFrame > itemi->beginFrame )
                noteBeginFrame = previousEndFrame;
            else
                if( itemi->isVCV )
                    noteBeginFrame = (long)( vsq.vsqTempoBp.tickToSecond( itemi->tick ) * 1000.0 / framePeriod );
                else
                    noteBeginFrame = itemi->beginFrame;
            portamentoBegin = noteBeginFrame
                      + (long)((double)(itemi->endFrame - noteBeginFrame)
                      * (1.0 - (double)(itemi->portamentoLength) / 100.0));
            tmp = noteFrequency[vsq.events.eventList[i + 1]->note] / noteFrequency[itemi->note];
        }else{
            continue;
        }

        portamentoLength = itemi->endFrame - portamentoBegin;
        double inv_portamentoLength = 1.0 / (double)portamentoLength;
        long frameOffset = portamentoBegin - beginFrame;
        for( long j = 0; j < portamentoLength; j++ ){
            double x = (double)j * inv_portamentoLength;
            f0[j + frameOffset] *= pow( tmp, 0.5 * (1.0 - cos( ST_PI * x ))
                                    - (double)itemi->portamentoDepth / 100.0 * (sin( ST_PI * 4.0 / 3.0 * x ) * (1.5 - x) / 1.5) );
        }
        for( long j = portamentoLength; j < portamentoLength * 3 / 2; j++ ){
            double x = (double)j * inv_portamentoLength;
            f0[j + frameOffset] *= pow( tmp, - (double)itemi->portamentoDepth / 100.0 * sin( ST_PI * 4.0 / 3.0 * x ) * (1.5 - x) / 1.5 );
        }
        previousEndFrame = itemi->endFrame;
    }
}

void vConnect::calculateDynamics( vector<double> &dynamics, double *wave, long wave_len, bool volumeNormalization )
{
    vector<long>   decayFrame( vsq.events.eventList.size() );
    vector<long>   attackFrame( vsq.events.eventList.size() );
    double presentVelocity;
    long noteBeginFrame;
    long previousEndFrame = LONG_MIN;
    long beginFrame = vsq.events.eventList[0]->beginFrame;
    long consonantEndFrame, preutterance;

    /*== 適用する位置を計算する ==*/
    for( unsigned int i = 0; i < vsq.events.eventList.size(); i++ ){
        vsqEventEx* itemi = vsq.events.eventList[i];

        /*============= 固定長終了時を attack, 真ん中か，可能であれば， attack + 100[ms] の位置を decay とする．============*/

        // 準備
        presentVelocity = pow( 2.0, (double)(64 - itemi->velocity ) / 64.0);
        consonantEndFrame = (long)(itemi->utauSetting.msFixedLength * presentVelocity / framePeriod);
        preutterance = (long)(itemi->utauSetting.msPreUtterance * presentVelocity / framePeriod);

        // 音符の開始位置は固定長終了位置か, 直前の音符が終了した位置.
        if( previousEndFrame > itemi->beginFrame + consonantEndFrame - beginFrame ){
            noteBeginFrame = previousEndFrame;
        }else{
            noteBeginFrame = itemi->beginFrame + consonantEndFrame - beginFrame;
        }

        // attack 位置の計算
        if( itemi->isVCV ){
            attackFrame[i] = itemi->beginFrame + preutterance - beginFrame;
        }else{
            attackFrame[i] = itemi->beginFrame + consonantEndFrame - beginFrame;
        }

        // decay 位置の計算
        // attack + 100ms はあまりよろしくない位置のとき．
        if( itemi->endFrame - beginFrame < attackFrame[i] + 100 / framePeriod )
            decayFrame[i] = ( noteBeginFrame + itemi->endFrame - beginFrame ) / 2;
        else
            decayFrame[i] = attackFrame[i] + 100 / framePeriod;

        previousEndFrame = itemi->endFrame - beginFrame;

    }

    /*== 抽出した音量と音符情報から音量遷移曲線を描画 ==*/
    previousEndFrame = LONG_MIN;
    // 取り扱っている音符が直前の音符と連続しているかどうか
    // (直前の音符のisContinuousBackフラグが起ってるかどうか)
    bool   isContinuousFront = false;
    int    previousFrame, tmpLength;
    double previousValue, tmpValue;
    for( unsigned int i = 0; i < vsq.events.eventList.size(); i++ ){
        vsqEventEx* itemi = vsq.events.eventList[i];

        // 明示的休符は無視する．
        if( itemi->isRest ){
            continue;
        }

        double attackRate = pow( 2.0, (double)( itemi->attack - 50 ) / 50.0 );
        double decayRate  = pow( 2.0, (double)( itemi->decay  - 50 ) / 50.0 );

        if( isContinuousFront ){
            noteBeginFrame = previousFrame;
        } else {
            noteBeginFrame = itemi->beginFrame - beginFrame;
            previousValue = attackRate;
        }

        // 前回終了位置から attack の開始（固定長終了）位置までを余弦関数で接続．
        int frameOffset = noteBeginFrame;
        tmpLength = attackFrame[i] - frameOffset;
        if( tmpLength + frameOffset >= (int)dynamics.size() ){
            // dynamicsのout of range例外を回避
            tmpLength = dynamics.size() - frameOffset;
        }
        tmpValue = attackRate - previousValue;
        for( int index = 0; index < tmpLength; index++ ){
            dynamics[index + frameOffset] *= ( previousValue + tmpValue * ( 0.5 - 0.5 * cos( ST_PI * (double)index / (double)tmpLength ) ) );
        }

        // attack の開始（固定長終了）位置から decay 位置まで．
        frameOffset = attackFrame[i];
        tmpLength = decayFrame[i] - frameOffset;
        if( tmpLength + frameOffset >= (int)dynamics.size() ){
            // dynamicsのout of range例外を回避
            tmpLength = dynamics.size() - frameOffset;
        }
        tmpValue = decayRate - attackRate;
        for( int index = 0; index < tmpLength; index++ ){
            dynamics[index + frameOffset] *= ( attackRate + tmpValue * ( 0.5 - 0.5 * cos( ST_PI * (double)index / (double)tmpLength ) ) );
        }

        if( !itemi->isContinuousBack || vsq.events.eventList[i+1]->isRest ){
            /* 後続音は存在しないか明示的休符なので最低限の正規化 */
            for( int index = decayFrame[i]; index < itemi->endFrame - beginFrame; index++ ){
                dynamics[index] *= decayRate;
            }
            isContinuousFront = false;
        } else {
            /* 次の音符用に準備 */
            previousFrame = decayFrame[i];
            previousValue = decayRate;
            isContinuousFront = true;
        }
    }
}

#include "Utility.h"

#include <QMessageBox>

#include <math.h>

// 波形の正規化に使う定数群
const static unsigned int NUM_NORMALIZE = 2048;
const static double VOL_NORMALIZE = 0.06;

bool stand::utility::makeDirectory(QWidget *w, const QDir &dir, bool confirm)
{
    // 存在する場合は真を返して終了．
    if(dir.exists())
    {
        return true;
    }
    // 確認する場合
    if(confirm)
    {
        int val = QMessageBox::warning(w,
                                       "Confirm",
                                       dir.path() + "\ndoes not exist. Do you want to create the directory",
                                       QMessageBox::Yes, QMessageBox::No);
        if(val == QMessageBox::No)
        {
            return false;
        }
    }
    // ディレクトリを作成．
    if(!dir.mkpath(dir.absolutePath()))
    {
        QMessageBox::critical(w, "Error", dir.path() + "\nCould not mkdir.");
        return false;
    }
    return true;
}

int stand::utility::lengthForUtauSetting(int waveLength, int samplingFrequency, float leftBlank, float rightBlank)
{
    int ret = 0;
    // 負値の右ブランクは音素長を示す．
    if(rightBlank < 0)
    {
        ret = -rightBlank / 1000.0 * samplingFrequency;
    }
    // 正値の右ブランクは音素終了位置からのブランク値
    else
    {
        ret = waveLength - (leftBlank + rightBlank) / 1000.0 * samplingFrequency;
    }
    return ret;
}

void stand::utility::normalizeByAnchor(double *x, unsigned int length)
{
    double sumBegin = 0, sumEnd = 0, maxAmp, c;
    // 開始・終了位置から NUM_NORMALIZE 点の二乗振幅平均のうち大きい方を使って正規化する．
    for(unsigned i = 0; i < NUM_NORMALIZE; i++)
    {
        sumBegin += x[i] * x[i];
        sumEnd += x[length - i - 1] * x[length - i - 1];
    }
    maxAmp = (sumBegin > sumEnd) ? sumBegin : sumEnd;
    maxAmp = sqrt(maxAmp / (double)NUM_NORMALIZE);

    // 完全に無音の場合はゼロ除算が出るので避ける．
    if(maxAmp == 0.0)
    {
        return;
    }
    c = VOL_NORMALIZE / maxAmp;
    for(unsigned int i = 0; i < length; i++)
    {
        x[i] *= c;
    }
}


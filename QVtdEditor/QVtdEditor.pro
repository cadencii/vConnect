# -------------------------------------------------
# Project created by QtCreator 2010-08-17T11:03:21
# -------------------------------------------------
TARGET = QVtdEditor
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    RenderArea.cpp \
    QScrollAreaEx.cpp \
    ../stand/vowelTable.cpp \
    ../stand/vConnect.cpp \
    ../stand/standTexture.cpp \
    ../stand/standSpecgram.cpp \
    ../stand/stand.cpp \
    ../stand/matching.cpp \
    ../stand/corpusManager.cpp \
    ../stand/mb_text.cpp \
    ../stand/utauVoiceDB/utauVoiceDataBase.cpp \
    ../stand/utauVoiceDB/utauFreq.cpp \
    ../stand/vsqMetaText/vsqTempo.cpp \
    ../stand/vsqMetaText/vsqHandle.cpp \
    ../stand/vsqMetaText/vsqFileEx.cpp \
    ../stand/vsqMetaText/vsqEventList.cpp \
    ../stand/vsqMetaText/vsqEventEx.cpp \
    ../stand/vsqMetaText/vsqBPList.cpp \
    ../stand/vsqMetaText/vsqBase.cpp \
    ../stand/waveFileEx/waveFileEx.cpp \
    ../stand/world/wavread.cpp \
    ../stand/world/tandem_ap.cpp \
    ../stand/world/synthesis_ap.cpp \
    ../stand/world/synthesis.cpp \
    ../stand/world/straight.cpp \
    ../stand/world/star.cpp \
    ../stand/world/platinum.cpp \
    ../stand/world/matlabfunctions.cpp \
    ../stand/world/dio.cpp \
    ../stand/WaveOut.cpp
HEADERS += MainWindow.h \
    RenderArea.h \
    qscrollareaex.h \
    ../stand/vowelTable.h \
    ../stand/vConnect.h \
    ../stand/standTexture.h \
    ../stand/standSpecgram.h \
    ../stand/standFrame.h \
    ../stand/stand.h \
    ../stand/matching.h \
    ../stand/corpusManager.h \
    ../stand/mb_text.h \
    ../stand/utauVoiceDB/utauVoiceDataBase.h \
    ../stand/utauVoiceDB/utauParameters.h \
    ../stand/utauVoiceDB/utauFreq.h \
    ../stand/vsqMetaText/vsqTempo.h \
    ../stand/vsqMetaText/vsqPhonemeDB.h \
    ../stand/vsqMetaText/vsqHandle.h \
    ../stand/vsqMetaText/vsqFileEx.h \
    ../stand/vsqMetaText/vsqEventList.h \
    ../stand/vsqMetaText/vsqEventEx.h \
    ../stand/vsqMetaText/vsqBPList.h \
    ../stand/vsqMetaText/vsqBase.h \
    ../stand/waveFileEx/waveFileEx.h \
    ../stand/world/world.h \
    ../stand/world/wavread.h \
    ../stand/WaveOut.h \
    ../stand/vsqControl.h \
    ../stand/standData.h \
    ../stand/vsqMetaText/vsqVibratoBP.h \
    ../stand/vsqMetaText/vsqLyric.h \
    ../stand/vsqMetaText/vsqBP.h \
    ../stand/waveFileEx/waveFormatEx.h
FORMS += MainWindow.ui
win32:LIBS += -L../../../../../Installer/FFTW/fftw-3.2.2.pl1-dll32/ \
    -lwinmm \
    -lfftw3-3 \
    -liconv
unix:LIBS += -lfftw3 \
    -liconv
mac:LIBS += -framework \
    Foundation \
    -framework \
    CoreAudio \
    -framework \
    AudioToolbox
win32:INCLUDEPATH += ../../../../../Installer/FFTW/fftw-3.2.2.pl1-dll32/

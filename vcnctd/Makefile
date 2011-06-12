WIN=
FFTW=lfftw3
WINSOCK=
LFFTW=D:\fftw

ifneq ($(WIN),)
  FFTW=lfftw3-3
  WINSOCK=-lws2_32
endif

SRC=vcnctd.cpp \
    Server.cpp \
	Config.cpp \
    ConfDB.cpp \
	daemon.c \
	../stand/corpusManager.cpp \
	../stand/mb_text.cpp \
	../stand/stand.cpp \
	../stand/utauVoiceDB/UtauDB.cpp \
	../stand/vConnect.cpp \
	../stand/vConnectSetting.cpp \
	../stand/vConnectPhoneme.cpp \
	../stand/vConnectUtility.cpp \
	../stand/vsqMetaText/vsqBase.cpp \
	../stand/vsqMetaText/vsqEventList.cpp \
	../stand/vsqMetaText/vsqEventEx.cpp \
	../stand/vsqMetaText/vsqFileEx.cpp \
	../stand/vsqMetaText/vsqHandle.cpp \
	../stand/vsqMetaText/vsqTempo.cpp \
	../stand/vsqMetaText/vsqBPList.cpp \
	../stand/waveFileEx/waveFileEx.cpp \
	../stand/world/dio.cpp \
	../stand/world/matlabfunctions.cpp \
	../stand/world/platinum.cpp \
	../stand/world/platinum_v4.cpp \
	../stand/world/star.cpp \
	../stand/world/synthesis.cpp \
	../stand/world/synthesis_v4.cpp

HDR=Server.h \
    Config.h \
	daemon.h \
    ConfDB.h \
	../stand/mb_text.h \
	../stand/vConnect.h \
	../stand/corpusManager.h \
	../stand/vsqMetaText/vsqBase.h \
	../stand/vsqMetaText/vsqTempo.h \
	../stand/vsqMetaText/vsqPhonemeDB.h \
	../stand/vsqMetaText/vsqHandle.h \
	../stand/vsqMetaText/vsqFileEx.h \
	../stand/utauVoiceDB/UtauDB.h \
	../stand/vsqMetaText/vsqEventList.h \
	../stand/vsqMetaText/vsqLyric.h \
	../stand/vsqMetaText/vsqEventEx.h \
	../stand/vsqMetaText/vsqBPList.h \
	../stand/world/world.h \
	../stand/waveFileEx/waveFileEx.h \
	../stand/vConnectSetting.h \
	../stand/vConnectPhoneme.h \
	../stand/vConnectUtility.h \
	../stand/stand.h

vcnctd: $(SRC) $(HDR)
	g++ -L"$(LFFTW)" -g $(SRC) -D_DEBUG -o vcnctd -liconv -logg -lvorbis -lvorbisenc -lvorbisfile -$(FFTW) $(WINSOCK)

clean:
	rm vcnctd

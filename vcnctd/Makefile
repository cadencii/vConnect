SRC=vcnctd.cpp \
	Config.cpp \
	daemon.c \
	../stand/mb_text.cpp \
	../stand/vConnect.cpp \
	../stand/standData.cpp \
	../stand/corpusManager.cpp \
	../stand/standFrame.cpp \
	../stand/vsqMetaText/vsqBase.cpp \
	../stand/vsqMetaText/vsqTempo.cpp \
	../stand/vsqMetaText/vsqHandle.cpp \
	../stand/vsqMetaText/vsqFileEx.cpp \
	../stand/utauVoiceDB/UtauDB.cpp \
	../stand/vsqMetaText/vsqEventList.cpp \
	../stand/vsqMetaText/vsqEventEx.cpp \
	../stand/vowelTable.cpp \
	../stand/vsqMetaText/vsqBPList.cpp \
	../stand/standSpecgram.cpp \
	../stand/world/star.cpp \
	../stand/world/platinum.cpp \
	../stand/world/matlabfunctions.cpp \
	../stand/waveFileEx/waveFileEx.cpp \
	../stand/world/synthesis.cpp \
	../stand/matching.cpp \
	../stand/vConnectSetting.cpp \
	../stand/standMelCepstrum.cpp \
	../stand/stand.cpp \
	../stand/world/platinum_v4.cpp \
	../stand/world/dio.cpp \
	../stand/world/synthesis_v4.cpp

HDR=Config.h \
	daemon.h \
	../stand/mb_text.h \
	../stand/vConnect.h \
	../stand/standData.h \
	../stand/corpusManager.h \
	../stand/standFrame.h \
	../stand/vsqMetaText/vsqBase.h \
	../stand/vsqMetaText/vsqTempo.h \
	../stand/vsqMetaText/vsqPhonemeDB.h \
	../stand/vsqMetaText/vsqHandle.h \
	../stand/vsqMetaText/vsqFileEx.h \
	../stand/utauVoiceDB/UtauDB.h \
	../stand/vsqMetaText/vsqEventList.h \
	../stand/vsqMetaText/vsqLyric.h \
	../stand/vsqMetaText/vsqEventEx.h \
	../stand/vowelTable.h \
	../stand/vsqMetaText/vsqBPList.h \
	../stand/standSpecgram.h \
	../stand/world/world.h \
	../stand/waveFileEx/waveFileEx.h \
	../stand/matching.h \
	../stand/vConnectSetting.h \
	../stand/standMelCepstrum.h \
	../stand/stand.h

vcnctd: $(SRC)
	g++ $(SRC) -o vcnctd -liconv -lfftw3 -lpthread

clean:
	rm vcnctd

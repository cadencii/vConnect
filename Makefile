SRC=./stand/corpusManager.cpp ./stand/main.cpp ./stand/stand.cpp \
    ./stand/vConnect.cpp ./stand/vConnectConverter.cpp \
    ./stand/vConnectPhoneme.cpp ./stand/vConnectUtility.cpp \
    ./stand/mb_text.cpp \
    \
    ./stand/utauVoiceDB/UtauDB.cpp ./stand/utauVoiceDB/utauFreq.cpp \
    \
    ./stand/vsqMetaText/vsqBase.cpp ./stand/vsqMetaText/vsqBPList.cpp \
    ./stand/vsqMetaText/vsqEventEx.cpp ./stand/vsqMetaText/vsqEventList.cpp \
    ./stand/vsqMetaText/vsqFileEx.cpp ./stand/vsqMetaText/vsqHandle.cpp \
    ./stand/vsqMetaText/vsqTempo.cpp \
    \
    ./stand/waveFileEx/waveFileEx.cpp \
    \
    ./stand/world/dio.cpp ./stand/world/platinum.cpp ./stand/world/star.cpp \
    ./stand/world/platinum_v4.cpp ./stand/world/synthesis.cpp \
    ./stand/world/synthesis_v4.cpp ./stand/world/matlabfunctions.cpp \
    ./stand/vConnectSetting.cpp

#we have more *.h's dependency
HEADER=./stand/mb_text.h ./stand/stand.h

#install path of FFTW (Windows)
PATHFFTW=
IPATHFFTW=-I$(PATHFFTW)
LPATHFFTW=-L$(PATHFFTW)
ifeq ($(PATHFFTW),)
  IPATHFFTW=
  LPATHFFTW=
endif

#lib name of FFTW; Windows -> fftw3-3, Mac&Linux -> fftw3
LIBFFT=fftw3

#Preparing FFTW libs for windows:
#    cd $(PATHFFTW)
#    lib /def:libfftw3-3.def
#    lib /def:libfftw3f-3.def
#    lib /def:libfftw3l-3.def
#    dlltool --add-stdcall-underscore -d libfftw3-3.def -l libfftw3-3.a
#    dlltool --add-stdcall-underscore -d libfftw3f-3.def -l libfftw3f-3.a
#    dlltool --add-stdcall-underscore -d libfftw3l-3.def -l libfftw3l-3.a

#get libiconv (in MinGW/MSYS)
# http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.13.tar.gz
# http://www2d.biglobe.ne.jp/~msyk/software/libiconv/libiconv-1.13-ja-1.patch.gz
# cd ~/src/libiconv-1.13/
# patch -p1 -N < ../libiconv-1.13-ja-1.patch
# ./configure --prefix=/mingw --enable-static --disable-shared
# make
# make install

vConnect-STAND.exe: $(SRC) $(HEADER)
	g++ -finput-charset=UTF-8 -D_STND_MULTI_THREAD -D_DEBUG -DUNICODE -g -s -O2 $(IPATHFFTW) $(SRC) $(LPATHFFTW) -lpthread -liconv -l$(LIBFFT) -logg -lvorbis -lvorbisfile -lvorbisenc -o vConnect-STAND.exe
#	g++ -finput-charset=UTF-8 -D_DEBUG -DUNICODE -s -O2 $(IPATHFFTW) $(SRC) $(LPATHFFTW) -lpthread -liconv -l$(LIBFFT) -o vConnect-STAND.exe

clean:
	$(RM) vConnect-STAND.exe

SRC=stand/*.cpp \
    stand/utauVoiceDB/*.cpp \
    stand/vsqMetaText/*.cpp \
    stand/waveFileEx/*.cpp \
    stand/world/*.cpp

HEADER=stand/*.h \
    stand/utauVoiceDB/*.h \
    stand/vsqMetaText/*.h \
    stand/waveFileEx/*.h \
    stand/world/*.h

OS=${shell uname}
ifeq ($(OS),Darwin)
    ARCH= -arch i386 -arch x86_64
else
    ARCH=
endif

#Preparing FFTW libs for windows:
#    cd $(PATHFFTW)
#    lib /def:libfftw3-3.def
#    lib /def:libfftw3f-3.def
#    lib /def:libfftw3l-3.def
#    dlltool --add-stdcall-underscore -d libfftw3-3.def -l libfftw3-3.a
#    dlltool --add-stdcall-underscore -d libfftw3f-3.def -l libfftw3f-3.a
#    dlltool --add-stdcall-underscore -d libfftw3l-3.def -l libfftw3l-3.a

all: vConnect-STAND.exe

vConnect-STAND.exe: $(SRC) $(HEADER) libiconv-1.13/lib/*.o
	g++ $(ARCH) -finput-charset=UTF-8 -DSTND_MULTI_THREAD -O2 $(SRC) libiconv-1.13/lib/*.o -lpthread -lfftw3 -logg -lvorbis -lvorbisfile -lvorbisenc -o vConnect-STAND.exe

libiconv-1.13.tar.gz:
	wget http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.13.tar.gz

libiconv-1.13-ja-1.patch.gz:
	wget http://www2d.biglobe.ne.jp/~msyk/software/libiconv/libiconv-1.13-ja-1.patch.gz

libiconv-1.13/lib/*.o: libiconv-1.13.tar.gz libiconv-1.13-ja-1.patch.gz
	rm -rf libiconv-1.13
	rm -f libiconv-1.13-ja-1.patch
	tar zxvf libiconv-1.13.tar.gz
	gunzip -c libiconv-1.13-ja-1.patch.gz > libiconv-1.13-ja-1.patch
	cd libiconv-1.13 && patch -p1 -N < ../libiconv-1.13-ja-1.patch
	cd libiconv-1.13 && ./configure --enable-static --disable-shared
	export O_CFLAGS=`cat libiconv-1.13/lib/Makefile | grep "^CFLAGS" | sed -e 's/^CFLAGS[ ]*=[ ]*\(.*\)$/\1/g'`
	rm -rf libiconv-1.13
	tar zxvf libiconv-1.13.tar.gz
	cd libiconv-1.13 && patch -p1 -N < ../libiconv-1.13-ja-1.patch
	cd libiconv-1.13 && ./configure --enable-static --disable-shared CFLAGS=" ${O_CFLAGS} $(ARCH) " && make

clean:
	rm -f vConnect-STAND.exe

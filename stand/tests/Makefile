SOURCE=./*.cpp ./*/*.cpp \
       ../Thread.cpp ../Mutex.cpp ../vsqMetaText/Lyric.cpp \
       ../utau/UtauDB.cpp
all: *.cpp *.h
	g++ -finput-charset=UTF-8 $(SOURCE) ../../libiconv-1.13/lib/*.o -lcppunit -o run
	./run

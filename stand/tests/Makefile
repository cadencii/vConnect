all: *.cpp *.h
	g++ -finput-charset=UTF-8 *.cpp ../Thread.cpp ../Mutex.cpp ../../libiconv-1.13/lib/*.o -lcppunit -o run
	./run

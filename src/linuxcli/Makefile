
INCLUDES=-I../include -I../third-party/utf8 -I../third-party/rapidxml

all: RHVoice

cli.o: cli.cpp Client.h
	g++ -c $(INCLUDES) cli.cpp

WaveFile.o: WaveFile.cpp WaveFile.h
	g++ -c $(INCLUDES) WaveFile.cpp

RHVoice: cli.o WaveFile.o
	g++ -oRHVoice cli.o WaveFile.o /usr/local/lib64/libRHVoice_core.so.0

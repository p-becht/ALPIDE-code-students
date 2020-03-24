CC=g++
GIT_VERSION:=$(shell git describe --dirty --always)
CFLAGS= -pipe -fPIC -DVERSION=\"$(GIT_VERSION)\" -g -std=c++0x
LINKFLAGS=-lusb-1.0
OBJECT= runTest
LIBRARY=libpalpidefs.so
CLASS=  USB.cpp TDaqboard.cpp TPalpidefs.cpp TDut.cpp TTestsetup.cpp chiptests.cpp TConfig.cpp TModuleSetup.cpp 
OBJS = $(CLASS:.cpp=.o)
CLASSCP=$(CLASS) TConfCurrentProfile.cpp
OBJSCP = $(CLASSCP:.cpp=.o)
all:	runTest

$(OBJECT):	$(OBJS) main.cpp
	$(CC) -o $@ $(OBJS) $(CFLAGS) main.cpp $(LINKFLAGS)

rawReader: $(OBJS) rawReader.cpp
	$(CC) -o $@ $(OBJS) $(CFLAGS) `root-config --cflags` rawReader.cpp $(LINKFLAGS) `root-config --glibs`

debugReader: $(OBJS) debugReader.cpp
	$(CC) -o $@ $(OBJS) $(CFLAGS) `root-config --cflags` debugReader.cpp $(LINKFLAGS) `root-config --glibs`

ADCDecoder:	$(OBJS) decodeADCData.cpp
		$(CC)  -pthread -o decodeADC  $(CFLAGS) decodeADCData.cpp $(LINKFLAGS)

ReadConf:	$(OBJS) ReadConfRegister.cpp
		$(CC) -o ReadReg $(OBJS) $(CFLAGS) ReadConfRegister.cpp $(LINKFLAGS)

lib:	$(OBJS)
	$(CC) -shared $(OBJS) $(LINKFLAGS) -o $(LIBRARY)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(OBJECT) $(LIBRARY)
	rm -rf readoutADC ReadReg decodeADC

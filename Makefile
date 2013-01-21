LIBS= \
	live555/liveMedia/libliveMedia.a \
	live555/groupsock/libgroupsock.a \
	live555/BasicUsageEnvironment/libBasicUsageEnvironment.a \
	live555/UsageEnvironment/libUsageEnvironment.a

INCFLAGS= \
	-Ilive555/UsageEnvironment/include \
	-Ilive555/BasicUsageEnvironment/include \
	-Ilive555/groupsock/include \
	-Ilive555/UsageEnvironment/include \
	-Ilive555/liveMedia/include

ifeq ("$(shell uname)", "Darwin")
    CPPFLAGS=-g -DBSD=1 -DSOCKLEN_T=socklen_t -DHAVE_SOCKADDR_LEN=1 -DTIME_BASE=int -Wall 
endif
ifeq ("$(shell uname)", "Linux")
    CPPFLAGS=-g -m64 -fPIC -I. -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -Wall -DBSD=1
endif
ifeq ("$(shell uname)", "CYGWIN")
    CPPFLAGS=-g -m64 -fPIC -I. -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -Wall -DBSD=1
endif

CPP=g++

all: wfdemu

wfdemu: openWFD.cpp playCommon.cpp WFDSink.cpp Makefile
	$(CPP) -c -o openWFD.o $(INCFLAGS) $(CPPFLAGS) openWFD.cpp
	$(CPP) -c -o playCommon.o $(INCFLAGS) $(CPPFLAGS) playCommon.cpp
	$(CPP) -c -o WFDSink.o $(INCFLAGS) $(CPPFLAGS) WFDSink.cpp
	$(CPP) -o wfdemu -L. playCommon.o openWFD.o WFDSink.o $(LIBS)

clean:
	rm -f *.o wfdemu


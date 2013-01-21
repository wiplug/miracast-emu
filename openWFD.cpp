// Cheng Yao @ 2013
// :-D -- Stolen from live555/testProgs/openRTSP.cpp

#include "playCommon.hh"

WFDSink* ourWFDSink = NULL;



Medium* createClient(UsageEnvironment& env, char const* url, int verbosityLevel, char const* applicationName) {
  extern portNumBits tunnelOverHTTPPortNum;
  return ourWFDSink = WFDSink::createNew(env, url, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

void setupSubsession(MediaSubsession* subsession, Boolean streamUsingTCP, WFDSink::responseHandler* afterFunc) {
  Boolean forceMulticastOnUnspecified = False;
  ourWFDSink->sendSETUPCommand(*subsession, afterFunc, False, streamUsingTCP, forceMulticastOnUnspecified, ourAuthenticator);
}

void startPlayingSession(MediaSession* session, double start, double end, float scale, WFDSink::responseHandler* afterFunc) {
  ourWFDSink->sendPLAYCommand(*session, afterFunc, start, end, scale, ourAuthenticator);
}

void startPlayingSession(MediaSession* session, char const* absStartTime, char const* absEndTime, float scale, WFDSink::responseHandler* afterFunc) {
  ourWFDSink->sendPLAYCommand(*session, afterFunc, absStartTime, absEndTime, scale, ourAuthenticator);
}

void tearDownSession(MediaSession* session, WFDSink::responseHandler* afterFunc) {
  ourWFDSink->sendTEARDOWNCommand(*session, afterFunc, ourAuthenticator);
}

Boolean allowProxyServers = False;
Boolean controlConnectionUsesTCP = True;
Boolean supportCodecSelection = False;
char const* clientProtocolName = "WFD";

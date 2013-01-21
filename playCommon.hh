// Cheng Yao @ 2013
// :-D -- Stolen from live555/testProgs/playCommon.hh


#include "liveMedia.hh"
#include "WFDSink.hh"

extern Medium* createClient(UsageEnvironment& env,
		                    char const* URL,
		                    int verbosityLevel,
		                    char const* applicationName);
extern WFDSink* ourWFDSink;
extern SIPClient* ourSIPClient;

extern void getOptions(WFDSink::responseHandler* afterFunc);

extern void getSDPDescription(WFDSink::responseHandler* afterFunc);

extern void setupSubsession(MediaSubsession* subsession,
		                    Boolean streamUsingTCP,
		                    WFDSink::responseHandler* afterFunc);

extern void startPlayingSession(MediaSession* session,
								double start,
								double end,
								float scale,
								WFDSink::responseHandler* afterFunc);

extern void startPlayingSession(MediaSession* session,
								char const* absStartTime,
								char const* absEndTime,
								float scale,
								WFDSink::responseHandler* afterFunc);
// For playing by 'absolute' time (using strings of the form
// "YYYYMMDDTHHMMSSZ" or "YYYYMMDDTHHMMSS.<frac>Z"

extern void tearDownSession(MediaSession* session,
						    WFDSink::responseHandler* afterFunc);

extern Authenticator* ourAuthenticator;
extern Boolean allowProxyServers;
extern Boolean controlConnectionUsesTCP;
extern Boolean supportCodecSelection;
extern char const* clientProtocolName;
extern unsigned statusCode;

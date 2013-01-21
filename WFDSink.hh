// Cheng Yao @ 2013
// :-D -- Stolen from live555/liveMedia/include/RTSPClient.hh

#ifndef _WFD_SINK_HH
#define _WFD_SINK_HH

#ifndef _MEDIA_SESSION_HH
#include "MediaSession.hh"
#endif
#ifndef _NET_ADDRESS_HH
#include "NetAddress.hh"
#endif
#ifndef _DIGEST_AUTHENTICATION_HH
#include "DigestAuthentication.hh"
#endif

#include <map>
#include <cstring>
#include <string>

struct ParsedMessage {
    static ParsedMessage Parse(
            const char *data, size_t size, bool noMoreData,
            size_t &length);

    bool findString(const char *name, std::string &value) const;
    bool findInt32(const char *name, int32_t &value) const;

    const char *getContent() const;
    bool isValid() const { return mValid; }
    void getRequestField(size_t index, std::string &field) const;
    bool getStatusCode(int32_t &statusCode) const;

    static bool GetAttribute(const char *s, const char *key, std::string& value);

    static bool GetInt32Attribute(
            const char *s, const char *key, int32_t &value);


//protected:
    virtual ~ParsedMessage();

private:
    typedef std::map<std::string, std::string>::iterator mapiter;
    std::map<std::string, std::string> mDict;
    std::string mContent;
    bool mValid;
    ParsedMessage();
    ssize_t parse(const char *data, size_t size, bool noMoreData);
};

class WFDSink: public Medium {
public:
    void startPlay();
  static WFDSink* createNew(UsageEnvironment& env, char const* wfdURL,
                   int verbosityLevel = 0,
                   char const* applicationName = NULL,
                   portNumBits tunnelOverHTTPPortNum = 0);
  typedef void (responseHandler)(WFDSink* wfdClient,
                 int resultCode, char* resultString);

  unsigned sendDESCRIBECommand(responseHandler* responseHandler, Authenticator* authenticator = NULL);
  unsigned sendM2(responseHandler* responseHandler, Authenticator* authenticator = NULL);
  unsigned sendSETUPCommand(MediaSubsession& subsession, responseHandler* responseHandler,
                Boolean streamOutgoing = False,
                Boolean streamUsingTCP = False,
                Boolean forceMulticastOnUnspecified = False,
                Authenticator* authenticator = NULL);
  unsigned sendPLAYCommand(MediaSession& session, responseHandler* responseHandler,
               double start = 0.0f, double end = -1.0f, float scale = 1.0f,
               Authenticator* authenticator = NULL);
  unsigned sendPLAYCommand(MediaSubsession& subsession, responseHandler* responseHandler,
               double start = 0.0f, double end = -1.0f, float scale = 1.0f,
               Authenticator* authenticator = NULL);
  unsigned sendPLAYCommand(MediaSession& session, responseHandler* responseHandler,
               char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
               Authenticator* authenticator = NULL);
  unsigned sendPLAYCommand(MediaSubsession& subsession, responseHandler* responseHandler,
               char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
               Authenticator* authenticator = NULL);
  unsigned sendTEARDOWNCommand(MediaSession& session, responseHandler* responseHandler, Authenticator* authenticator = NULL);
  unsigned sendTEARDOWNCommand(MediaSubsession& subsession, responseHandler* responseHandler, Authenticator* authenticator = NULL);
  Boolean changeResponseHandler(unsigned cseq, responseHandler* newResponseHandler);
  int socketNum() const { return fInputSocketNum; }
  static Boolean lookupByName(UsageEnvironment& env,
                  char const* sourceName,
                  WFDSink*& resultClient);
  static Boolean parseWFDURL(UsageEnvironment& env, char const* url,
                  char*& username, char*& password, NetAddress& address, portNumBits& portNum, char const** urlSuffix = NULL);
  void setUserAgentString(char const* userAgentName);
  unsigned sessionTimeoutParameter() const { return fSessionTimeoutParameter; }
  char const* url() const { return fBaseURL; }
  static unsigned responseBufferSize;

public: // Some compilers complain if this is "private:"
  // The state of a request-in-progress:
  class RequestRecord {
  public:
    RequestRecord(unsigned cseq, char const* commandName, responseHandler* handler,
          MediaSession* session = NULL, MediaSubsession* subsession = NULL, u_int32_t booleanFlags = 0,
          double start = 0.0f, double end = -1.0f, float scale = 1.0f, char const* contentStr = NULL);
    RequestRecord(unsigned cseq, responseHandler* handler,
          char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
          MediaSession* session = NULL, MediaSubsession* subsession = NULL);
        // alternative constructor for creating "PLAY" requests that include 'absolute' time values
    virtual ~RequestRecord();

    RequestRecord*& next() { return fNext; }
    unsigned& cseq() { return fCSeq; }
    char const* commandName() const { return fCommandName; }
    MediaSession* session() const { return fSession; }
    MediaSubsession* subsession() const { return fSubsession; }
    u_int32_t booleanFlags() const { return fBooleanFlags; }
    double start() const { return fStart; }
    double end() const { return fEnd; }
    char const* absStartTime() const { return fAbsStartTime; }
    char const* absEndTime() const { return fAbsEndTime; }
    float scale() const { return fScale; }
    char* contentStr() const { return fContentStr; }
    responseHandler*& handler() { return fHandler; }
    

  private:
    RequestRecord* fNext;
    unsigned fCSeq;
    char const* fCommandName;
    MediaSession* fSession;
    MediaSubsession* fSubsession;
    u_int32_t fBooleanFlags;
    double fStart, fEnd;
    char *fAbsStartTime, *fAbsEndTime; // used for optional 'absolute' (i.e., "time=") range specifications
    float fScale;
    char* fContentStr;
    responseHandler* fHandler;
  };

protected:
  WFDSink(UsageEnvironment& env, char const* wfdURL,
         int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
      // called only by createNew();
  virtual ~WFDSink();

  void reset();
  void setBaseURL(char const* url);
  virtual unsigned sendRequest(RequestRecord* request);

private: // redefined virtual functions
  virtual Boolean isWFDSink() const;

private:
  class RequestQueue {
  public:
    RequestQueue();
    RequestQueue(RequestQueue& origQueue); // moves the queue contents to the new queue
    virtual ~RequestQueue();

    void enqueue(RequestRecord* request); // "request" must not be NULL
    RequestRecord* dequeue();
    void putAtHead(RequestRecord* request); // "request" must not be NULL
    RequestRecord* findByCSeq(unsigned cseq);
    Boolean isEmpty() const { return fHead == NULL; }

  private:
    RequestRecord* fHead;
    RequestRecord* fTail;
  };

  void resetTCPSockets();
  void resetResponseBuffer();
  int openConnection(); // -1: failure; 0: pending; 1: success
  int connectToServer(int socketNum, portNumBits remotePortNum); // used to implement "openConnection()"; result values are the same
  char* createAuthenticatorString(char const* cmd, char const* url);
  void handleRequestError(RequestRecord* request);
  Boolean parseResponseCode(char const* line, unsigned& responseCode, char const*& responseString);
  void handleIncomingRequest();
  void handleOPTIONSRequest(const ParsedMessage &msg);
  void handleGET_PARAMETERRequest(const ParsedMessage &msg);
  void handleTriggerMethod(const ParsedMessage &msg);
  void handleSET_PARAMETERRequest(const ParsedMessage &msg);

  static Boolean checkForHeader(char const* line, char const* headerName, unsigned headerNameLength, char const*& headerParams);
  Boolean parseTransportParams(char const* paramsStr,
                   char*& serverAddressStr, portNumBits& serverPortNum,
                   unsigned char& rtpChannelId, unsigned char& rtcpChannelId);
  Boolean parseScaleParam(char const* paramStr, float& scale);
  Boolean parseRTPInfoParams(char const*& paramStr, u_int16_t& seqNum, u_int32_t& timestamp);
  Boolean handleOPTIONSResponse(MediaSubsession& subsession, char const* sessionParamsStr, char const* transportParamsStr);
  Boolean handleSETUPResponse(MediaSubsession& subsession, char const* sessionParamsStr, char const* transportParamsStr,
                  Boolean streamUsingTCP);
  Boolean handlePLAYResponse(MediaSession& session, MediaSubsession& subsession,
                             char const* scaleParamsStr, char const* rangeParamsStr, char const* rtpInfoParamsStr);
  Boolean handleTEARDOWNResponse(MediaSession& session, MediaSubsession& subsession);
  Boolean handleGET_PARAMETERResponse(char const* parameterName, char*& resultValueString);
  Boolean handleSET_PARAMETERResponse(char const* parameterName, char*& resultValueString);
  Boolean handleAuthenticationFailure(char const* wwwAuthenticateParamsStr);
  Boolean resendCommand(RequestRecord* request);
  char const* sessionURL(MediaSession const& session) const;
  static void handleAlternativeRequestByte(void*, u_int8_t requestByte);
  void handleAlternativeRequestByte1(u_int8_t requestByte);
  void constructSubsessionURL(MediaSubsession const& subsession,
                  char const*& prefix,
                  char const*& separator,
                  char const*& suffix);

  // Support for tunneling RTSP-over-HTTP:
  Boolean setupHTTPTunneling1(); // send the HTTP "GET"
  static void responseHandlerForHTTP_GET(WFDSink* wfdClient, int responseCode, char* responseString);
  void responseHandlerForHTTP_GET1(int responseCode, char* responseString);
  Boolean setupHTTPTunneling2(); // send the HTTP "POST"

  // Support for asynchronous connections to the server:
  static void connectionHandler(void*, int /*mask*/);
  void connectionHandler1();

  // Support for handling data sent back by a server:
  static void incomingDataHandler(void*, int /*mask*/);
  void incomingDataHandler1();
  void handleResponseBytes(int newBytesRead);


  void setupSession(const char* sdpDescription);
  

protected:
  int fVerbosityLevel;
  unsigned fCSeq; // sequence number, used in consecutive requests
  Authenticator fCurrentAuthenticator;

private:
  MediaSession* fSession;
  MediaSubsession* fSubsession;
  std::string fPresentationUrl;
  portNumBits fTunnelOverHTTPPortNum;
  char* fUserAgentHeaderStr;
  unsigned fUserAgentHeaderStrLen;
  int fInputSocketNum, fOutputSocketNum;
  netAddressBits fServerAddress;
  char* fBaseURL;
  unsigned char fTCPStreamIdCount; // used for (optional) RTP/TCP
  char* fLastSessionId;
  unsigned fSessionTimeoutParameter; // optionally set in response "Session:" headers
  char* fResponseBuffer;
  unsigned fResponseBytesAlreadySeen, fResponseBufferBytesLeft;
  RequestQueue fRequestsAwaitingConnection, fRequestsAwaitingHTTPTunneling, fRequestsAwaitingResponse;

  // Support for tunneling RTSP-over-HTTP:
  char fSessionCookie[33];
  unsigned fSessionCookieCounter;
  Boolean fHTTPTunnelingConnectionIsPending;
};
#endif

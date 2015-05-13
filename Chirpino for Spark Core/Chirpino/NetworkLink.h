/*
    CHIRPINO version 1.0
    This software is provided by ASIO Ltd trading as Chirp

        http://chirp.io

    and is distributed freely and without warranty under the Creative Commons licence CC BY-NC 3.0
  
        https://creativecommons.org/licenses/by-nc/3.0/
  
    Have fun
*/


#ifndef NETWORK_H
#define NETWORK_H

#include "Chirpino.h"
#include "Appender.h"


class ContentReader {
public:
    virtual void acceptResponse(char *responseContent) = 0;
    virtual void acceptProblemResponse(char *message) = 0;
    virtual void responseFinished() = 0;
};


class NetworkLink {
    protected:
        TCPClient client;
        uint32_t waitTimer;
        Appender *text; // wrapping send & receive buffer
        ContentReader *contentReader;

        char connectionStatus();
        bool setup();
        void readRawResponse();

    public:
        bool busy;

        NetworkLink(Appender *text);

        bool ready();
        bool sendWebRequest(const char *host, uint16_t port, const char *apiKey, const char *pathPrefix,
                const char *pathContinued, char *request, ContentReader *contentReader);
        void loop();
};


#endif

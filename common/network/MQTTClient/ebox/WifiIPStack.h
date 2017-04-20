#ifndef ARDUINOWIFIIPSTACK_H
#define ARDUINOWIFIIPSTACK_H

#include "ebox.h"
#include "esp8266.h"
#include "esp8266_tcp.h"


class WifiIPStack
{
public:
    WifiIPStack(WIFI_TCP* ptcp)
    {
        tcp = ptcp;
        tcp->begin();
    }

    int        connect(char* hostname, int port)
    {
        return tcp->connect(hostname, port, 7200);
    }

//    int connect(uint32_t hostname, int port)
//    {
//       return tcp->connect(hostname, port, 7200);
//    }

    int        read(char* buffer, int len, int timeout = 1000)
    {
        //set timeout
        if( tcp->available())
            for(int i = 0; i < len; ++i)
            {
                if( tcp->available() > 0 )
                    buffer[i] = tcp->read_onebyte();
                else
                    return i;
            }
        return 0;
    }

    int        write(char* buffer, int len, int timeout = 1000)
    {
        //set timeout
        return tcp->send((uint8_t*)buffer, len);
    }

    int        disconnect()
    {
        return tcp->disconnect();
    }

private:
    WIFI_TCP *      tcp;

};

#endif


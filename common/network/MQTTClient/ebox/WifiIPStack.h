#ifndef ARDUINOWIFIIPSTACK_H
#define ARDUINOWIFIIPSTACK_H

#include "ebox.h"
#include "esp8266.h"
#include "esp8266_tcp.h"


class WifiIPStack
{
public:
    WifiIPStack()
    {
        wifi.begin(&PA4, &uart2, 115200);
        wifi.join_ap();
        tcp.begin();
    }

    int        connect(char* hostname, int port)
    {
        return tcp.connect(hostname, port, 7200);
    }

//    int connect(uint32_t hostname, int port)
//    {
//       return tcp.connect(hostname, port, 7200);
//    }

    int        read(char* buffer, int len, int timeout)
    {
        //set timeout
        for(int i = 0; i < len; ++i)
        {
            if( tcp.available())
                buffer[i] = tcp.read_onebyte();
            else
                return i;
        }

        return len;
    }

    int        write(char* buffer, int len, int timeout)
    {
        //set timeout
        return tcp.send((uint8_t*)buffer, len);
    }

    int        disconnect()
    {
        return tcp.disconnect();
    }

private:

    WIFI_TCP        tcp;


};

#endif


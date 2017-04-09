#if !defined(ETHIPSTACK_H)
#define ETHIPSTACK_H

#include "ebox.h"
#include "tcp.h"
#include "dns.h"
#include "w5500.h"

u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};
u8 ip[4] = {192, 168, 1, 119};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 1, 1};
u8 dns[4] = {192, 168, 1, 1};
extern W5500 w5500(&PC13, &PC14, &PC15, &spi2);

class EthIPStack 
{
public:    
    EthIPStack()
    {
		
		tcp.begin(SOCKET7, 3000);
    }
	
	int dnsquery(char* name)
	{
//		int ret;
//		ret = ddns.query(name);
//		if ( ret == 1 )
//		{
//			hostip[0] = ddns.domain_ip[0];
//			hostip[1] = ddns.domain_ip[1];
//			hostip[2] = ddns.domain_ip[2];
//			hostip[3] = ddns.domain_ip[3];
//		}
//		
//		return ret;
		hostip[0] = 198;
		hostip[1] = 41;
		hostip[2] = 30;
		hostip[3] = 241;
		return 0;
	}
    
    int connect(char* hostname, int port)
    {
		dnsquery(hostname);
		return tcp.connect(hostip, port);
    }

    int connect(uint32_t hostname, int port)
    {
		hostip[0] = (hostname & 0xFF000000) >> 3; 
		hostip[1] = (hostname & 0x00FF0000) >> 2;
		hostip[2] = (hostname & 0x0000FF00) >> 1;
		hostip[3] = (hostname & 0x000000FF);
        return tcp.connect(hostip, port);
    }

    int read(unsigned char* buffer, int len, int timeout)
    {
        int interval = 10;  // all times are in milliseconds
		int total = 0, rc = -1;

		if (timeout < 30)
			interval = 2;
		while (tcp.available() < len && total < timeout)
		{
			delay_us(interval);
			total += interval;
		}
		if (tcp.available() >= len)
			rc = tcp.recv((uint8_t*)buffer, len);
		return rc;
    }
    
    int write(unsigned char* buffer, int len, int timeout)
    {
        //tcp.setTimeout(timeout);  
		return tcp.send((uint8_t*)buffer, len);
    }
    
    int disconnect()
    {
        tcp.stop();
        return 0;
    }

private:

    TCPCLIENT tcp;
	DNS ddns;
	u8 hostip[4];
};

#endif




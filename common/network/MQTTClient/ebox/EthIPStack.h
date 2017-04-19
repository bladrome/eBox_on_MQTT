#if !defined(ETHIPSTACK_H)
#define ETHIPSTACK_H

#include "ebox.h"
#include "tcp.h"
#include "dns.h"
#include "w5500.h"

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

    int connect(u8 hostname[], int port)
    {
		//uart1.printf("Calling conncect hostname\n");
		return tcp.connect(hostname, port);
    }

    int read(unsigned char* buffer, int len, int timeout = 500)
    {
        int interval = 10;  // all times are in milliseconds
		int total = 0, rc = -1;

		if (timeout < 30)
			interval = 2;
		while (tcp.available() < len && total < timeout)
		{
			delay_ms(interval);
			total += interval;
		}
		if (tcp.available() >= len)
			rc = tcp.recv((uint8_t*)buffer, len);
		//uart1.printf("Calling read \n");
		return rc;
    }
    
    int write(unsigned char* buffer, int len, int timeout = 500)
    {
        //tcp.setTimeout(timeout);  
		//uart1.printf("Calling write \n");
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




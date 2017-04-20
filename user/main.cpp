/*
 *  file   : *.cpp
 *  author : shentq
 *  version: V1.0
 *  date   : 2015/7/5
 *
 *  Copyright 2015 shentq. All Rights Reserved.
 */

//STM32 RUN IN eBox
#include "ebox.h"
#include "esp8266.h"
#include "esp8266_tcp.h"
#include "WifiIPStack.h"
#include "MQTTClient.h"


WIFI_TCP tcp(&wifi);
WifiIPStack ipstack(&tcp);

char remote_ip[] = "192.168.1.197";
uint16_t remote_port = 8080;
uint16_t local_port = 4321;


uint8_t recv_buf[1024] = {0};
uint8_t send_buf[] = "this is a udp send test!\r\n";
uint16_t len = 0;
uint32_t count = 0;

void setup()
{
    ebox_init();
    uart1.begin(115200);
    uart1.printf("esp8266 tcp single client test\r\n");
    uart1.printf("-------------------------------\r\n");

    wifi.begin();
    wifi.join_ap();

}

int main(void)
{
    bool ret;
    setup();

    ret = ipstack.connect(remote_ip, remote_port);
    if(ret)
    {
        uart1.printf("connect ok!\r\n");
    }
    else
    {
        uart1.printf("connect failed!\r\n");

    }

    while(1)
    {
        len = ipstack.read((char *)recv_buf,13);
        if(len)
        {
            uart1.printf((const char*)recv_buf);
        }
//        if(count == 0)
        {
            // ret = ipstack.write((char *)send_buf, sizeof(send_buf));
            ret = ipstack.write((char *)"send string test1234567890", 26);
            if(ret)
                uart1.printf("send ok!\r\n");
        }
//        count++;
//        count %= 500000;

    }

}






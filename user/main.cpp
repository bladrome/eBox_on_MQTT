/**
 ******************************************************************************
 * @file   : *.cpp
 * @author : shentq
 * @version: V1.2
 * @date   : 2016/08/14
 *
 * @brief   ebox application example .
 *
 * Copyright 2016 shentq. All Rights Reserved.
 ******************************************************************************
 */

#include "ebox.h"
#include "WifiIPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "WString.h"
#include "dht11.h"
#include "colorled.h"
#include "lcd_1.8.h"
#include "color_convert.h"
#include "gui.h"

#define MQTT_SSID             "LION"
#define MQTT_PASSWORD         "zijicaia"
#define MQTT_SERVER_HOSTNAME  "192.168.1.197"
//#define MQTT_SERVER_HOSTNAME  "iot.eclipse.org"
#define MQTT_SERVER_PORT      1883

// Yiled time in ms.
const int QOS0_YILED_TIME = 100;
const int QOS1_YILED_TIME = 2000;
const int QOS2_YILED_TIME = 2000;

// Packages counter.
int arrivedcount = 0;
int ledcolorstate = 0;
const char chledcolorstate[3][8] = {"red", "green", "blue"};
// Topics.
const char* topic = "eBox-sample/#";
const char* topic_fan = "eBox-sample/fan";
const char* topic_colorled = "eBox-sample/colorled";
const char* topic_temperature = "eBox-sample/temperature";
const char* topic_humidity = "eBox-sample/humidity";
const char* topic_counter = "eBox-sample/counter";

// MQTT Network stack.
WIFI_TCP tcp(&wifi);
WifiIPStack ipstack(&tcp);
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

// Sensor and executor
Dht11 sensor(&PB2);
COLORLED led(&PB7, &PB8, &PB9);


// LCD display
Lcd lcd(&PB5, &PB6, &PB4, &PB3, &spi1);
GUI gui(&lcd, 128, 160);

void connect(void);
void messageArrived(MQTT::MessageData& md);
void messageLEDcommand(MQTT::MessageData& md);
void messageFancommand(MQTT::MessageData& md);

void setup()
{
    ebox_init();
    uart1.begin(115200);
    led.begin();
    led.color_rgb(255, 0, 0);
    PB10.mode(OUTPUT_PP);
	lcd.begin(1);
	gui.begin();
	gui.fill_screen(RED);
    wifi.begin();
    wifi.join_ap(MQTT_SSID, MQTT_PASSWORD);
    connect();
}

int main()
{
    int rc = 0;
    int count = 0;
    setup();
    while(1)
    {

        if (!client.isConnected())
            connect();

        MQTT::Message message;

        sensor.read();
        // Send temperature.
        String strtemperature = String(sensor.getTemperature()) + String("C");
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)strtemperature.c_str();
        message.payloadlen = strtemperature.length() + 1;
        rc = client.publish(topic_temperature, message);
        if (rc != 0 )
            uart1.printf("Error %d from sending temperature message\n", rc);
        else
            client.yield(QOS0_YILED_TIME);

        // Send humidity
        String strhumidity = String(sensor.getHumidity()) + String("%");
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)strhumidity.c_str();
        message.payloadlen = strhumidity.length() + 1;
        rc = client.publish(topic_humidity, message);
        if (rc != 0 )
            uart1.printf("Error %d from sending humidity message\n", rc);
        else
            client.yield(QOS0_YILED_TIME);

        // Send counter
        String strcount = String(count);
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)strcount.c_str();
        message.payloadlen = strcount.length() + 1;
        rc = client.publish(topic_counter, message);
        if (rc != 0 )
            uart1.printf("Error %d from sending counter message\n", rc);
        else
            client.yield(QOS0_YILED_TIME);

//        // Send and receive QoS 1 message
//        uart1.printf("Hello World!  QoS 1 message\n");
//        message.qos = MQTT::QOS1;
//        message.payloadlen = strlen(buf) + 1;
//        rc = client.publish(topic, message);
//        if (rc != 0 )
//            uart1.printf("Error %d from sending QoS 1 message\n", rc);
//        else
//            client.yield(1000);

//        // Send and receive QoS 2 message
//        uart1.printf("Hello World!  QoS 2 message\n");
//        message.qos = MQTT::QOS2;
//        message.payloadlen = strlen(buf) + 1;
//        rc = client.publish(topic, message);
//        if (rc != 0 )
//            uart1.printf("Error %d from sending QoS 2 message\n", rc);
//        else
//            client.yield(1000);

//		sensor.read();
//		uart1.printf("Temperature:%d  Humidity:%d\n", sensor.getTemperature(), sensor.getHumidity());
//        String strdata = String("  Temperature:") + String(sensor.getTemperature()) +
//		                 String("  Humidity:")    + String(sensor.getHumidity());
//		message.qos = MQTT::QOS0;
//        message.retained = false;
//        message.dup = false;
//        message.payload = (void*)strdata.c_str();
//        message.payloadlen = strdata.length() + 1;
//        rc = client.publish(topic, message);
//        if (rc != 0 )
//            uart1.printf("Error %d from sending QoS 0 Sensor message\n", rc);
//        else
//            client.yield(1000);
//		delay_ms(500);
		String strline;
		lcd.clear2(RED, 8 * 10, 0, 160, 60);
		gui.set_text_mode(LCD_TEXTMODE_TRANS);
		gui.set_font(&GUI_FontHZ16X16);
		gui.set_color(BLUE);
		
		strline = String("Temperature:") + String(sensor.getTemperature()) + String("C");
		gui.set_cursor(16, 0);
		gui.disp_string(strline.c_str());
		strline = String("Humidity:       ") + String(sensor.getHumidity()) + String("%");
		gui.set_cursor(16, 16);
		gui.disp_string(strline.c_str());
		strline = String("Led color:      ") + String(chledcolorstate[ledcolorstate]);
		gui.set_cursor(16, 32);
		gui.disp_string(strline.c_str());
		strline = String("Counter:        ") + String(count);
		gui.set_cursor(16, 48);
		gui.disp_string(strline.c_str());
		
        count++;
        if(count == 1000)
            count = 0;

    }

    return 0;
}

void connect()
{
    uart1.printf("Connecting to %s:%d\n", MQTT_SERVER_HOSTNAME, MQTT_SERVER_PORT);
    int rc = ipstack.connect(MQTT_SERVER_HOSTNAME, MQTT_SERVER_PORT);
    if (rc != 1)
        uart1.printf("rc from TCP connect failed with %d\n", rc);

    uart1.printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*)"eBox-sample-id0F0F0F";
    rc = client.connect(data);
    if (rc != 0)
        uart1.printf("rc from MQTT connect is %d failed\n", rc);
    else
        uart1.printf("MQTT connected\n");

    // Subscribe ALL topics
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0)
        uart1.printf("rc from MQTT subscribe eBox-sample is %d\n", rc);
    else
        uart1.printf("MQTT subscribed\n");

    // Subscribe colorled
    rc = client.subscribe(topic_colorled, MQTT::QOS2, messageLEDcommand);
    if (rc != 0)
        uart1.printf("rc from MQTT subscribe eBox-sample/colorled is %d\n", rc);
    else
        uart1.printf("MQTT subscribed eBox-sample/colorled\n");

    // Subscribe fan
    rc = client.subscribe(topic_fan, MQTT::QOS2, messageFancommand);
    if (rc != 0)
        uart1.printf("rc from MQTT subscribe eBox-sample/fan is %d\n", rc);
    else
        uart1.printf("MQTT subscribed eBox-sample/fan\n");

    // publish colorled state
    String strledcolorstate = String(chledcolorstate[ledcolorstate]);
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)strledcolorstate.c_str();
    message.payloadlen = strledcolorstate.length() + 1;
    rc = client.publish(topic_colorled, message);
    if (rc != 0 )
        uart1.printf("Error %d from sending color led state message\n", rc);
    else
        client.yield(QOS0_YILED_TIME);
}


void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    uart1.printf("Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n",
                 ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    uart1.printf("Payload %s\n", (char*)message.payload);
}

void messageLEDcommand(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    String rgb = String((char*)message.payload);
    String rgbindex = String(rgb[0]);
    uint8_t color[3] = {0};
    ledcolorstate = rgbindex.toInt();
    color[ledcolorstate] = 255;
    led.color_rgb(color[0], color[1], color[2]);
}

void messageFancommand(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    String pl = String((char*)message.payload);
    String turn = String(pl[0]);
    PB10.write(turn.toInt());
}

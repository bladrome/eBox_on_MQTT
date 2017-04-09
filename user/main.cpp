#include "ebox.h"

#include <EthIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>


//#define MQTTCLIENT_QOS2 1
char printbuf[100];

int arrivedcount = 0;

void messageArrived(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;

  sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n",
          ++arrivedcount, message.qos, message.retained, message.dup, message.id);
  uart1.printf(printbuf);
  sprintf(printbuf, "Payload %s\n", (char*)message.payload);
  uart1.printf(printbuf);
}


EthIPStack ipstack(c);
MQTT::Client<EthIPStack, Countdown> client = MQTT::Client<EthIPStack, Countdown>(ipstack);

//uint8_t mac[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };  // replace with your device's MAC
const char* topic = "ebox-sample";

void connect()
{
  char hostname[] = "iot.eclipse.org";
  int port = 1883;
  sprintf(printbuf, "Connecting to %s:%d\n", hostname, port);
  uart1.printf(printbuf);
  int rc = ipstack.connect(hostname, port);
  if (rc != 1)
  {
    sprintf(printbuf, "rc from TCP connect is %d\n", rc);
    uart1.printf(printbuf);
  }

  uart1.printf("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*)"arduino-sample";
  rc = client.connect(data);
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT connect is %d\n", rc);
    uart1.printf(printbuf);
  }
  uart1.printf("MQTT connected");

  rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
    uart1.printf(printbuf);
  }
  uart1.printf("MQTT subscribed");
}

void setup()
{
  uart1.begin(9600);
  //Ethernet.begin(mac); // replace by Bridge.begin() if running on a Yun
  //Bridge.begin();
  uart1.printf("MQTT Hello example");
  connect();
}

void loop()
{
  if (!client.isConnected())
    connect();

  MQTT::Message message;

  arrivedcount = 0;

  // Send and receive QoS 0 message
  char buf[100];
  sprintf(buf, "Hello World! QoS 0 message");
  uart1.printf(buf);
  message.qos = MQTT::QOS0;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf) + 1;
  int rc = client.publish(topic, message);
  while (arrivedcount == 0)
    client.yield(1000);

  // Send and receive QoS 1 message
  sprintf(buf, "Hello World!  QoS 1 message");
  uart1.printf(buf);
  message.qos = MQTT::QOS1;
  message.payloadlen = strlen(buf) + 1;
  rc = client.publish(topic, message);
  while (arrivedcount == 1)
    client.yield(1000);

  // Send and receive QoS 2 message
  sprintf(buf, "Hello World!  QoS 2 message");
  uart1.printf(buf);
  message.qos = MQTT::QOS2;
  message.payloadlen = strlen(buf) + 1;
  rc = client.publish(topic, message);
  while (arrivedcount == 2)
    client.yield(1000);

  delay_ms(2000);
}

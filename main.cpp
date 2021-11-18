#include "MQTTClient.h"
#include "Socket.h"
#include "SocketAddress.h"
#include "mbed.h"
#include "WIZnetInterface.h"
#include <cstdint>
#include "TCPSocket.h"
#include "MQTTClientMbedOs.h"
#include <memory>
#include "nsapi_types.h"
#include "cmsis_os2.h"


//uint8_t mac = 0X2c549188c9e3;
unsigned short  int port = 1884;
MQTTPacket_connectData mqttSes = MQTTPacket_connectData_initializer;
SocketAddress sock;

int main()
{
    printf("Begin: \n");
    WIZnetInterface eth(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS, SPI_RESET);
    printf("1");
    eth.init();
    printf(" 2");
    int result = eth.connect();
    sock.set_ip_address("192.168.1.99"); //Broker's IP
    sock.set_port(port);
    
    SocketAddress a;

    TCPSocket socket;

    MQTTClient client(&socket);
    socket.open(&eth);
    nsapi_error_t res = socket.connect(sock);

    printf("3");

    if(res == 0)
        printf("Connesso al socket del broker\n");
    else
        printf("Impossibile connettersi al socket del broker\n");


    res = client.connect(mqttSes);

    if(client.isConnected())
        printf("Client connesso al broker\n");
    else
        printf("Impossibile connettere il client al broker\n");

    client.subscribe("Temperature", MQTT::QOS1, [](MQTT::MessageData &md){  
                                                                            MQTT::Message &message = md.message;
                                                                            printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
                                                                            printf("Payload MQTT:%.*s\r\n", message.payloadlen, (char *)message.payload);
                                                                            }
                                                                            );
    MQTT::Message MSG;
    MSG.qos = MQTT::QOS1;
    MSG.retained = 0;
    MSG.dup = 0;
    char greeting[] = "CIao";              
    MSG.payload = &greeting;
    MSG.payloadlen = sizeof(greeting);      
    MSG.id = 0;            

    client.publish("Temperature", MSG);

    printf("%d\n", res);
    client.yield(10000);
    osDelay(5000);
    printf("Disconnecting...\n");
    client.disconnect();
    socket.close();
    eth.disconnect();
    printf("Done\n");

}


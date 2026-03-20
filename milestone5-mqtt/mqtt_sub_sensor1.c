#include "MQTTClient.h"
#include <string.h>
#include <stdio.h>

#define ADDRESS     "tcp://localhost:1884"
#define CLIENTID    "SubClient1"
#define TOPIC       "chat/sensor1"
#define QOS         1

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    printf("Message arrived on topic %s: ", topicName);

    char buf[1024];
    memcpy(buf, message->payload, message->payloadlen);
    buf[message->payloadlen] = '\0';
    printf("%s\n", buf);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1; // Must return int
}


int main() {
    MQTTClient client;
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect\n");
        return -1;
    }

    MQTTClient_subscribe(client, TOPIC, QOS);

    while(1) {
        // Keep running to receive messages
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
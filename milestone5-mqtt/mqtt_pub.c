#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADDRESS     "tcp://localhost:1884"
#define CLIENTID    "PublisherClient"
#define TIMEOUT     10000L

// Define topics as macros
#define TOPIC1      "chat/sensor1"
#define TOPIC2      "chat/sensor2"
#define TOPIC3      "chat/sensor3"

// Delivery complete callback
void deliveryComplete(void *context, MQTTClient_deliveryToken dt) {
    printf("Delivery confirmed for token %d\n", dt);
}

int main() {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;   // seconds
    conn_opts.cleansession = 1;

    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

    // Register the delivery callback
    MQTTClient_setCallbacks(client, NULL, NULL, NULL, deliveryComplete);

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to broker at %s\n", ADDRESS);

    char buffer[256];
    int qos_choice, topic_choice;
    const char* topic;

    while (1) {
        printf("\nChoose topic: 1) %s  2) %s  3) %s  (0 to exit):\n",
               TOPIC1, TOPIC2, TOPIC3);
        if (scanf("%d", &topic_choice) != 1 || topic_choice == 0) break;
        getchar(); // consume newline

        switch (topic_choice) {
            case 1: topic = TOPIC1; break;
            case 2: topic = TOPIC2; break;
            case 3: topic = TOPIC3; break;
            default: printf("Invalid choice\n"); continue;
        }

        printf("Enter QoS level (0,1,2): ");
        if (scanf("%d", &qos_choice) != 1) break;
        getchar(); // consume newline

        printf("Enter message: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        buffer[strcspn(buffer, "\n")] = '\0';

        pubmsg.payload = buffer;
        pubmsg.payloadlen = (int)strlen(buffer);
        pubmsg.qos = qos_choice;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, topic, &pubmsg, &token);
        printf("Message published to [%s] with QoS %d: %s\n",
               topic, qos_choice, buffer);

        // MQTTClient_waitForCompletion(client, token, TIMEOUT); // Wait for delivery confirmation (optional, since we have a callback)
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
# Makefile for MQTT Publisher and Subscriber (Paho C)

CC = gcc
CFLAGS = -Wall -O2

# Source files
PUB_SRC = mqtt_pub.c
SUB_SRC1 = mqtt_sub_sensor1.c
SUB_SRC2 = mqtt_sub_sensor2.c

# Output binaries
PUB_BIN = mqtt_pub
SUB_BIN1 = mqtt_sub1
SUB_BIN2 = mqtt_sub2

# Libraries
PUB_LIBS = -lpaho-mqtt3c
SUB_LIBS = -lpaho-mqtt3a

all: $(PUB_BIN) $(SUB_BIN1) $(SUB_BIN2)

$(PUB_BIN): $(PUB_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(PUB_LIBS) $(SUB_LIBS)

$(SUB_BIN1): $(SUB_SRC1)
	$(CC) $(CFLAGS) -o $@ $^ $(SUB_LIBS) $(PUB_LIBS)

$(SUB_BIN2): $(SUB_SRC2)
	$(CC) $(CFLAGS) -o $@ $^ $(SUB_LIBS) $(PUB_LIBS)

clean:
	rm -f $(PUB_BIN) $(SUB_BIN1) $(SUB_BIN2)
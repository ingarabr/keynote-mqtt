#!/usr/bin/python

# pip install paho-mqtt

import os
import sys
import paho.mqtt.client as mqtt

def nextSlide():
    print("=> Next slide")
    cmd = """osascript -e 'tell application "System Events" to keystroke "n"'"""
    os.system(cmd)

def prevSlide():
     print("=> Previous slide")
     cmd = """osascript -e 'tell application "System Events" to keystroke "p"'"""
     os.system(cmd)

def onConnect(topic):
    def innerOnConnect(client, userdata, flags, rc):
        print("Connected with result code " + str(rc) + " to topic")
        client.subscribe(topic + "/#")
    return innerOnConnect

def onMessage(topic):
    def innerOnMessage(client, userdata, msg):
        if msg.topic == topic + "/keynote":
            if msg.payload == "next":
                nextSlide()
            elif msg.payload == "prev":
                prevSlide()
            else:
                print("=> Unknown cmd " + msg.payload)
    return innerOnMessage

args = sys.argv
if len(args) != 4:
    print("Expecting arguments mqtt 'host' and 'port'")
    print("Args: " + str(args))
    sys.exit(-1)

host = args[1]
port = int(args[2])
topic = args[3]

client = mqtt.Client()
client.on_connect = onConnect(topic)
client.on_message = onMessage(topic)

client.connect(host, port, 60)

client.loop_forever()


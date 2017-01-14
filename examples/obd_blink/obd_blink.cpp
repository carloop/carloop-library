/* Blink Carloop LED on OBD activity
 *
 * Blinks the blue LED when OBD CAN messages are received.
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */

#include "application.h"
//#include "carloop.h"
#include "carloop.h"

SYSTEM_MODE(MANUAL);

void sendObdRequest();
void waitForObdResponse();
void delayUntilNextRequest();

Carloop<CarloopRevision2> carloop;

// ODB CAN message IDs
const auto OBD_REQUEST_ID      = 0x7E0;
const auto OBD_REPLY_ID        = 0x7E8;

const auto OBD_PID_SERVICE     = 0x01;

// ODB PID constants
const auto ENGINE_COOLANT_TEMP = 0x05;
const auto ENGINE_RPM          = 0x0C;
const auto VEHICLE_SPEED       = 0x0D;
const auto MAF_SENSOR          = 0x10;
const auto O2_VOLTAGE          = 0x14;
const auto THROTTLE    		   = 0x11;

const uint8_t pid = ENGINE_RPM;
const int ledPin = D7;

auto *obdLoopFunction = sendObdRequest;
unsigned long transitionTime = 0;

void setup()
{
    pinMode(ledPin, OUTPUT);
    carloop.begin();

    transitionTime = millis();
}

void loop()
{
    carloop.update();
    obdLoopFunction();
}

void sendObdRequest() {
    CANMessage message;
    message.id = OBD_REQUEST_ID;
    message.len = 8;
    message.data[0] = 0x02;
    message.data[1] = OBD_PID_SERVICE;
    message.data[2] = pid;
    carloop.can().transmit(message);

    obdLoopFunction = waitForObdResponse;
    transitionTime = millis();
}

void waitForObdResponse() {
    if(millis() - transitionTime >= 100) {
        obdLoopFunction = delayUntilNextRequest;
        transitionTime = millis();
        return;
    }

    bool responseReceived = false;
    CANMessage message;
    while(carloop.can().receive(message)) {
        if(message.id == OBD_REPLY_ID && message.data[2] == pid) {
            responseReceived = true;
        }
    }

    if(responseReceived) {
        digitalWrite(ledPin, HIGH);
    }
}

void delayUntilNextRequest() {
    if(millis() - transitionTime >= 400) {
        obdLoopFunction = sendObdRequest;
        transitionTime = millis();
    } else if(millis() - transitionTime >= 250) {
        digitalWrite(ledPin, LOW);
    }
}

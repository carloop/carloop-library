/* Minimal Carloop example app
 *
 * Reads battery voltage, counts CAN messages and report GPS position to
 * USB serial once per second
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */

#include "application.h"
#include "carloop.h"

void updateCanMessageCount();
void printValuesAtInterval();
void printValues();
void printFloat(float val, bool valid, int len, int prec);
void printDateTime(TinyGPSDate &d, TinyGPSTime &t);

Carloop<CarloopRevision2> carloop;

int canMessageCount = 0;

void setup()
{
    Serial.begin(115200);
    carloop.begin();
}

void loop()
{
    carloop.update();
    updateCanMessageCount();
    printValuesAtInterval();
}

void updateCanMessageCount()
{
    CANMessage message;
    while(carloop.can().receive(message))
    {
        canMessageCount++;
    }
}

void printValuesAtInterval() {
    static const unsigned long interval = 1000;

    static unsigned long lastDisplay = 0;
    if(millis() - lastDisplay < 1000)
    {
        return;
    }
    lastDisplay = millis();
    printValues();
}

void printValues()
{
    auto &gps = carloop.gps();
    // Ensure that the GPS state doesn't change while printing
    WITH_LOCK(gps) {
        Serial.printf("Battery voltage: %12f ", carloop.battery());
        Serial.printf("CAN messages: %12d ", canMessageCount);
        Serial.print("GPS ");
        printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
        printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
        printDateTime(gps.date, gps.time);
        Serial.printf("%6d chars, %d checksum error", gps.charsProcessed(), gps.failedChecksum());
        Serial.println("");
    }
}

void printFloat(float val, bool valid, int len, int prec)
{
    if(!valid)
    {
        while(len-- > 1)
        {
            Serial.print('*');
        }
        Serial.print(' ');
    }
    else
    {
        char format[10];
        snprintf(format, sizeof(format), "%%%d.%df", len, prec);
        Serial.printf(format, val);
    }
}

void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
    if (!d.isValid())
    {
        Serial.print("********** ");
    }
    else
    {
        Serial.printf("%02d/%02d/%02d ", d.month(), d.day(), d.year());
    }

    if (!t.isValid())
    {
        Serial.print("******** ");
    }
    else
    {
        Serial.printf("%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    }
}


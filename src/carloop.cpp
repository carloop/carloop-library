/* Library for the Carloop connected car device
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */

#include "carloop.h"
#include <functional>

template<typename Config>
Carloop<Config>::Carloop()
    : canDriver(Config::CAN_PINS),
    canSpeed(Config::CAN_DEFAULT_SPEED),
    gpsSerialThread(NULL)
{
}

template <typename Config>
void Carloop<Config>::setCANSpeed(uint32_t canSpeed)
{
    this->canSpeed = canSpeed;
}

template <typename Config>
void Carloop<Config>::begin(CarloopFeatures_e features)
{
    this->features = features;

    if(hasCAN())
    {
        enableCAN();
    }

    if(hasGPS())
    {
        enableGPS();
    }

    if(hasBattery())
    {
        enableBattery();
    }
}

template <typename Config>
void Carloop<Config>::update()
{
    if(hasBattery())
    {
        batteryVoltage = readBattery();
    }
}

template <typename Config>
CANChannel &Carloop<Config>::can()
{
    return canDriver;
}

template <typename Config>
TinyGPSPlus &Carloop<Config>::gps()
{
    return gpsDriver;
}

template <typename Config>
float Carloop<Config>::battery()
{
    return batteryVoltage;
}

template <typename Config>
void Carloop<Config>::enableCAN()
{
    pinMode(Config::CAN_ENABLE_PIN, OUTPUT);
    digitalWrite(Config::CAN_ENABLE_PIN, Config::CAN_ENABLE_ACTIVE);
    canDriver.begin(canSpeed);
}

template <typename Config>
void Carloop<Config>::disableCAN()
{
    canDriver.end();
    digitalWrite(Config::CAN_ENABLE_PIN, Config::CAN_ENABLE_INACTIVE);
}

template <typename Config>
void Carloop<Config>::enableGPS()
{
    pinMode(Config::GPS_ENABLE_PIN, OUTPUT);
    digitalWrite(Config::GPS_ENABLE_PIN, Config::GPS_ENABLE_ACTIVE);

    Serial1.begin(Config::GPS_BAUD_RATE);

    if (!gpsSerialThread) {
        // Start a thread that will run this->receiveSerialChars()
        gpsSerialThread = new Thread("gps_serial", [this]() { receiveSerialChars(); });
    }
}

template <typename Config>
void Carloop<Config>::disableGPS()
{
    digitalWrite(Config::GPS_ENABLE_PIN, Config::GPS_ENABLE_INACTIVE);
}

template <typename Config>
void Carloop<Config>::enableBattery()
{
    pinMode(Config::BATTERY_PIN, INPUT);
    readBattery();
}

template <typename Config>
float Carloop<Config>::readBattery()
{
    static constexpr auto MAX_ANALOG_VALUE = 4096;
    static constexpr auto MAX_ANALOG_VOLTAGE = 3.3f;
    auto adcValue = analogRead(Config::BATTERY_PIN);
    return adcValue * MAX_ANALOG_VOLTAGE / MAX_ANALOG_VALUE * Config::BATTERY_FACTOR;
}

template <typename Config>
bool Carloop<Config>::hasCAN()
{
    return Config::FEATURES & features & CARLOOP_CAN;
}

template <typename Config>
bool Carloop<Config>::hasGPS()
{
    return Config::FEATURES & features & CARLOOP_GPS;
}

template <typename Config>
bool Carloop<Config>::hasBattery()
{
    return Config::FEATURES & features & CARLOOP_BATTERY;
}

// Receive bytes from the GPS
template <typename Config>
void Carloop<Config>::receiveSerialChars()
{
    while(true) {
        WITH_LOCK(gpsDriver) {
            while(Serial1.available())
            {
                char c = Serial1.read();
                gpsDriver.encode(c);
            }
        }
        delay(1);
    }
}

// Template instantiation
template class Carloop<CarloopRevision2>;

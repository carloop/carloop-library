/* Library for the Carloop connected car device
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */
#pragma once
#include "application.h"
#include "TinyGPS++.h"

enum CarloopFeatures_e
{
    CARLOOP_CAN = 1,
    CARLOOP_GPS = 2,
    CARLOOP_BATTERY = 4,
    CARLOOP_ALL_FEATURES = CARLOOP_CAN | CARLOOP_GPS | CARLOOP_BATTERY
};

struct CarloopRevision2
{
    static constexpr auto CAN_PINS = CAN_D1_D2;
    static constexpr uint32_t CAN_DEFAULT_SPEED = 500000;

    static constexpr auto BATTERY_PIN = A1;
    static constexpr auto BATTERY_FACTOR = 7.2f;

    static constexpr auto CAN_ENABLE_PIN = D0;
    static constexpr auto CAN_ENABLE_ACTIVE = LOW;
    static constexpr auto CAN_ENABLE_INACTIVE = HIGH;

    static constexpr auto GPS_BAUD_RATE = 9600;
    static constexpr auto GPS_ENABLE_PIN = A0;
    static constexpr auto GPS_ENABLE_ACTIVE = HIGH;
    static constexpr auto GPS_ENABLE_INACTIVE = LOW;

    static constexpr auto FEATURES = CARLOOP_CAN | CARLOOP_GPS | CARLOOP_BATTERY;
};

template <typename Config>
class Carloop
{
public:
    Carloop();

    void setCANSpeed(uint32_t canSpeed);
    void begin(CarloopFeatures_e features = CARLOOP_ALL_FEATURES);

    void update();

    CANChannel &can();
    TinyGPSPlus &gps();
    float battery();

    void enableCAN();
    void disableCAN();
    void enableGPS();
    void disableGPS();
    void enableBattery();

    float readBattery();

    bool hasCAN();
    bool hasGPS();
    bool hasBattery();

private:
    void receiveSerialChars();

    CANChannel canDriver;
    uint32_t canSpeed;

    Thread *gpsSerialThread;
    TinyGPSPlus gpsDriver;

    float batteryVoltage;

    CarloopFeatures_e features;
};

# [Carloop](https://www.carloop.io)

Make apps for your car using signals from OBD-II, CAN and GPS. Publish data online using the Particle platform.

### _Don't have one yet? [Buy a Carloop](https://www.carloop.io/buy)_
### _Have questions? [Ask the Carloop community.](https://community.carloop.io)_

## Usage

```
#include "carloop/carloop.h"
Carloop<CarloopRevision2> carloop;

void setup()
{
    carloop.begin();
}

void loop()
{
    carloop.update();
    CANMessage message;
    if(carloop.can().receive(message)) {
        // Do awesome things with message!
    }
}
```

See complete examples in [the examples directory.](/firmware/examples)

## Getting started

[Follow these instructions on the community](https://community.carloop.io/t/carloop-demo-program/18) to create your first application using the Carloop library.

Learn about programming with the Particle platform at <https://docs.particle.io>

If you want to add a GPS, we recommend [the GP-735](https://www.sparkfun.com/products/13670) and [a small jumper wire](https://www.sparkfun.com/products/574).

## Documentation

### `Carloop` object

Create a global `Carloop` object and specify the hardware revision (`CarloopRevision2`). Find this information on the Carloop circuit board (R2.1 means revision 2).

Call `begin()` in `setup()` and `update()` in `loop()`.

```
Carloop<CarloopRevision2> carloop;

void setup() {
  carloop.begin();
}

void loop() {
  carloop.update();
}
```

### `can()`

Read and write raw CAN messages.

```
void setup() {
    carloop.begin();
    Serial.begin(9600);
}

void loop() {
    carloop.update();
    CANMessage message;
    while(carloop.can().receive(message))
    {
        Serial.printf("ID %03x len %d", message.id, message.length);
        for(int i = 0; i < message.length; i++) {
            Serial.printf("%02x ", message.data[i]);
        }
        Serial.print("\n");
    }
}
```

[See the documentation for the Particle CAN driver for more options.](https://docs.particle.io/reference/firmware/photon/#can-canbus-)

### `gps()`

If you have a GPS connected to the Carloop, you access GPS function through the TinyGPS object with `carlopp.gps()`

```
float lat = carloop.gps().location.lat();
float lng = carloop.gps().location.lng();
bool gpsValid = carloop.gps().location.isValid();
```

[See the TinyGPS library for detailed examples.](https://github.com/codegardenllc/tiny_gps_plus/tree/master/firmware/examples)

### `battery()`

The voltage of the car battery. Make sure you call `carloop.update()` in the `loop()` function to update the battery voltage.

```
float battVoltage = carloop.battery();
```

## TODO

- OBD PID decoder. `carloop["engine_speed"]` to get engine speed.
- OBD fault code reader.

## License

Copyright 2016 Julien Vanier. Distributed under the MIT license. See [LICENSE](/license) for details.


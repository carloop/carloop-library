/* Library for the Carloop connected car device
 *
 * Copyright 2016 Julien Vanier
 *
 * Distributed under the MIT license. See LICENSE.txt for more details.
 */

enum CarloopFeatures_e {
  CARLOOP_CAN,
  CARLOOP_GPS,
  CARLOOP_BATTERY,
  CARLOOP_ALL_FEATURES = CARLOOP_CAN | CARLOOP_GPS | CARLOOP_BATTERY
};

class CarloopClass {
public:
  Carloop();
  void begin(CarloopFeatures_e features = CARLOOP_ALL_FEATURES);

};

extern CarloopClass Carloop;

#include <SPI.h>
#include "DW1000Ranging.h" //https://github.com/thotro/arduino-dw1000
#include "DW1000.h"
#define ANCHOR_ADD "83:17:5B:D5:A9:9A:E2:9C"

HardwareSerial Serial1(PA10, PA9);
// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0;  // irq pin
const uint8_t PIN_SS = SS;    // spi select pin
void newRange();
void inactiveDevice(DW1000Device *device);
void newDevice(DW1000Device *device);
char this_anchor_addr[] = "84:00:22:EA:82:60:3B:9C";
float this_anchor_target_distance = 1; // measured distance to anchor in m

uint16_t Adelay = 16543;

// previously determined calibration results for antenna delay
// #1 16630
// #2 16610
// #3 16607
// #4 16580

// calibration distance
float dist_m = 1; // meters

void newRange();
void newDevice(DW1000Device *device);
void inactiveDevice(DW1000Device *device);

void setup()
{
  Serial1.begin(115200);
  delay(1000); // wait for Serial1 monitor to connect
  Serial1.println("Anchor config and start");
  Serial1.print("Antenna delay ");
  Serial1.println(Adelay);
  Serial1.print("Calibration distance ");
  Serial1.println(dist_m);

  // init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); // Reset, CS, IRQ pin

  // set antenna delay for anchors only. Tag is default (16384)
  DW1000.setAntennaDelay(Adelay);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  // start the module as an anchor, do not assign random short address
  DW1000Ranging.startAsAnchor(this_anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_LOWPOWER);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_LOWPOWER);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_SHORTDATA_FAST_ACCURACY);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_ACCURACY);
  // DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

void loop()
{
  DW1000Ranging.loop();
}

void newRange()
{
  //    Serial1.print("from: ");
  Serial1.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial1.print(", ");

  #define NUMBER_OF_DISTANCES 100
  float dist = 0.0;
  for (int i = 0; i < NUMBER_OF_DISTANCES; i++)
  {
    dist += DW1000Ranging.getDistantDevice()->getRange();
  }
  dist = dist / NUMBER_OF_DISTANCES;
  Serial1.println(dist);
}

void newDevice(DW1000Device *device)
{
  Serial1.print("Device added: ");
  Serial1.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
  Serial1.print("Delete inactive device: ");
  Serial1.println(device->getShortAddress(), HEX);
}

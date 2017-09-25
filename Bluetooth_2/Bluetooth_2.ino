
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

//BNO055 sensor objects
Adafruit_BNO055 bno1 = Adafruit_BNO055(55, 0x29);
Adafruit_BNO055 bno2 = Adafruit_BNO055(56, 0x28);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
unsigned long time;
uint32_t logtime;

void error(const __FlashStringHelper*err)
{
  Serial.println(err);
  while (1);
}

void setup(void)
{
  //while (!Serial);
  delay(500);

  Serial.begin(115200);
  Serial.println("Twin BNO055 Orientation Sensor Test");
  Serial.println("");

  /* Initialise the first sensor */
  if (!bno1.begin())
  {
    /* There was a problem detecting the BNO055 at 0x28 ... check your connections */
    Serial.print("Ooops, no BNO055 detected at 0x28 ... Check your wiring or I2C ADDR!");
    while (1);
  }

  /* Initialise the second sensor */
    if (!bno2.begin())
    {
      /* There was a problem detecting the BNO055 at 0x29 ... check your connections */
      Serial.print("Ooops, no BNO055 detected at 0x29 ... Check your wiring or I2C ADDR!");
      while (1);
    }

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  Serial.println( F("OK!") );
  ble.echo(false);
  ble.info();
  ble.verbose(false);
  while (! ble.isConnected()) {
    delay(500);
  }
  delay(1000);

  bno1.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);
}

void loop(void)
{
  /* Get a new sensor event */
  time = millis();
  logtime = time;
  sensors_event_t bno1Event;
  sensors_event_t bno2Event;

  bno1.getEvent(&bno1Event);
  bno2.getEvent(&bno2Event);

  Serial.print(logtime);
  Serial.print(",");
  Serial.print(bno2Event.orientation.z, 0);
  Serial.print(",");
  Serial.print(bno2Event.orientation.z - bno1Event.orientation.z, 0);
  Serial.print(",");
  Serial.print(bno1Event.orientation.z, 0);
  Serial.println("");

  if (ble.isConnected()) {


    /* Display the floating point data */
    ble.print("AT+BLEUARTTX=");
    ble.print(logtime);
    ble.print(",");
    ble.print(bno2Event.orientation.z, 0);
    ble.print(",");
    ble.print(bno2Event.orientation.z - bno1Event.orientation.z, 0);
    ble.print(",");
    ble.print(bno1Event.orientation.z, 0);
    ble.print("");
    ble.println("\\n");



    //    ble.print("\t");
    //
    //    ble.print("X2: ");
    //    ble.print(bno2Event.orientation.x, 4);
    //    ble.print("\tY2: ");
    //    ble.print(bno2Event.orientation.y, 4);
    //    ble.print("\tZ2: ");
    //    ble.println(bno2Event.orientation.z, 4);

    delay(100);
  }
}

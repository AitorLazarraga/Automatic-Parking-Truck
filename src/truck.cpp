#include <Arduino.h>
#include "truck.h"

extern TwoWire Wire1;

long pulse_in(uint8_t trig, uint8_t echo) {
  /*Function to obtain pulse time with HC-SR04 sensors
  ARGUMENTS trig :: Trigger pin of the sensor // echo :: Echo pin of the sensor
  RETURNS :: time between pulses, divide result by 59 to obtain the distance in CM 
  If used with more than 1 sensor add a delay of at least 60ms to evade the crossound*/
  long timeout = 30000;
  
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  unsigned long start = micros();
  while (digitalRead(echo) == LOW) {
    if (micros() - start > timeout) return -1;
  }
  
  unsigned long pulseStart = micros();
  while (digitalRead(echo) == HIGH) {
    if (micros() - pulseStart > timeout) return -1;
  }
  
  return micros() - pulseStart;
}



// ---------------- I2C ----------------

bool VL53L4CD_Custom::write8(uint16_t reg, uint8_t val) {

    Wire1.beginTransmission(VL53_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);
    Wire1.write(val);

    return Wire1.endTransmission() == 0;
}

bool VL53L4CD_Custom::write16(uint16_t reg, uint16_t val) {

    Wire1.beginTransmission(VL53_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);
    Wire1.write(val >> 8);
    Wire1.write(val & 0xFF);

    return Wire1.endTransmission() == 0;
}

bool VL53L4CD_Custom::read8(uint16_t reg, uint8_t &val) {

    Wire1.beginTransmission(VL53_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);

    if (Wire1.endTransmission(false) != 0) return false;

    Wire1.requestFrom(VL53_ADDR, 1);

    if (!Wire1.available()) return false;

    val = Wire1.read();
    return true;
}

bool VL53L4CD_Custom::read16(uint16_t reg, uint16_t &val) {

    Wire1.beginTransmission(VL53_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);

    if (Wire1.endTransmission(false) != 0) return false;

    Wire1.requestFrom(VL53_ADDR, 2);

    if (Wire1.available() < 2) return false;

    val = (Wire1.read() << 8) | Wire1.read();
    return true;
}

// ---------------- BEGIN ----------------

bool VL53L4CD_Custom::begin() {

    Wire1.begin();
    delay(200);
  
    uint16_t id;
    if (!read16(0x010F, id)) return false;

    return initSensor();
}

// ---------------- INIT SENSOR ----------------

bool VL53L4CD_Custom::initSensor() {

    uint8_t tmp;

    for (int i = 0; i < 1000; i++) {

        read8(0x00E5, tmp);
        if (tmp == 0x03) break;
        delay(1);
    }

    if (tmp != 0x03) return false;

    return true;
}

// ---------------- TIMING ----------------

bool VL53L4CD_Custom::setRangeTiming(uint32_t timing_budget_ms, uint32_t inter_measurement_ms) {

    if (timing_budget_ms < 10 || timing_budget_ms > 400) return false;

    write8(0x006C, 0);
    write8(0x006D, 0);

    return true;
}

// ---------------- START / STOP ----------------

bool VL53L4CD_Custom::startSingle() {

    return write8(0x0087, 0x10);
}

void VL53L4CD_Custom::stopRanging() {

    write8(0x0087, 0x00);
}

// ---------------- OFFSET ----------------

bool VL53L4CD_Custom::calibrateOffset(int16_t target_mm, int16_t &measured_offset_mm, uint16_t samples) {

    int32_t avg = 0;

    for (int i = 0; i < samples; i++) {

        startSingle();

        uint8_t st;
        for (int j = 0; j < 500; j++) {

            read8(0x0089, st);
            if (st & 0x01) break;
            delay(1);
        }

        uint16_t dist;
        read16(0x0096, dist);

        avg += dist;

        write8(0x0086, 0x01);
    }

    avg /= samples;

    measured_offset_mm = target_mm - avg;

    write16(0x0054, measured_offset_mm * 4);

    return true;
}

// ---------------- RESULT ----------------

bool VL53L4CD_Custom::getResult(uint8_t &st, uint16_t lista[7]) {

    uint16_t temp16;
    uint8_t temp8;

    if (!read8(0x0089, temp8)) return false;

    st = (temp8 >> 1) & 0x0F;

    if (!read16(0x0096, temp16)) return false;

    lista[4] = temp16;

    return true;
}

// ---------------- SIMPLE DISTANCE ----------------

uint16_t VL53L4CD_Custom::readDistance() {

    uint8_t st;

    for (int i = 0; i < 500; i++) {

        read8(0x0089, st);
        if (st & 0x01) break;
        delay(1);
    }

    if (!(st & 0x01)) return TOF_NO_DATA;

    uint16_t dist;
    read16(0x0096, dist);

    write8(0x0086, 0x01);

    return dist;
}


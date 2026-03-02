#ifndef TRUCK.H
#define TRUCK.H
#include <Arduino.h>
#include <Wire.h>


#define VL53_ADDR 0x29

#define TOF_NO_DATA   0x0000
#define TOF_NO_TARGET 0xFFFF

// Function to obtain pulse time with HC-SR04 sensors
long pulse_in(uint8_t trig, uint8_t echo);



class VL53L4CD_Custom {
// Class to control a VL53L4CD without using created libraries

public:
    //Function to start the sensor
    bool begin();
    //Function to set the timing 
    bool setRangeTiming(uint32_t timing_budget_ms, uint32_t inter_measurement_ms);
    //Function to calibrate the sensor
    bool calibrateOffset(int16_t target_mm, int16_t &measured_offset_mm, uint16_t samples = 50);


    //Functions to start and stop the sensor
    bool startSingle();
    void stopRanging();

    //Function to get results from the sensor and from the results get the distance
    bool getResult(uint8_t &status, uint16_t result[7]);

    uint16_t readDistance();

private:

    //Private functions to use I2C protocol
    bool write8(uint16_t reg, uint8_t val);
    bool write16(uint16_t reg, uint16_t val);

    bool read8(uint16_t reg, uint8_t &val);
    bool read16(uint16_t reg, uint16_t &val);

    bool initSensor();
};

#endif


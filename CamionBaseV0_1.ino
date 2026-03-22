#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include <Wire.h>
#include "truck.h"
#include <Modulino.h>



// Defines
// Pin 0 y Pin 1 Usandose para UART
// Reset IMU
#define RST 5 
// PINS
const uint8_t TRIGGER_PIN = 2;
const uint8_t ECHO_PINS[] = {6, 7, 8, 9, 10, 11, 12, 13};
const size_t NUM_SENSORS = sizeof(ECHO_PINS) / sizeof(ECHO_PINS[0]);
// Classes
//VL53L4CD_Custom tof;
ModulinoDistance distance; // Se vuelve a usar la libreria modulino tras actualización que arregla el fallo de compatibildiad

uint8_t status;
uint16_t data[7];
String msg = "";


void controlMotores(String cmd){
  // Envio de datos por serial al Arduino Mega
  Serial.println(cmd);
}

void setup() {
  // Start sensors
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  for (size_t i = 0; i < NUM_SENSORS; i++) {
    pinMode(ECHO_PINS[i], INPUT);
  }

  
  Bridge.begin();
  msg = "Arduino inicializado, esperando modulino";
  Bridge.notify("status", msg);
  delay(1000);
  Serial.println("UNO Listo");
  Bridge.provide("controlMotores", controlMotores);
  Modulino.begin();
  distance.begin();
}


void loop() {
  //Arduino lee continuamente para obtener los datos del sensor IMU desde el Arduino Mega
  if(Serial.available()){
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    Bridge.notify("parse", cmd);
  }

  //Se lee en cada iteración cada sensor ultrasonico
  for (size_t i = 0; i < NUM_SENSORS; i++) {
    long pulse = pulse_in(TRIGGER_PIN, ECHO_PINS[i]);
    if (pulse > 0) {
      long dist_cm = pulse / 59;
      String envio = String(i) + ":" + String(dist_cm);
      Bridge.notify("distance", envio);
    }
    delay(60);
  }
  //Se lee en cada iteración el sensor ToF trasero
  if (distance.available()) {
    int measure = distance.get();
    measure = measure / 10;
    String envio = "M:" + String(measure);
    Bridge.notify("distance", envio);
  }
  delay(500);
}

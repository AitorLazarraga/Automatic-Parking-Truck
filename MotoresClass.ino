#define M1_FRW 34
#define M1_BCK 35
#define M1_PWM 10
#define ENABLE1 8

#define M2_FRW 36
#define M2_BCK 37
#define M2_PWM 11
#define ENABLE2 12

#define ENC2_A 19  // Pin con interrupción INT4
#define ENC2_B 38  // Pin digital normal

#define giro_completo 375
float motivo_grados = 375.0 / 360.0;

volatile long countM2 = 0;
float targetAngle = 0;
// ISR para encoder de M2
void ISR_M2() {
  if (digitalRead(ENC2_B) == HIGH) countM2++;
  else countM2--;
}

// Clase Motor
class Motor {
  public:
    int pinFRW, pinBCK, pinPWM, pinEnable;
    bool hasEncoder;
    int encA, encB;
    volatile long* encoderCount;

    Motor(int frw, int bck, int pwm, int enable) {
      pinFRW = frw;
      pinBCK = bck;
      pinPWM = pwm;
      pinEnable = enable;
      hasEncoder = false;
    }

    Motor(int frw, int bck, int pwm, int enable, int eA, int eB, volatile long* count) {
      pinFRW = frw;
      pinBCK = bck;
      pinPWM = pwm;
      pinEnable = enable;
      encA = eA;
      encB = eB;
      encoderCount = count;
      hasEncoder = true;
    }

    void begin() {
      pinMode(pinFRW, OUTPUT);
      pinMode(pinBCK, OUTPUT);
      pinMode(pinPWM, OUTPUT);
      pinMode(pinEnable, OUTPUT);
      digitalWrite(pinEnable, HIGH);

      if (hasEncoder) {
        pinMode(encA, INPUT_PULLUP);
        pinMode(encB, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(encA), ISR_M2, RISING);
      }
    }

    void forward(int speed) {
      analogWrite(pinPWM, speed);
      digitalWrite(pinFRW, HIGH);
      digitalWrite(pinBCK, LOW);
    }

    void backward(int speed) {
      analogWrite(pinPWM, speed);
      digitalWrite(pinFRW, LOW);
      digitalWrite(pinBCK, HIGH);
    }

    void stop() {
      digitalWrite(pinFRW, LOW);
      digitalWrite(pinBCK, LOW);
      analogWrite(pinPWM, 0);
    }

    long getEncoder() {
      if (hasEncoder) return *encoderCount;
      return -1; // No tiene encoder
    }

    void movePosition(float degrees) {

      long goal = degrees * motivo_grados;
      long error = goal - *encoderCount;
  
      int speed = constrain(abs(error) * 2, 60, 150);

      if (abs(error) < 5) {
        stop();
        return;
      }

      analogWrite(pinPWM, speed);

      if (error < 0) {
        digitalWrite(pinFRW, HIGH);
        digitalWrite(pinBCK, LOW);
      } else {
        digitalWrite(pinFRW, LOW);
        digitalWrite(pinBCK, HIGH);
      }
    }
};

// Crear instancias de motores
Motor M1(M1_FRW, M1_BCK, M1_PWM, ENABLE1);
Motor M2(M2_FRW, M2_BCK, M2_PWM, ENABLE2, ENC2_A, ENC2_B, &countM2);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  M1.begin();
  M2.begin();
}

void loop() {
 // Serial.print("Encoder M2: ");
 // Serial.println(M2.getEncoder());
  M2.movePosition(targetAngle);
  if (Serial2.available()) {

    String cmd = Serial2.readStringUntil('\n');
    cmd.trim();

    Serial.print("Recibido: ");
    Serial.println(cmd);

    if (cmd == "F") {
      M1.forward(255);
      Serial2.println("OK_F");
    }

    else if (cmd == "B") {
      M1.backward(180);
      Serial2.println("OK_B");
    }

    else if (cmd == "S") {
      M1.stop();
      Serial2.println("OK_S");
    }

    else if (cmd.startsWith("G")) {
      targetAngle = cmd.substring(1).toFloat();
      Serial2.println("OK_G");
    }
  }
}
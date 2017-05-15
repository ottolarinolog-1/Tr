#include <Servo.h>
#include <SimpleModbusSlave.h>

// Налаштовувані константи.
////////////////////////////////////////////////////////////////////////////////
#define hysteresis  70    // Для сонця ~70.

#define BaudRate  250000  // Швидкість передачі Serial.

#define CalibrationDelay  60000 // Затримка для виставлення дзеркала в 0.
#define PositioningDelay  1000  // Затримка в позиціонуванні після зупинки.
#define ServoPosDelay 15  // Зтримка для позиціонування сервомотора.
////////////////////////////////////////////////////////////////////////////////

#define SensCPin  A4	// Common.
#define SensHPin  A5	// Horizontal.
#define SensVPin  A6	// Vertical.

#define MoveUPin  9		// Up.
#define MoveDPin  10	// Down.
#define ServoPin  11	// Left/Right.

#define EncoderPin2 4 // 2-га нога енкодера (1-ша на другому піні).

#define moveUp  	{digitalWrite(MoveUPin, LOW); digitalWrite(MoveDPin, HIGH); Serial.println("UP");}
#define moveDown  {digitalWrite(MoveDPin, LOW); digitalWrite(MoveUPin, HIGH); Serial.println("DOWN");}
#define moveStop  {digitalWrite(MoveDPin, LOW); digitalWrite(MoveUPin, LOW); Serial.println("STOP");}

int valC, valH, valV, errorV, errorH, posMirror;
byte posServo = 90;
double timeCount;

Servo servoMotor;

//  Modbus.
enum {MIRROR_POSITION, HOLDING_REGS_SIZE};  // Опис регістрів.
unsigned int holdingRegs[HOLDING_REGS_SIZE];  // Об'явлення масиву регістрів.

void setup() {
  //Serial.begin(BaudRate);
  
  servoMotor.attach(ServoPin);
  servoMotor.write(posServo); // Виставити на "середину".
  delay(ServoPosDelay);       // Почекати поки стане.

  pinMode(SensCPin, INPUT);
  pinMode(SensHPin, INPUT);
  pinMode(SensVPin, INPUT);
  
  pinMode(EncoderPin2, INPUT);

  pinMode(MoveUPin, OUTPUT);
  pinMode(MoveDPin, OUTPUT);

  digitalWrite(MoveDPin, LOW);
  digitalWrite(MoveUPin, LOW);

  // Виведення дзеркала в початкову позицію.
  moveDown;
  delay(CalibrationDelay);
  moveStop;
  posMirror = 0;

  timeCount = millis();
  
  attachInterrupt(0, int_0, RISING);  // Об'явлення обробника переривань.
  
  //  Modbus.
  modbus_configure(&Serial, BaudRate, SERIAL_8N1, 1, 2, HOLDING_REGS_SIZE, holdingRegs);// Конфігурація.
  modbus_update_comms(BaudRate, SERIAL_8N1, 1); // Параметри обміну.
}

void loop() {
  // Опитування і визначення похибок.
  valC = analogRead(SensCPin);
  valH = analogRead(SensHPin);
  valV = analogRead(SensVPin);
  errorH = abs(valC - valH);
  errorV = abs(valC - valV);

  // Актуатор.
  if ((errorV <= hysteresis - hysteresis/2)) {
    moveStop;
    //delay(PositioningDelay);  // ?????
  }
  else if (valC > valV) { 
    if (errorV > hysteresis) moveUp;
  } // 1 раз ввікнулось і не вимкнеться поки не спрацює умова на СТОП.
  else if (valV > valC) {
    if (errorV > hysteresis) moveDown;
  }

  // Сервопривод.
  if (timeCount > millis()) timeCount = millis();	// Спрацює при переповненні millis().

  if (millis() - timeCount > ServoPosDelay) {     // Все робити коли пройде час
    timeCount = millis();                         // для зміни положення.

    if (errorH > hysteresis) {
      if (valC > valH) {
        posServo--;
        if (posServo > 180) posServo = 180;
        servoMotor.write(posServo);
      }
      
      if (valH > valC) {
        posServo++;
        if (posServo == 255) posServo = 0;
        servoMotor.write(posServo);
      }
    }
  }
}

void int_0() {
  if (digitalRead(EncoderPin2) == LOW) {
    posMirror++;
  }
  else {
    posMirror--;
  }
  
  // Modbus.
  holdingRegs[MIRROR_POSITION] = posMirror; // Запис данних в регистр.
  modbus_update();// Оновлення данних в регістрах.
}

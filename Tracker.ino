#include <Servo.h>

#define SensCPin  A4	// Common.
#define SensHPin  A5	// Horizontal.
#define SensVPin  A6	// Vertical.

#define MoveUPin  9		// Up.
#define MoveDPin  10	// Down.
#define ServoPin  11	// Left/Right.

#define hysteresis  70  // Для сонця ~70.
//
//#define corrL  1
//#define corrR  1

#define moveUp  	{digitalWrite(MoveUPin, HIGH); digitalWrite(MoveDPin, LOW); Serial.println("UP");}
#define moveDown  {digitalWrite(MoveDPin, HIGH); digitalWrite(MoveUPin, LOW); Serial.println("DOWN");}
#define moveStop  {digitalWrite(MoveDPin, HIGH); digitalWrite(MoveUPin, HIGH); Serial.println("STOP");}

int valC, valH, valV, errorV, errorH;
byte position = 90;
double timeCount;

Servo servoMotor;

void setup() {
  Serial.begin(250000);

  servoMotor.attach(ServoPin);
  servoMotor.write(position);     // Виставити на "середину".
  delay(15);                // Почекати поки стане.

  pinMode(SensCPin, INPUT);
  pinMode(SensHPin, INPUT);
  pinMode(SensVPin, INPUT);

  pinMode(MoveUPin, OUTPUT);
  pinMode(MoveDPin, OUTPUT);

  digitalWrite(MoveDPin, HIGH);
  digitalWrite(MoveUPin, HIGH);

  timeCount = millis();
}

void loop() {
  info(0);
  
  // Опитування і визначення похибок.
  valC = analogRead(SensCPin);
  valH = analogRead(SensHPin);
  valV = analogRead(SensVPin);
  errorH = abs(valC - valH);
  errorV = abs(valC - valV);

  // Актуатор.
  if ((errorV <= hysteresis-35)) {
    //info(1);
    moveStop;
    delay(1000);
  }
  else if (valC > valV) {
    //info(1);
    if (errorV > hysteresis) moveUp;
  }
  else if (valV > valC) {
    //info(1);
    if (errorV > hysteresis) moveDown;
  }

  // Сервопривод.
  if (timeCount > millis()) timeCount = millis();	// Спрацює при переповненні millis().

  if (millis() - timeCount > 15) {
    timeCount = millis();

    if (errorH > hysteresis) {
      if (valC > valH) {
        position--;
        if (position > 180) position = 180;
        servoMotor.write(position);
         Serial.println("--");
        //info(2);
      }
      
      if (valH > valC) {
        position++;
        if (position == 255) position = 0;
        servoMotor.write(position);
         Serial.println("++");
        //info(2);
      }
     // info(2);
    }
  }
  /*
    // Надаштування гістерезиса та корекцій.
    if (Serial.available())
    {
      switch (Serial.read())
      {
        case 113:   // q
          hysteresis++;
          break;
        case 97:    // a
          hysteresis--;
          break;

        case 119:   // w
          corrL++;
          break;
        case 115:   // s
          corrL--;
          break;

        case 101:    // e
          corrR++;
          break;
        case 100:    // d
          corrR--;
          break;

        case 114:   // r
          moveLeft;
          break;
        case 102:   // f
          moveRight;
          break;

        case 104:    // h
          Serial.println("For +/- hysteresis press q/a");
          Serial.println("For +/- corrL press w/s");
          Serial.println("For +/- corrR press e/d");
          Serial.println("For move actuatotor left/right press r/f");
          break;

        default:
          Serial.println("No such function");
      }

      Serial.println();
      Serial.println();
      Serial.print("hysteresis = ");
      Serial.println(hysteresis);
      Serial.print("corrL = ");
      Serial.println(corrL);
      Serial.print("corrR = ");
      Serial.println(corrR);
      Serial.println();
      Serial.println();

    delay(1500);
    }*/
}

void info(byte direction) {
  switch (direction) {
    case 1:
      Serial.println("ACTUATOR");
      break;
    case 2:
      Serial.println("SERVO");
      break;
  }

  Serial.print("Common Sensor: ");
  Serial.print(valC);

  Serial.print("        Horizontal Sensor: ");
  Serial.print(valH);

  Serial.print("				Vertical Sensor: ");
  Serial.println(valV);


  Serial.print("errorH: ");
  Serial.print(errorH);
  Serial.print("        errorV: ");
  Serial.println(errorV);
  Serial.print("        POSITION: ");
  Serial.println(position);
  Serial.println();

}


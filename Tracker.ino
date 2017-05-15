//#include <math.h>

#define SensLPin  A6
#define SensRPin  A7

#define MoveLPin 9
#define MoveRPin 10

//#define hysteresis  5
//
//#define corrL  1
//#define corrR  1

#define moveLeft  {digitalWrite(MoveRPin, HIGH); digitalWrite(MoveLPin, LOW);}
#define moveRight  {digitalWrite(MoveLPin, HIGH); digitalWrite(MoveRPin, LOW);}
#define moveStop  {digitalWrite(MoveRPin, HIGH); digitalWrite(MoveLPin, HIGH);}

float valL, valR, error, hysteresis = 3, corrL, corrR;
int valSumm;

void setup()
{
  Serial.begin(9600);

  pinMode(SensLPin, INPUT);
  pinMode(SensRPin, INPUT);

  pinMode(MoveLPin, OUTPUT);
  pinMode(MoveRPin, OUTPUT);

  digitalWrite(MoveLPin, HIGH);
  digitalWrite(MoveRPin, HIGH);
}

void loop()
{
  valL = analogRead(SensLPin);//(valL + analogRead(SensLPin)) / 2;
  valR = analogRead(SensRPin);//(valR + analogRead(SensRPin)) / 2;
  error = abs(valL - valR);

  valSumm = (valL + valR) / 2;

  if ((error <= hysteresis)) //(valSumm <= 450) || 
  {
    info();
    moveStop;
    delay(700);
  }
  else if (valL > valR)
  {
    //info();
    if (error > hysteresis) moveLeft;
  }
  else if (valR > valL)
  {
  //  info();
    if (error > hysteresis) moveRight;
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

void info(void)
{
  
  Serial.println("For help press h");
  Serial.print("Left Sensor: ");
  Serial.print(valL);
  Serial.print("         valSumm: ");
  Serial.println(valSumm);
  Serial.print("Right Sensor: ");
  Serial.print(valR);
  Serial.print("         error: ");
  Serial.println(error);
  Serial.println();
}


#include <Arduino.h>

// consts
#define timermax 7000
#define safetimemax 20000
enum states
{
  OPEN,
  CLOSED,
  OPENING,
  CLOSING
};

// vars
unsigned long lasttime;
unsigned long safetime;
boolean timerstate = false;
boolean safetimeractive = false;
int doorstate;
boolean motor1_on = false;
boolean motor2_on = false;
boolean sensorsTriggered = false;
String strr[4] = {"Open", "Closed", "Opening", "Closing"};

// pins
int limitsw_open = 9;
int limitsw_close = 8;
int sens_1 = 7;
int sens_2 = 6;
int motor_1 = 10;
int motor_2 = 11;

// funcs
void triggerTimer()
{
  timerstate = true;
  lasttime = millis();
}

void closeDoor()
{
  digitalWrite(motor_2, LOW);
  for (int i = 0; i < 256; i++)
  {
    analogWrite(motor_1, i);
    delay(2);
  }
  digitalWrite(motor_1, HIGH);
  motor1_on = true;
  motor2_on = false;
}

void openDoor()
{
  digitalWrite(motor_1, LOW);
  for (int i = 0; i < 256; i++)
  {
    analogWrite(motor_2, i);
    delay(2);
  }
  digitalWrite(motor_2, HIGH);
  motor2_on = true;
  motor1_on = false;
}

void stopMotor()
{
  if (motor1_on)
  {
    delay(0);
    for (int i = 190; i >= 0; i--)
    {
      analogWrite(motor_1, i);
      delay(1);
    }
    digitalWrite(motor_1, LOW);
  }
  if (motor2_on)
  {
    for (int i = 255; i >= 0; i--)
    {
      analogWrite(motor_2, i);
      delay(2);
    }
    digitalWrite(motor_2, LOW);
  }
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);
  motor1_on = false;
  motor2_on = false;
}

void checkSensors()
{
  static int lastsen1 = false;
  static int lastsen2 = false;

  if (digitalRead(sens_1) && !lastsen1)
  {
    triggerTimer();
  } 
  if (digitalRead(sens_2) && !lastsen2)
  {
    triggerTimer();
  }
  lastsen1 = digitalRead(sens_1);
  lastsen2 = digitalRead(sens_2);
}

// timer set to expire after timermax milliseconds
// timer activates every time a sensor is triggered
void timerUpdate()
{
  if ((millis() - lasttime) > timermax)
  {
    timerstate = false;
  }
}


boolean timerActive()
{
  return timerstate;
}

void setup()
{
  // init pins
  Serial.begin(9600);
  pinMode(limitsw_close, INPUT_PULLUP);
  pinMode(limitsw_open, INPUT_PULLUP);
  pinMode(sens_1, INPUT);
  pinMode(sens_2, INPUT);
  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);

  // determine current state
  if (!digitalRead(limitsw_close))
  {
    doorstate = CLOSED;
  }
  else if (!digitalRead(limitsw_open))
  {
    doorstate = OPEN;
  }
  else
  {
    doorstate = OPENING;
    openDoor();
  }
}

void loop()
{
  checkSensors();
  timerUpdate();
  for (int i = 0; i < 4; i++)
  {
    if (doorstate == i)
    {
      Serial.println(strr[i]);
    }
  }
  
  if (doorstate == OPENING)
  {
    if (!digitalRead(limitsw_open))
    {
      stopMotor();
      doorstate = OPEN;
      triggerTimer();
    }
  }

  if (doorstate == OPEN)
  {
    if (!timerActive())
    {
      closeDoor();
      doorstate = CLOSING;
    }
  }

  if (doorstate == CLOSING)
  {
    if (!digitalRead(limitsw_close))
    {
      stopMotor();
      doorstate = CLOSED;
    }
    if (timerActive())
    {
      stopMotor();
      openDoor();
      doorstate = OPENING;
    }
  }

  if (doorstate == CLOSED)
  {
    if (timerActive())
    {
      openDoor();
      doorstate = OPENING;
    }
  }

  /*if (!safetimeractive)
  {
    digitalWrite(motor_1, LOW);
    digitalWrite(motor_2, LOW);
  }*/
}

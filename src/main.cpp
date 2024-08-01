#include <Arduino.h>

// consts
#define timermax 5000
enum states
{
  OPEN,
  CLOSED,
  OPENING,
  CLOSING
};

// vars
unsigned long lasttime;
boolean timerstate = false;
int doorstate;
boolean motor1_on = false;
boolean motor2_on = false;

// pins
int limitsw_open = 9;
int limitsw_close = 8;
int sens_1 = 7;
int sens_2 = 6;
int motor_1 = 10;
int motor_2 = 11;

// funcs
void closeDoor()
{
  digitalWrite(motor_2, LOW);
  for (int i = 0; i < 256; i++)
  {
    analogWrite(motor_1, i);
    delay(1);
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
    delay(1);
  }
  digitalWrite(motor_2, HIGH);
  motor2_on = true;
  motor1_on = false;
}

void stopMotor()
{
  if (motor1_on)
  {
    for (int i = 255; i >= 0; i--)
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
      delay(1);
    }
    digitalWrite(motor_2, LOW);
  }
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, LOW);
  motor1_on = false;
  motor2_on = false;
}

// timer set to expire after timermax milliseconds
// timer activates every time a sensor is triggered
void timerUpdate()
{
  if (digitalRead(sens_1) || digitalRead(sens_2))
  {
    timerstate = true;
    lasttime = millis();
  }
  if ((millis() - lasttime) > timermax)
  {
    timerstate = false;
  }
}
void triggerTimer()
{
  timerstate = true;
  lasttime = millis();
}
boolean timerActive()
{
  return timerstate;
}

void setup()
{
  // init pins
  pinMode(limitsw_close, INPUT);
  pinMode(limitsw_open, INPUT);
  pinMode(sens_1, INPUT);
  pinMode(sens_2, INPUT);
  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);

  // determine current state
  if (digitalRead(limitsw_close))
  {
    doorstate = CLOSED;
  }
  else if (digitalRead(limitsw_open))
  {
    doorstate = OPEN;
  }
  else
  {
    doorstate = OPENING;
    // openDoor();
    // triggerTimer();
  }
}

void loop()
{
  openDoor();
  delay(3000);
  stopMotor();
  delay(3000);
  closeDoor();
  delay(3000);
  stopMotor();
  delay(3000);
  /*timerUpdate();

  if (doorstate == OPENING)
  {
    if (digitalRead(limitsw_open))
    {
      stopMotor();
      doorstate = OPEN;
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
    if (digitalRead(limitsw_close))
    {
      stopMotor();
      doorstate = CLOSED;
    }
    if (timerActive())
    {
      stopMotor();
      delay(100);
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
  }*/
}

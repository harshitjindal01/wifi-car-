#include <Servo.h>
#include <PS2X_lib.h>  //for v1.6
PS2X ps2x;
Servo myservo;

#define PS2_DAT        A0  //14    
#define PS2_CMD        A1  //15
#define PS2_SEL        A2  //16
#define PS2_CLK        A3  //17
#define PWMA1          5   //Motor A's pin
#define PWMA2          6   //Motor A's pin
#define SERVOPIN       4   //servo's pin

#define SERVOMIN 30      //the angle of servo
#define SERVOMID 90
#define SERVOMAX 150
#define TURNLEFT myservo.write(SERVOMIN)  //turn left
#define TURNMID myservo.write(SERVOMID)   //turn middle
#define TURNRIGHT myservo.write(SERVOMAX)  //turn right
#define MOTORFOWARD setMotor(255)   //motor foward
#define MOTORBACK setMotor(-255)     //motor back
#define MOTORSTOP setMotor(0)        //motor stop

bool EnableRockerR = 1;
bool EnableRockerL = 1;
int RMid = 0 ;
int LStop = 0 ;

#define pressures   false
#define rumble      false
int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {
  Serial.begin(57600);
  pinMode(PWMA1, OUTPUT);
  pinMode(PWMA2, OUTPUT);
  myservo.attach(SERVOPIN);
  SERVOMID;
  delay(300);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }
  else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
}
void(* resetFunc) (void) = 0;
void loop() {
  if (error == 1) //skip loop if no controller found
    resetFunc();
  ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START))        //will be TRUE as long as button is pressed
    Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT))
    Serial.println("Select is being held");


  if (ps2x.ButtonPressed(PSB_PAD_UP)) {
    Serial.println("PAD_UP just pressed");
    MOTORFOWARD;
    EnableRockerL = 0;
  }else if(ps2x.ButtonReleased(PSB_PAD_UP)) {
    Serial.println(" PAD_UP just released");
    MOTORSTOP;
    EnableRockerL = 1;
    LStop = 0;
  }

  if (ps2x.ButtonPressed(PSB_PAD_DOWN)) {
    Serial.println("PAD_DOWN just pressed");
    MOTORBACK;
    EnableRockerL = 0;
  }else if(ps2x.ButtonReleased(PSB_PAD_DOWN)) {
    Serial.println(" PAD_DOWN just released");
    MOTORSTOP;
    EnableRockerL = 1;
    LStop = 0;
  }

  if (EnableRockerL) {
    if (0 <= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) < 100) {
      Serial.println(" f");
      MOTORFOWARD;
      LStop = 0;
    } else if (255 >= ps2x.Analog(PSS_LY) && ps2x.Analog(PSS_LY) > 170) {
      MOTORBACK;
      Serial.println(" b");
      LStop = 0;
    } else {
      if(LStop == 0){
        MOTORSTOP;
        Serial.println(" s");
        LStop ++;
      }
    }
  }


  if (ps2x.ButtonPressed(PSB_CIRCLE)) {             //will be TRUE if button was JUST pressed
    Serial.println("Circle just pressed");
    TURNRIGHT;
    EnableRockerR = 0;
  } else if (ps2x.ButtonReleased(PSB_CIRCLE)) {
    Serial.println("Circle just released");
    TURNMID;
    EnableRockerR = 1;
    RMid = 0;
  }
  
  if (ps2x.ButtonPressed(PSB_SQUARE)) {            //will be TRUE if button was JUST released
    Serial.println("Square just pressed");
    TURNLEFT;
    EnableRockerR = 0;
  } else if (ps2x.ButtonReleased(PSB_SQUARE)) {            //will be TRUE if button was JUST released
    Serial.println("Square just released");
    TURNMID;
    EnableRockerR = 1;
    RMid = 0;
  }

  if (EnableRockerR) {
    if (0 <= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) < 100) {
      TURNLEFT;
      Serial.println(" l");
      RMid = 0;
    } else if (255 >= ps2x.Analog(PSS_RX) && ps2x.Analog(PSS_RX) > 160) {
      TURNRIGHT;
      Serial.println(" r");
      RMid = 0;
    } else {
      if(RMid == 0){
        TURNMID;
        Serial.println(" mid");
        RMid ++;
      }
    }
  }


  if (ps2x.NewButtonState(PSB_CROSS)) {             //will be TRUE if button was JUST pressed OR released
    Serial.println("X just changed");
    MOTORSTOP;
  }
  delay(50);
}



void setMotor(int m1Speed) //电机驱动函数
{
  if (m1Speed >= 0)
  {
    digitalWrite(PWMA2, HIGH);
    analogWrite(PWMA1, 255 - m1Speed);
  }
  else
  {
    digitalWrite(PWMA1, HIGH);
    analogWrite(PWMA2, m1Speed + 255);
  }
}

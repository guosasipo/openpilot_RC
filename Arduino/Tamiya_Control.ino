#include <Servo.h>

Servo driveMotor;
Servo turnMotor;

// duration for output
int timeDelay = 50;
int reverseDelay=100;
int forwardDelay=500;

// initial command
int command = 0x30;

//Specify external pins
int drivePin = 9;
int turnPin=8;

//Initialise speed and turn
int driveSpeed=90;
int turnAngle=90;

//Trim needed to center the steering 
int trimAngle=-10; 

//set max settings
int maxFSpeed=80;
int maxRSpeed=110;
int maxLeft=60;
int maxRight=120;
int index=0;
int control_message[8];
int throttle=0;
int angle=0;
int vehicle_speed=0;

void setup() {
  //Setup serial port
  Serial.begin(115200);

  //Attach drive and turn motors
  driveMotor.attach(drivePin);
  turnMotor.attach(turnPin);

  //Neutral the drive motor
  driveMotor.write(90);

  //Center the steering
  turnMotor.write(90+trimAngle);
}

void loop() {
  //receive command
  if (Serial.available() > 0){
    command = Serial.read();
    command = command - 48;
    control_message[index] = command;
    index++;
  }
  
  //If 8 characters have been received
  if (index >= 8)
  {
    //Decompose array to calculate the steering angle specified
   angle =  100 * control_message[1] + 10 * control_message[2] + control_message[3];
   //Serial.print(angle);
    //Decompose array to calculate the throttle value specified
   throttle = 100 * control_message[5] + 10 * control_message[6] + control_message[7];
   //Serial.print(throttle);
   
    //Scale angle to 50 degrees of turn
    turnAngle = angle*50/255;
    if (control_message[0] == 1)
    {
      //Change sign if it's a left turn
      turnAngle = turnAngle * -1;
    }
    //Send command to steering servo
    turnMotor.write(90 + turnAngle + trimAngle);
    
    //Scale throttle to 30 units from neutral (90)
    vehicle_speed = throttle*30/255;
    if (control_message[4] == 0)
    {
      //Driving forwards
      driveMotor.write(90 - vehicle_speed);
    }
    else
    {
      //Driving reverse
      driveMotor.write(90 + vehicle_speed);
    }
    
    
   /* For Debugging:
   for (int i=0; i<8; i++)
   {
     Serial.print( control_message[i]);
   }
   Serial.println();
   Serial.print ("Throttle: ");
   Serial.print (90 - vehicle_speed);
   Serial.print("        ");
   Serial.print ("Angle: ");
   Serial.print (90 + turnAngle);   
   Serial.println();*/
   
   index=0;
  }

}
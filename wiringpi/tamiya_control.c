#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <softPwm.h>
#include <errno.h>

int main (void)
{
        // duration for output
        int timeDelay = 50;
        int reverseDelay=100;
        int forwardDelay=500;

        // initial command
        int command = 0x30;

        //Specify external pins
        int drivePin = 22;
        int turnPin=23;

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

        if(wiringPiSetup() == -1)
        return 1;

        int fd;

        if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
        {
                return 0 ;
        }

        pinMode (drivePin, OUTPUT);
        pinMode (turnPin, OUTPUT);
        softPwmCreate(drivePin, 0, 200) ;
        softPwmCreate(turnPin, 0, 200) ;

        for (;;)
        {
                if ((putchar(serialGetchar(fd))) > 0){
                        command = fd;
                        command = command - 48;
                        control_message[index] = command;
                        index++;
                }

                //If 8 characters have been received
                if (index >= 8)
                {
                        //Decompose array to calculate the steering angle specified
                        angle =  100 * control_message[1] + 10 * control_message[2] + control_message[3];
                        //Decompose array to calculate the throttle value specified
                        throttle = 100 * control_message[5] + 10 * control_message[6] + control_message[7];

                        //Scale angle to 50 degrees of turn
                        turnAngle = angle*50/255;
                        if (control_message[0] == 1)
                        {
                        //Change sign if it's a left turn
                        turnAngle = turnAngle * -1;
                        }
                        //Send command to steering servo
                        softPwmWrite(turnPin,90 + turnAngle + trimAngle);

                        //Scale throttle to 30 units from neutral (90)
                        vehicle_speed = throttle*30/255;
                        if (control_message[4] == 0)
                        {
                        //Driving forwards
                        softPwmWrite(drivePin,90 - vehicle_speed);
                        }
                        else
                        {
                        //Driving reverse
                        softPwmWrite(drivePin,90 + vehicle_speed);
                        }
                }

                index=0;
        }
}
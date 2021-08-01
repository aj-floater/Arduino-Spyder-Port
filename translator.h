#ifndef CUSTOMSERVO_H
#define CUSTOMSERVO_H

#include "C:\Users\arjam\Gits\Arduino-Spyder-Port\position.h"
#include "C:\Users\arjam\Gits\Arduino-Spyder-Port\arm.h"
#include "C:\Users\arjam\Gits\Arduino-Spyder-Port\body.h"

#include <ServoEasing.h>

class CustomServo {
public:
    float offset_angle, pin;
    ServoEasing servo;

    CustomServo(){}
    CustomServo(float offset_angle_in, float pin_in) : offset_angle{ offset_angle_in }, pin{ pin_in }
    {}
    
    void write(float angle){
        this->servo.write(offset_angle + angle);
    }

    void easeTo(float angle){
        this->servo.easeTo(offset_angle + angle);
    }
};

class SimulationToArduinoTranslator {
private:
    int Degrees(float radians){
        int degrees = radians * 180/M_PI;
        return degrees;
    }
    void SetOffsetAngles() {
        servos[0].offset_angle = 130;
        servos[1].offset_angle = 140;
        servos[2].offset_angle = 175;
        servos[3].offset_angle = 55;
        servos[4].offset_angle = 80;
        servos[5].offset_angle = 170;
        servos[6].offset_angle = 80;
        servos[7].offset_angle = 100;
        servos[8].offset_angle = 175;
        servos[9].offset_angle = 70;
        servos[10].offset_angle = 80;
        servos[11].offset_angle = 40;
        servos[12].offset_angle = 60;
        servos[13].offset_angle = 70;
        servos[14].offset_angle = 70;
        servos[15].offset_angle = 90;
        servos[16].offset_angle = 100;
        servos[17].offset_angle = 80;
    }
    void EaseUpdateLegs(){
        // Set Leg group 1
        servos[6].easeTo(- 180 + Degrees(spyder.arms[0].motor1_angle));
        servos[7].easeTo(- Degrees(spyder.arms[0].motor2_angle));
        servos[8].easeTo(- Degrees(spyder.arms[0].motor2_angle) + Degrees(spyder.arms[0].motor3_angle));

        servos[0].easeTo(180 + Degrees(spyder.arms[1].motor1_angle));
        servos[1].easeTo(- Degrees(spyder.arms[1].motor2_angle));
        servos[2].easeTo(- Degrees(spyder.arms[1].motor2_angle) + Degrees(spyder.arms[1].motor3_angle));

        servos[12].easeTo(Degrees(spyder.arms[2].motor1_angle));
        servos[13].easeTo(Degrees(spyder.arms[2].motor2_angle));
        servos[14].easeTo(Degrees(spyder.arms[2].motor2_angle) - Degrees(spyder.arms[2].motor3_angle));

        // Set Leg group 2
        if (Degrees(spyder.arms[3].motor1_angle) < 0)
            servos[3].easeTo(180 + Degrees(spyder.arms[3].motor1_angle));
        else 
            servos[3].easeTo(- 180 + Degrees(spyder.arms[3].motor1_angle));
        servos[4].easeTo(- Degrees(spyder.arms[3].motor2_angle));
        servos[5].easeTo(- Degrees(spyder.arms[3].motor2_angle) + Degrees(spyder.arms[3].motor3_angle));
        
        servos[9].easeTo(Degrees(spyder.arms[4].motor1_angle));
        servos[10].easeTo(Degrees(spyder.arms[4].motor2_angle));
        servos[11].easeTo(Degrees(spyder.arms[4].motor2_angle) - Degrees(spyder.arms[4].motor3_angle));

        servos[15].easeTo(Degrees(spyder.arms[5].motor1_angle));
        servos[16].easeTo(- Degrees(spyder.arms[5].motor2_angle));
        servos[17].easeTo(- Degrees(spyder.arms[5].motor2_angle) + Degrees(spyder.arms[5].motor3_angle));
    }
    void ServoStartupSequence() {
        // Attaches each servo while making sure that the movement of the servo doesn't sabotage the robot
        int pointer = 0;
        int servo_pin = 22;
        while (servo_pin < 40) {
            int i = servo_pin - 22;
            servos[i].servo.attach(servo_pin);
            servos[i].write(0);
            servos[i].pin = servo_pin;

            if (pointer == 0) servo_pin += 2;
            if (pointer == 1) servo_pin -= 1;
            if (pointer == 2) {
                servo_pin += 2;
                pointer = -1;
            }
            pointer += 1;

            delay(150);
        }

        for (int i = 0; i < 18; i++){
            servos[i].servo.setSpeed(180);
        }

        EaseUpdateLegs();
    }
public:
    CustomServo servos[18];
    Spyder spyder;

    SimulationToArduinoTranslator(){}

    void Init(){
        spyder.Init(Position(0,0,0), 1.5, 1.5); // initialize the spyder robot simulation code

        SetOffsetAngles();

        delay(2000);
        spyder.position.y = 0;
        spyder.Update(0);

        ServoStartupSequence();
    }

    void UpdateLegs() {
        // Set Leg group 1
        servos[6].write(- 180 + Degrees(spyder.arms[0].motor1_angle));
        servos[7].write(- Degrees(spyder.arms[0].motor2_angle));
        servos[8].write(- Degrees(spyder.arms[0].motor2_angle) + Degrees(spyder.arms[0].motor3_angle));

        servos[0].write(180 + Degrees(spyder.arms[1].motor1_angle));
        servos[1].write(- Degrees(spyder.arms[1].motor2_angle));
        servos[2].write(- Degrees(spyder.arms[1].motor2_angle) + Degrees(spyder.arms[1].motor3_angle));

        servos[12].write(Degrees(spyder.arms[2].motor1_angle));
        servos[13].write(Degrees(spyder.arms[2].motor2_angle));
        servos[14].write(Degrees(spyder.arms[2].motor2_angle) - Degrees(spyder.arms[2].motor3_angle));

        // Set Leg group 2
        if (Degrees(spyder.arms[3].motor1_angle) < 0)
            servos[3].write(180 + Degrees(spyder.arms[3].motor1_angle));
        else 
            servos[3].write(- 180 + Degrees(spyder.arms[3].motor1_angle));
        servos[4].write(- Degrees(spyder.arms[3].motor2_angle));
        servos[5].write(- Degrees(spyder.arms[3].motor2_angle) + Degrees(spyder.arms[3].motor3_angle));
        
        servos[9].write(Degrees(spyder.arms[4].motor1_angle));
        servos[10].write(Degrees(spyder.arms[4].motor2_angle));
        servos[11].write(Degrees(spyder.arms[4].motor2_angle) - Degrees(spyder.arms[4].motor3_angle));

        servos[15].write(Degrees(spyder.arms[5].motor1_angle));
        servos[16].write(- Degrees(spyder.arms[5].motor2_angle));
        servos[17].write(- Degrees(spyder.arms[5].motor2_angle) + Degrees(spyder.arms[5].motor3_angle));
    }
};

#endif
#pragma once

#include "position.h"
#include "body.h"

#include <SoftwareSerial.h>

struct Joystick{
    float x, z, angle, radius;

    float previous_x, previous_z;
};

struct Timer {
    float start_time;
    float GetTimer(float current_time){
        return current_time - start_time;
    }
};

class Controller{
    Timer joystick_idle;
    bool turn_callibrated, height_change, walk_to_turn;
    float axes[4];

    const byte numChars = 32;
    char receivedChars[(byte)32];
    bool newData = false;

    void StoreAxesData(){
        left_joystick.x = axes[0];
        left_joystick.z = axes[1];
        right_joystick.x = axes[2];
        right_joystick.z = axes[3];
    }
    void CalculateJoystickAngles(){
        left_joystick.angle = atan2(left_joystick.z, left_joystick.x);
        right_joystick.angle = atan2(right_joystick.z, right_joystick.x);
    }
    void CalculateJoystickDistanceFromCenter(){
        left_joystick.radius = sqrt(pow(left_joystick.x,2) + pow(left_joystick.z,2));
        right_joystick.radius = sqrt(pow(right_joystick.x,2) + pow(right_joystick.z,2));
    }
    bool JoystickMovementEratic(Joystick *joystick){
        float distance_travelled = sqrt(pow(joystick->previous_x - joystick->x, 2) + pow(joystick->previous_z - joystick->z, 2));
        joystick->previous_x = joystick->x;
        joystick->previous_z = joystick->z;
        if (distance_travelled > 0.5) return true;
        else return false;
    }

    void RecieveSerialData(SoftwareSerial* HM10) {
        static bool recvInProgress = false;
        static byte ndx = 0;
        char startMarker = '<';
        char endMarker = '>';
        char rc;
    
        while (HM10->available() > 0 && newData == false) {
            rc = HM10->read();

            if (recvInProgress == true) {
                if (rc != endMarker) {
                    receivedChars[ndx] = rc;
                    ndx++;
                    if (ndx >= numChars) {
                        ndx = numChars - 1;
                    }
                }
                else {
                    receivedChars[ndx] = '\0'; // terminate the string
                    recvInProgress = false;
                    ndx = 0;
                    newData = true;
                }
            }

            else if (rc == startMarker) {
                recvInProgress = true;
            }
        }
    }
    void ProcessSerialData() {
        if (newData == true) {
            String recievedData = String(receivedChars);
            Serial.print("processing");
            Serial.println(recievedData);
            int previousSubstring = 0;
            int pointer = 0;
            for (int i = 0; i < recievedData.length(); i++){
                if (recievedData.charAt(i) == ':'){
                    String substring = recievedData.substring(previousSubstring, i-1);
                    previousSubstring = i + 1;

                    axes[pointer] = substring.toFloat();
                    pointer++;
                }
            }
            newData = false;
        }
    }
public:
    Joystick left_joystick, right_joystick;

    Controller(){

    }
    void ProcessInput(SoftwareSerial* HM10){
        RecieveSerialData(HM10);
        ProcessSerialData();
        StoreAxesData();

        CalculateJoystickAngles();
        CalculateJoystickDistanceFromCenter();
    }
    void SendInputToBody(Spyder *body, float delta_time){
        if (left_joystick.radius > 0.2 && !body->turning && !body->resting && !body->walk_to_turn && !JoystickMovementEratic(&left_joystick)){
            body->walking = true;

            body->time_to_take = 0.5 / left_joystick.radius;
            if (body->time_to_take > 0.5) body->time_to_take = 0.5;

            body->walk.x = left_joystick.x/2;
            body->walk.z = left_joystick.z/2;

            Position direction = Normalize((body->position + body->walk) - body->position);

            float x = -direction.x * abs(body->walk.x)/body->time_to_take * delta_time;
            float z = -direction.z * abs(body->walk.z)/body->time_to_take * delta_time;

            float world_radius = sqrt(pow(x, 2) + pow(z, 2));
            float angle = atan2(z, x) + body->world_angle;

            joystick_idle.start_time = 0;
            body->turn_callibrated = false;
        }
        else if (abs(right_joystick.x) > 0.2 && !body->walking && !body->resting && !JoystickMovementEratic(&right_joystick)){
            if (!body->turn_callibrated) {
                body->walk_to_turn = true;
                if (!walk_to_turn) {
                    body->section_number = 0;
                    walk_to_turn = true;
                }
            }
            else {
                body->turning = true;
            }

            body->time_to_take = 0.4 / right_joystick.radius;
            if (body->time_to_take > 0.5) body->time_to_take = 0.5;

            body->turn = right_joystick.x/5;

            body->world_angle += body->turn * 4 * delta_time;

            joystick_idle.start_time = 0;
        }
        else {
            body->walking = false;
            body->turning = false;
            
            if (joystick_idle.start_time == 0){
                joystick_idle.start_time = millis();
            }
            if (joystick_idle.GetTimer(millis()) > 1000){
                joystick_idle.start_time = -1;
                body->resting = true;
            }
            body->walk.x = 0;
            body->walk.z = 0;
            body->turn = 0;

            body->turn_callibrated = false;
        }
    }
};
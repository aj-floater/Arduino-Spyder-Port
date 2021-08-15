#pragma once

#include "position.h"
#include "body.h"

#include <SoftwareSerial.h>
#include <Arduino.h>

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

    const byte numChars = 20;
    char receivedChars[20];
    bool newData = false;

    char state;

    void CalculateJoystickAngles(){
        left_joystick.angle = atan2(left_joystick.z, left_joystick.x);
        right_joystick.angle = atan2(right_joystick.z, right_joystick.x);
    }
    void CalculateJoystickDistanceFromCenter(){
        left_joystick.radius = sqrt(pow(left_joystick.x,2) + pow(left_joystick.z,2));
        right_joystick.radius = sqrt(pow(right_joystick.x,2) + pow(right_joystick.z,2));
    }
    float TransformModifiedSerialData(const char *receivedData, int start_pos){
        char temp_data[4];
        for (int i = start_pos; i < start_pos + 4; i++){
            temp_data[i-start_pos] = receivedData[i];
        }
        float data = atoi(temp_data);
        data = data / 100;
        return data;
    }
    void ProcessSerialData() {
        if (newData == true) {
            // Serial.println(TransformModifiedSerialData(receivedChars, 0));
            left_joystick.x = TransformModifiedSerialData(receivedChars, 0);
            left_joystick.z = TransformModifiedSerialData(receivedChars, 4);
            right_joystick.x = TransformModifiedSerialData(receivedChars, 8);
            right_joystick.z = TransformModifiedSerialData(receivedChars, 12);

            state = receivedChars[16];

            Serial.println(String(left_joystick.x)+' '+String(left_joystick.z)+' '+String(right_joystick.x)+' '+String(right_joystick.z));

            newData = false;
        }
    }
    void RecieveSerialData() {
        static bool recvInProgress = false;
        static byte ndx = 0;
        char startMarker = '<';
        char endMarker = '>';
        char rc;
    
        while (Serial2.available() > 0 && newData == false) {
            rc = Serial2.read();

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
                digitalWrite(LED_BUILTIN, HIGH);
            }

            else if (rc == startMarker) {
                recvInProgress = true;
            }
            else {
                digitalWrite(LED_BUILTIN, LOW);
            }
        }
        // if (!Serial2.available()){
        //     left_joystick.x =  0;
        //     left_joystick.z =  0;
        //     right_joystick.x = 0;
        //     right_joystick.z = 0;
        // }
    }
public:
    Joystick left_joystick, right_joystick;

    Controller(){

    }
    void ProcessInput(){
        RecieveSerialData();
        ProcessSerialData();
        // Serial.println('[' + String(left_joystick.x) + ' ' + String(left_joystick.z) + ' ' + String(right_joystick.x) + ' ' + String(right_joystick.z) + ']');

        CalculateJoystickAngles();
        CalculateJoystickDistanceFromCenter();
    }
    void movementInput(Spyder *body, float delta_time) {
        if (left_joystick.radius > 0.4 && !body->turning && !body->resting && !body->walk_to_turn){
            body->walking = true;

            body->time_to_take = 0.5 / left_joystick.radius;
            if (body->time_to_take > 0.5) body->time_to_take = 0.5;

            body->walk.x = left_joystick.x/2;
            body->walk.z = left_joystick.z/2;

            joystick_idle.start_time = 0;
            body->turn_callibrated = false;
        }
        else if (abs(right_joystick.x) > 0.4 && !body->walking && !body->resting){
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
        else if (abs(right_joystick.z) > 0.4 && !body->turning){
            if (right_joystick.z < 0.4)
                if (body->position.y < 2.1) body->position.y -= right_joystick.z * 0.02;
            if (right_joystick.z > 0.4)
                if (body->position.y > 0) body->position.y -= right_joystick.z * 0.02;
        }
        else {
            body->walking = false;
            body->turning = false;
            
            if (joystick_idle.start_time == 0){
                joystick_idle.start_time = millis();
            }
            if (joystick_idle.GetTimer(millis()) > 2000){
                joystick_idle.start_time = -1;
                body->resting = true;
            }
            body->walk.x = 0;
            body->walk.z = 0;
            body->turn = 0;

            body->turn_callibrated = false;
        }
    }
    void SendInputToBody(Spyder *body, float delta_time){
        if (state == 'm'){
            movementInput(body, delta_time);
        }
        if (state == 'r'){
            if (right_joystick.radius > 0.4 && !body->turning && !body->resting && !body->walk_to_turn){
                if (right_joystick.x < 0.55)
                    if (body->x_rotation < 2.1) body->x_rotation -= right_joystick.x * 0.015;
                if (right_joystick.x > -0.55)
                    if (body->x_rotation > 0) body->x_rotation -= right_joystick.x * 0.015;
                if (right_joystick.z < 0.26)
                    if (body->z_rotation < 2.1) body->z_rotation -= right_joystick.z * 0.015;
                if (right_joystick.z > -0.26)
                    if (body->z_rotation > 0) body->z_rotation -= right_joystick.z * 0.015;
            }
        }
        
    }
};
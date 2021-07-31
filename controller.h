#pragma once

#include "/Users/archiejames/Desktop/Coding/Arduino Spyder Port/position.h"
#include "/Users/archiejames/Desktop/Coding/Arduino Spyder Port/body.h"


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
    int axesCount;
    float axes[4];
    Timer joystick_idle;
    bool turn_callibrated, height_change, walk_to_turn;

    void TranslateFromSerial(){
        Serial.
    }
    void RecieveAxesData(){
        axes = TranslateFromSerial();
    }
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
public:
    int gamepad_present;
    Joystick left_joystick, right_joystick;

    Controller(){}
    void ProcessInput(){
        gamepad_present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        if (gamepad_present){
            RecieveAxesData();
            StoreAxesData();
            CalculateJoystickAngles();
            CalculateJoystickDistanceFromCenter();
        }
    }
    void SendInputToBody(Spyder *body){
        if (left_joystick.radius > 0.2 && !body->turning && !body->resting && !body->walk_to_turn && !JoystickMovementEratic(&left_joystick)){
            body->walking = true;

            body->time_to_take = 0.5 / left_joystick.radius;
            if (body->time_to_take > 0.5) body->time_to_take = 0.5;

            body->walk.x = left_joystick.x/2;
            body->walk.z = left_joystick.z/2;

            Position direction = Normalize((body->position + body->walk) - body->position);

            float x = -direction.x * abs(body->walk.x)/body->time_to_take * PropertyManager::delta_time;
            float z = -direction.z * abs(body->walk.z)/body->time_to_take * PropertyManager::delta_time;

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

            body->time = 0.4 / right_joystick.radius;
            if (body->time > 0.5) body->time = 0.5;

            body->turn = right_joystick.x/5;

            body->world_angle += body->turn * 4 * PropertyManager::delta_time;

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
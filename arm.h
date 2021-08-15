#ifndef ARM_H
#define ARM_H

#include "position.h"

enum MovementType {
    STRAIGHT,
    CIRCULAR
};

class Arm {
public:
    Position start_point;
    Position end_point;
    Position end_rest_point;

    float delta_time;

    float limb1_length, limb2_length, limb3_length;
    float motor1_angle, motor2_angle, motor3_angle;

    float motor1_offset_angle, motor2_offset_angle, motor3_offset_angle;

    int ID;

    char MovementType;

    // Variables for STRAIGHT Animations
    float distance, speed, y_offset_angle;
    bool moving, apply_ground_offset, middle;
    Position direction, start, end;
    // Variables for CIRCULAR Animations
    float start_angle, end_angle, rest_angle, radius, direction_angle;

    Arm(){};
    Arm(Position start_point, Position end_point, float limb1_length, float limb2_length, float limb3_length){
        Init(start_point, end_point, limb1_length, limb2_length, limb3_length);
    }

    void Init(Position start_point_input, Position end_point_input, float limb1_length, float limb2_length, float limb3_length){
        this->start_point.x = start_point_input.x;
        this->start_point.y = start_point_input.y;
        this->start_point.z = start_point_input.z;
        
        this->end_point.x = end_point_input.x;
        this->end_point.y = end_point_input.y;
        this->end_point.z = end_point_input.z;

        this->limb1_length = limb1_length;
        this->limb2_length = limb2_length;
        this->limb3_length = limb3_length;
    }

    void ComputeInverseKinematics(){
        float x = end_point.x - start_point.x;
        float y = end_point.y - start_point.y;
        float z = end_point.z - start_point.z;
        motor1_angle = atan2(z, x);

        x = (end_point.z/sinf(motor1_angle)) - ((start_point.z/sinf(motor1_angle)) + limb1_length);
        if (end_point.z == start_point.z)
            x = (end_point.x/cosf(motor1_angle)) - ((start_point.x/cosf(motor1_angle)) + limb1_length);

        float q2 = M_PI - acosf((powf(limb2_length, 2) + powf(limb3_length, 2) - powf(x, 2) - powf(y, 2))/(2*limb2_length*limb3_length));
        float q1 = atan2f(y,x) + atan2f((limb3_length*sinf(q2)),(limb2_length+limb3_length*cosf(q2))) - q2;

        motor2_angle = q1 + q2;
        motor3_angle = q1;
    }

    void MoveEndPositionTo(Position new_position, float time, bool apply_ground_offset){
        start = end_point;
        end = new_position;
        distance = DistanceNoY(start, end);
        this->speed = distance / time;
        // if (distance < 0.2) this->speed = distance / (time * 2);
        direction = Normalize(end - start);
        moving = true;
        this->apply_ground_offset = apply_ground_offset;
        // this->apply_ground_offset = false;
        y_offset_angle = 0;
        MovementType = STRAIGHT;
    }

    void ApplySTRAIGHTMovement(){
        end_point.x += direction.x * speed * delta_time;
        end_point.y += direction.y * speed * delta_time;
        end_point.z += direction.z * speed * delta_time;

        float distance_from_target = sqrt(pow(end_point.x-start.x,2) + pow(end_point.z-start.z,2));

        if (apply_ground_offset){
            float time = distance/speed;
            float height = 0.75;
            float y = height * sinf(y_offset_angle);
            y_offset_angle += M_PI/time * delta_time;
            end_point.y = y;
        }

        if (distance_from_target >= distance){
            end_point.x = end.x;
            end_point.y = end.y;
            end_point.z = end.z;
            moving = false;
        }
    }

    float smallestAngleBetween(float targetA, float sourceA){
        float a = targetA - sourceA;
        if (a > M_PI) a -= M_PI*2;
        if (a < -M_PI) a += M_PI*2;
        return a;
    }

    void MoveEndPositionAroundPointAtAngle(Position *point, float angle, float time, int ID, bool apply_ground_offset){
        float current_angle = atan2(end_point.z - point->z, end_point.x - point->x);
        if (ID == 3 && current_angle < 0){
            current_angle = M_PI*2 + current_angle;
        }
        float smallest_angle = smallestAngleBetween(angle, current_angle);
        start_angle = current_angle;
        end_angle = angle;
        if (smallest_angle < 0){
            direction_angle = -1;
            distance = fabs(smallest_angle);
        }
        if (smallest_angle > 0){
            direction_angle = 1;
            distance = fabs(smallest_angle);
        }
        this->speed = distance/time;
        this->ID = ID; 
        moving = true;
        this->apply_ground_offset = apply_ground_offset;
        y_offset_angle = 0;
        MovementType = CIRCULAR;
    }

    void ApplyCIRCULARMovement(){
        start_angle += direction_angle * speed * delta_time;
        end_point.x = radius * cos(start_angle);
        end_point.z = radius * sin(start_angle);

        if (apply_ground_offset){
            float time = fabs(distance)/speed;
            float height = 0.75;
            float y = height * sinf(y_offset_angle);
            y_offset_angle += M_PI/time * delta_time;
            end_point.y = y;
        }
        
        if (direction_angle > 0 && start_angle >= end_angle){
            end_point.x = radius * cos(end_angle);
            end_point.z = radius * sin(end_angle);
            moving = false;
        }
        else if (direction_angle < 0 && start_angle <= end_angle){
            end_point.x = radius * cos(end_angle);
            end_point.z = radius * sin(end_angle);
            moving = false;
        }
    }

    void Update(float delta_time){
        ComputeInverseKinematics();

        this->delta_time = delta_time;

        if (moving){
            if (MovementType == STRAIGHT)
                ApplySTRAIGHTMovement();
            else if (MovementType == CIRCULAR){
                ApplyCIRCULARMovement();
            }
        }
    }
};

#endif
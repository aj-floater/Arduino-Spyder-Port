#ifndef BODY_H
#define BODY_H

#include "arm.h"

class Spyder{
public:
    Position position;
    float world_angle;

    float x_rotation, z_rotation;
    float width, length;
    Arm arms[6];

    Position walk;
    float turn, time_to_take;
    bool walking, walk_to_turn, turning, resting;
    bool turn_callibrated;
    int section_number = 0;

    float angle;
    
    Spyder(){}
    
    void Init(Position position, float width, float length){
        this->position.x = position.x;
        this->position.y = position.y;
        this->position.z = position.z;

        this->width = width;
        this->length = length;

        for (int i = 0; i < 6; i++) arms[i].Init(Position(0, 0, 0), Position(0, 0, 0), 0.5, 1, 1.5);
        // Arm set 1
        SetArmEndPoints(&arms[0], Position(position.x - width/2 - 1, 0, position.z + length/2 +1));
        SetArmEndPoints(&arms[1], Position(position.x - width/2 - 1, 0, position.z + -length/2-1));
        SetArmEndPoints(&arms[2], Position(position.x + width/2 + 1.75 , 0, position.z             ));
        // Arm se 2
        SetArmEndPoints(&arms[3], Position(position.x - width/2 - 1.75 , 0, position.z             ));
        SetArmEndPoints(&arms[4], Position(position.x + width/2 + 1, 0, position.z + -length/2-1));
        SetArmEndPoints(&arms[5], Position(position.x + width/2 + 1, 0, position.z + length/2 +1));

        for (int i = 0; i < 6; i++) {
            arms[i].rest_angle = atan2(arms[i].end_point.z - position.z, arms[i].end_point.x - position.x);
            arms[i].radius = sqrt(pow(arms[i].end_point.z - position.z, 2) + pow(arms[i].end_point.x - position.x, 2));
        }

        section_number = 0;

        // for (int i = 0; i < 6; i++) arms[i].MoveEndPositionTo(Position(arms[i].end_point.x, arms[i].end_point.y, arms[i].end_point.z+1));
    }

    float ApplyRotations(Arm *arm){
        arm->start_point.x = cosf(x_rotation) * (arm->start_point.x-position.x) - sinf(x_rotation) * (arm->start_point.y-position.y) + position.x; // apply x_rotation to x value
        arm->start_point.y = sinf(x_rotation) * (arm->start_point.x-position.x) + cosf(x_rotation) * (arm->start_point.y-position.y) + position.y; // apply x_rotation to y value
        arm->start_point.z = cosf(z_rotation) * (arm->start_point.z-position.z) - sinf(z_rotation) * (arm->start_point.y-position.y) + position.z; // apply z_rotation to x value
        arm->start_point.y = sinf(z_rotation) * (arm->start_point.z-position.z) + cosf(z_rotation) * (arm->start_point.y-position.y) + position.y; // apply z_rotation to y value
    }

    void SetArmStartPoint(Arm *arm, Position start_point){
        arm->start_point.x = start_point.x;
        arm->start_point.y = start_point.y;
        arm->start_point.z = start_point.z;
    }

    void SetArmEndPoints(Arm *arm, Position end_point){
        arm->end_point.x = end_point.x;
        arm->end_point.y = end_point.y;
        arm->end_point.z = end_point.z;

        arm->end_rest_point.x = end_point.x;
        arm->end_rest_point.y = end_point.y;
        arm->end_rest_point.z = end_point.z;
    }

    void SetArmRestPoint(Arm *arm, Position end_rest_point){
        arm->end_rest_point.x = end_rest_point.x;
        arm->end_rest_point.y = end_rest_point.y;
        arm->end_rest_point.z = end_rest_point.z;
    }

    void Update(float delta_time){
        // Arm set 1
        SetArmStartPoint(&arms[0], Position(position.x + -width/2+0.25, position.y, position.z + length/2 ));
        SetArmStartPoint(&arms[1], Position(position.x + -width/2+0.25, position.y, position.z + -length/2));
        SetArmStartPoint(&arms[2], Position(position.x + width/2,       position.y, position.z            ));
        // Arm set 2
        SetArmStartPoint(&arms[3], Position(position.x + -width/2,      position.y, position.z            ));
        SetArmStartPoint(&arms[4], Position(position.x + width/2-0.25,  position.y, position.z + -length/2));
        SetArmStartPoint(&arms[5], Position(position.x + width/2-0.25,  position.y, position.z + length/2 ));

        // Arm set 1
        SetArmRestPoint(&arms[0], Position(position.x - width/2 - 1, 0, position.z + length/2 +1));
        SetArmRestPoint(&arms[1], Position(position.x - width/2 - 1, 0, position.z + -length/2-1));
        SetArmRestPoint(&arms[2], Position(position.x + width/2 + 1.75 , 0, position.z             ));
        // Arm se 2
        SetArmRestPoint(&arms[3], Position(position.x - width/2 - 1.75 , 0, position.z             ));
        SetArmRestPoint(&arms[4], Position(position.x + width/2 + 1, 0, position.z + -length/2-1));
        SetArmRestPoint(&arms[5], Position(position.x + width/2 + 1, 0, position.z + length/2 +1));

        bool all_stopped = true;
        for (int i = 0; i < 6; i++){
            if (arms[i].moving) all_stopped = false;
        }
        if (all_stopped && walking){
            if (section_number == 0){
                for (int i = 0; i < 3; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x + walk.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z + walk.z), time_to_take, true);
                }
                for (int i = 3; i < 6; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x - walk.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z - walk.z), time_to_take, false);
                }
                section_number = 1;
            }
            else if (section_number == 1){
                for (int i = 3; i < 6; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x + walk.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z + walk.z), time_to_take, true);
                }
                for (int i = 0; i < 3; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x - walk.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z - walk.z), time_to_take, false);
                }
                section_number = 0;
            }
        }
        if (all_stopped && walk_to_turn){
            if (section_number == 0){
                for (int i = 0; i < 3; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].radius * cosf(arms[i].rest_angle - turn), arms[i].end_rest_point.y, arms[i].radius * sinf(arms[i].rest_angle - turn)), time_to_take, false);
                }
                for (int i = 3; i < 6; i++){
                    arms[i].MoveEndPositionTo(Position(arms[i].radius * cosf(arms[i].rest_angle + turn), arms[i].end_rest_point.y, arms[i].radius * sinf(arms[i].rest_angle + turn)), time_to_take, true);
                }
                section_number = 1;
            }
            else if (section_number == 1){
                turn_callibrated = true;
                section_number = 0;
                walk_to_turn = false;
            }
        }
        if (all_stopped && turning){
            if (section_number == 0){
                for (int i = 0; i < 3; i++){
                    arms[i].MoveEndPositionAroundPointAtAngle(&position, arms[i].rest_angle + turn, time_to_take, i, true);
                }
                for (int i = 3; i < 6; i++){
                    arms[i].MoveEndPositionAroundPointAtAngle(&position, arms[i].rest_angle - turn, time_to_take, i, false);
                }
                section_number = 1;
            }
            else if (section_number == 1){
                for (int i = 3; i < 6; i++){
                    arms[i].MoveEndPositionAroundPointAtAngle(&position, arms[i].rest_angle + turn, time_to_take, i, true);
                }
                for (int i = 0; i < 3; i++){
                    arms[i].MoveEndPositionAroundPointAtAngle(&position, arms[i].rest_angle - turn, time_to_take, i, false);
                }
                section_number = 0;
            }
        }
        if (all_stopped && resting){
            bool rested = true;
            for (int i = 0; i < 6; i++){
                if (Distance(arms[i].end_rest_point, arms[i].end_point) > 0) rested = false;
            }
            if (!rested){
                if (section_number == 0){
                    for (int i = 0; i < 3; i++){
                        arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z), 0.5, true);
                    }
                    section_number = 1;
                }
                else if (section_number == 1){
                    for (int i = 3; i < 6; i++){
                        arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z), 0.5, true);
                    }
                    section_number = 0;
                }
            }
            if (rested){
                resting = false;
                section_number = 0;
            }
        }

        if (all_stopped && resting){
            bool rested = true;
            for (int i = 0; i < 6; i++){
                if (Distance(arms[i].end_rest_point, arms[i].end_point) > 0) rested = false;
            }
            if (!rested){
                if (section_number == 0){
                    for (int i = 0; i < 3; i++){
                        arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z), 0.5, true);
                    }
                    section_number = 1;
                }
                else if (section_number == 1){
                    for (int i = 3; i < 6; i++){
                        arms[i].MoveEndPositionTo(Position(arms[i].end_rest_point.x, arms[i].end_rest_point.y, arms[i].end_rest_point.z), 0.5, true);
                    }
                    section_number = 0;
                }
            }
            if (rested){
                resting = false;
                section_number = 0;
            }
        }

        for (int i = 0; i < 6; i++){
            ApplyRotations(&arms[i]);
            arms[i].Update(delta_time);
        }
    }
};

#endif
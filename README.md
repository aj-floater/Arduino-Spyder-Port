# Hexapod Robot Arduino Code

This repository contains the Arduino code for controlling a hexapod robot. The code is written in C++ and is intended to be run on an Arduino microcontroller board.

## Description

The hexapod robot is a six-legged walking robot designed to mimic the movement of insects. It consists of six legs, each controlled by three servo motors. The code implements the control logic for coordinating the movements of the legs to achieve walking, turning, and resting motions.

## Files

- `customservo.ino`: This file includes the code for controlling the servo motors used in the hexapod robot. It defines functions for initializing and updating the servo positions.
- `controller.h`: This file contains the code for the controller class, which handles the input and output of the robot. It processes input commands received from an external device and sends control signals to the hexapod robot.
- `body.h`: This file defines the Spyder class, which represents the body of the hexapod robot. It contains functions for initializing the body, updating its state, and performing various motions such as walking, turning, and resting.
- `arm.h`: This file defines the Arm class, which represents an individual arm of the hexapod robot. It includes functions for setting the arm's start and end points, calculating angles for servo motors, and performing movements.

## Usage

The hexapod robot can be controlled using the HM10 Bluetooth module or by sending commands directly to the Arduino board. The controller class handles the input commands and sends corresponding control signals to the hexapod robot.

The code provides several functions for controlling the robot, including:

- Walking: The robot can perform walking motions by moving its legs in a coordinated manner.
- Turning: The robot can turn by adjusting the angles of its legs.
- Resting: The robot can assume a resting position with its legs in a relaxed state.

To control the robot, send commands through the HM10 Bluetooth module or modify the code to incorporate other input methods such as sensors or another remote control.

## Contributing

Contributions to this Arduino code are welcome. If you find any issues or have suggestions for improvements, please feel free to open an issue or submit a pull request.

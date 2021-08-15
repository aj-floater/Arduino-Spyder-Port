#include "translator.h"
#include "controller.h"

#define spyder sim.spyder

SimulationToArduinoTranslator sim;
Controller controller;

SoftwareSerial HM10(10, 9); // RX = 2, TX = 3

float currentTime = 0, previousTime = 0;
float CalculateDeltaTime(){
    currentTime = millis();
    currentTime /=1000;
    float delta_time = currentTime - previousTime;
    previousTime = currentTime;
    return delta_time;
}

float delta_time;

void setup (){
    Serial.begin(9600);
    // Serial.println("SPYDER ROBOT STARTING...");
    Serial2.begin(9600);

    while (!Serial2.available()) {}

    sim.Init();

    // spyder.rest_time = 2;
    // spyder.resting = true;
    // while (spyder.resting) {
    //     delta_time = CalculateDeltaTime();
    //     sim.UpdateLegs();
    //     spyder.Update(delta_time);
    // }
    spyder.rest_time = 0.5;

    // while (spyder.position.y < 1.1) {
    //     spyder.position.y += 0.005;
    //     sim.UpdateLegs();P
    //     spyder.Update(0);
    // }

    delay(500);
}

void loop () {
    delta_time = CalculateDeltaTime();
    controller.ProcessInput();
    controller.SendInputToBody(&spyder, delta_time);
    spyder.Update(delta_time);

    for (int i = 0; i < 18; i++){
        if (!sim.servos[i].servo.attached()){
            // Serial.println("attatching dettached servo");
            sim.servos[i].servo.attach(sim.servos[i].pin);
        }
    }

    sim.UpdateLegs();
}
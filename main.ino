#include "/Users/archiejames/Desktop/Coding/Arduino Spyder Port/customservo.ino"
#include "/Users/archiejames/Desktop/Coding/Arduino Spyder Port/controller.h"

#include <SoftwareSerial.h>

#define spyder sim.spyder

SimulationToArduinoTranslator sim;
Controller controller;

float currentTime;
float previousTime;
float CalculateDeltaTime(){
    currentTime = millis();
    currentTime /=1000;
    float delta_time = currentTime - previousTime;
    previousTime = currentTime;
    return delta_time;
}

int Degrees(float radians){
    int degrees = radians * 180/PI;
    return degrees;
}

float delta_time;

void setup (){
    Serial.begin(9600);
    Serial.println("SPYDER ROBOT STARTING...");

    sim.Init();

    while (spyder.position.y < 0.9) {
        spyder.position.y += 0.005;
        spyder.Update(delta_time);
        sim.UpdateLegs();
    }

    delay(500);
}

void loop () {
    delta_time = CalculateDeltaTime();
    spyder.Update(delta_time);

    for (int i = 0; i < 18; i++){
        if (!sim.servos[i].servo.attached()){
            Serial.println("attatching dettached servo");
            sim.servos[i].servo.attach(sim.servos[i].pin);
        }
    }

    sim.UpdateLegs();
}
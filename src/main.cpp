#include <Arduino.h>
#include <HardwareSerial.cpp>
#include "../lib/Motor/Motor.h"


Rover rover = Rover();

class RoverCtl {
    unsigned long ui_delay;
    int speed;
    unsigned char c;

    void handleFwd() const {
        rover.forward(speed);
        Serial.println("Forward.");
    }

    void handleBack() const {
        rover.backward(speed);
        Serial.println("Backward.");
    }

    static void handleStop() {
        rover.stop();
        Serial.println("Stop.");
    }

    void handleTurnRight() const {
        rover.turnRight(speed);
        Serial.println("Turn right.");
    }

    void handleTurnLeft() const {
        rover.turnLeft(speed);
        Serial.println("Turn left.");
    }

    void handleReduceSpeed() {
        speed -= 10;
        if (speed < 0) {
            speed = 0;
        }
        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.println();
        ui_delay = millis() + 250;
    }

    void handleIncreaseSpeed() {
        speed += 10;
        if (speed > 255) {
            speed = 255;
        }
        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.println();
        ui_delay = millis() + 250;
    }

public:
    RoverCtl() {
        ui_delay = 0;
        speed = 200;
        c = 0;
    }

    void handleSerialCommands() {
        if (millis() > ui_delay) {
            ui_delay = 0;
        } else {
            return;
        }
        if (Serial.available() > 0) {
            c = Serial.read();
            switch(c) {
                case '=':
                    handleIncreaseSpeed();
                    break;
                case '-':
                    handleReduceSpeed();
                    break;
                case 'a':
                    handleTurnLeft();
                    break;
                case 'w':
                    handleFwd();
                    break;
                case 'd':
                    handleTurnRight();
                    break;
                case 's':
                    handleBack();
                    break;
                case 'x':
                    handleStop();
                    break;

            };
        }
    }

    static void initSerial() {
        Serial.begin(9600);
    }
};

RoverCtl roverCtl;
void setup() {
    RoverCtl::initSerial();
}


void loop() {
    roverCtl.handleSerialCommands();
}
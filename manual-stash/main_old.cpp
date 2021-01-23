#include <Arduino.h>
#include <HardwareSerial.cpp>
#include "lib/Motor/Motor.h"

// Hint:
// Tools -> PlatformIO -> Update All
// Needed to get intellisense working.

// A255B25500

class RoverSerial
    uint8_t bytes[3] = {};
    uint8_t robot_state = 0;
public:
    void parseSerial() {
        if (Serial.available() >= 3) {
            Serial.readBytes(bytes, 3);
            motorA_state    = bytes[BYTE_MOTOR_A];
            motorB_state    = bytes[BYTE_MOTOR_B];
            robot_state     = bytes[BYTE_ROBOT_STATE];

            Serial.print(motorADirection());
            Serial.print(motorBDirection());
            Serial.print(robotIsEnabled());
        }
    }

    bool robotIsEnabled() const {
        return robot_state & BITMASK_ROBOT_ENABLED;
    }

    bool motorADirection() const {
        return robot_state & BITMASK_ROBOT_MOTOR_A_DIRECTION;
    }

    bool motorBDirection() const {
        return robot_state & BITMASK_ROBOT_MOTOR_B_DIRECTION;
    }

    uint8_t motorA_state = 0;
    uint8_t motorB_state = 0;
};

Rover rover = Rover();
RoverSerial roverSerial;

void setup() {
    Serial.begin(9600);
}

void loop() {
    roverSerial.parseSerial();
  // rover.motorA->setMotor(255, roverSerial.motorADirection());
   rover.motorB->setMotor(roverSerial.motorB_state, roverSerial.motorBDirection());


//    if (Serial.available() > 0) {
//        c = Serial.read();
//        if (c == 'a') {
//            rover.turnLeft(255);
//        }
//        else if (c == 'w') {
//            rover.forward(255);
//        }
//        else if (c == 'd') {
//            rover.turnRight(255);
//        }
//        else if (c == 's') {
//            rover.backward(255);
//        }
//        else if (c == 'x') {
//            rover.stop();
//        }
//    }
}
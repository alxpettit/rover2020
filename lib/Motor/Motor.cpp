#include "Motor.h"
#include "Arduino.h"

void Motor::setMotor(int speed, int direction) const {
    if (direction == CLOCKWISE) {
        analogWrite(Motor::pin_clockwise, 255 - speed);
        analogWrite(Motor::pin_counter, 255);
    } else {
        analogWrite(Motor::pin_clockwise, 255);
        analogWrite(Motor::pin_counter, 255 - speed);
    }
}

void Motor::init() const {
    pinMode(Motor::pin_clockwise, OUTPUT);
    pinMode(Motor::pin_counter, OUTPUT);
    // set pullup resistor
    digitalWrite(Motor::pin_clockwise, HIGH);
    digitalWrite(Motor::pin_counter, HIGH);
}

Motor::Motor(int new_pin_clockwise, int new_pin_counter) {
    Motor::pin_clockwise = new_pin_clockwise;
    Motor::pin_counter = new_pin_counter;
    init();
}


Rover::Rover(Motor* motorA, Motor* motorB) {
    if (motorA == nullptr) Rover::motorA = new Motor(MOTOR_A_CLOCK, MOTOR_A_COUNTER);
    else Rover::motorA = motorA;
    if (motorB == nullptr) Rover::motorB = new Motor(MOTOR_B_CLOCK, MOTOR_B_COUNTER);
    else Rover::motorB = motorB;
}

void Rover::forward(int speed) {
    motorA->setMotor(speed, CLOCKWISE);
    motorB->setMotor(speed, COUNTER_CLOCKWISE);
}

void Rover::backward(int speed) {
    motorA->setMotor(speed, COUNTER_CLOCKWISE);
    motorB->setMotor(speed, CLOCKWISE);
}


void Rover::turnLeft(int speed) {
    motorA->setMotor(speed, COUNTER_CLOCKWISE);
    motorB->setMotor(speed, COUNTER_CLOCKWISE);
}


void Rover::turnRight(int speed) {
    motorA->setMotor(speed, CLOCKWISE);
    motorB->setMotor(speed, CLOCKWISE);
}

void Rover::stop() {
    motorA->setMotor(0, CLOCKWISE);
    motorB->setMotor(0, CLOCKWISE);
}
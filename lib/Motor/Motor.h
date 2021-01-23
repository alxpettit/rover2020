#ifndef ROVER_MOTOR_H
#define ROVER_MOTOR_H

#define CLOCKWISE 0
#define COUNTER_CLOCKWISE 1

#ifndef MOTOR_A_CLOCK
#define MOTOR_A_CLOCK 5
#endif

#ifndef MOTOR_A_COUNTER
#define MOTOR_A_COUNTER 3
#endif

#ifndef MOTOR_B_CLOCK
#define MOTOR_B_CLOCK 9
#endif

#ifndef MOTOR_B_COUNTER
#define MOTOR_B_COUNTER 7
#endif

class Motor {
    int pin_clockwise, pin_counter;

public:
    Motor(int new_pin_clockwise, int new_pin_counter);

    void init() const;

    void setMotor(int speed, int direction) const;
};

class Rover {
public:
    Motor* motorA = nullptr;
    Motor* motorB = nullptr;
    explicit Rover(Motor* motorA = nullptr, Motor* motorB = nullptr);

    void forward(int speed);

    void backward(int speed);

    void turnLeft(int speed);

    void turnRight(int speed);

    void stop();

};

#endif //ROVER_MOTOR_H

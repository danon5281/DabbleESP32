#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "motorControls.h"

int motorControls::minPulseWidth = 544;
int motorControls::maxPulseWidth = 2400;
int motorControls::minAngle = 0;
int motorControls::maxAngle = 180;

motorControls::motorControls() : ModuleParent(CONTROLS_ID) {}

void motorControls::processData() {

    functionId = getDabbleInstance().getFunctionId();

    if(functionId == 0x01 || functionId == 0x02) {

        byte1 = getDabbleInstance().getArgumentData(0)[0]; 
        byte2 = getDabbleInstance().getArgumentData(0)[1];

        if(functionId == 0x01) {
            if(byte1 == 0xf0)      pwmMotor1 = byte2;
            else if(byte1 == 0xff) pwmMotor1 = -byte2;
            else                   pwmMotor1 = 0;
        }

        if(functionId == 0x02) {
            if(byte1 == 0xf0)      pwmMotor2 = byte2;
            else if(byte1 == 0xff) pwmMotor2 = -byte2;
            else                   pwmMotor2 = 0;
        }
    }

    else if(functionId == 0x03 || functionId == 0x04) {

        byte1 = getDabbleInstance().getArgumentData(0)[0];

        if(functionId == 0x03)      angleServo1 = byte1;
        else if(functionId == 0x04) angleServo2 = byte1;
    }
}


// ======================================================================
//                          MOTOR 1  (Channel 0)
// ======================================================================
void motorControls::runMotor1(uint8_t pwmPin, uint8_t direction1, uint8_t direction2) {

    pinMode(direction1, OUTPUT);
    pinMode(direction2, OUTPUT);

    if(prevMotor1pin != pwmPin) {
        ledcSetup(0, 100, 8);           // channel 0 / 100 Hz / 8-bit
        ledcAttachPin(pwmPin, 0);
        prevMotor1pin = pwmPin;
    }

    if(pwmMotor1 > 0) {
        digitalWrite(direction1, HIGH);
        digitalWrite(direction2, LOW);
        ledcWrite(0, pwmMotor1);
    }
    else if(pwmMotor1 < 0) {
        digitalWrite(direction1, LOW);
        digitalWrite(direction2, HIGH);
        ledcWrite(0, -pwmMotor1);
    }
    else {
        digitalWrite(direction1, LOW);
        digitalWrite(direction2, LOW);
        ledcWrite(0, 0);
    }
}


// ======================================================================
//                          MOTOR 2  (Channel 1)
// ======================================================================
void motorControls::runMotor2(uint8_t pwmPin, uint8_t direction1, uint8_t direction2) {

    pinMode(direction1, OUTPUT);
    pinMode(direction2, OUTPUT);

    if(prevMotor2pin != pwmPin) {
        ledcSetup(1, 100, 8);           // channel 1
        ledcAttachPin(pwmPin, 1);
        prevMotor2pin = pwmPin;
    }

    if(pwmMotor2 > 0) {
        digitalWrite(direction1, HIGH);
        digitalWrite(direction2, LOW);
        ledcWrite(1, pwmMotor2);
    }
    else if(pwmMotor2 < 0) {
        digitalWrite(direction1, LOW);
        digitalWrite(direction2, HIGH);
        ledcWrite(1, -pwmMotor2);
    }
    else {
        digitalWrite(direction1, LOW);
        digitalWrite(direction2, LOW);
        ledcWrite(1, 0);
    }
}


// ======================================================================
//                              SERVO 1  (Channel 3)
// ======================================================================
void motorControls::runServo1(uint8_t pin) {
    if(prevServo1pin != pin) {
        ledcSetup(3, 50, 16);    // 50 Hz, 16 bit resolution
        ledcAttachPin(pin, 3);
        prevServo1pin = pin;
    }
    writeServoAngle(angleServo1, 3);
}


// ======================================================================
//                              SERVO 2  (Channel 4)
// ======================================================================
void motorControls::runServo2(uint8_t pin) {
    if(prevServo2pin != pin) {
        ledcSetup(4, 50, 16);
        ledcAttachPin(pin, 4);
        prevServo2pin = pin;
    }
    writeServoAngle(angleServo2, 4);
}


//==================================================================
//                      SERVO UTILITIES
//==================================================================
int motorControls::angleTomicroseconds(int degree) {
    degree = constrain(degree, minAngle, maxAngle);
    return map(degree, minAngle, maxAngle, minPulseWidth, maxPulseWidth);
}

int motorControls::microsecondsToDuty(int pulse) {
    pulse = constrain(pulse, minPulseWidth, maxPulseWidth);
    return map(pulse, 0, 20000, 0, 65535);
}

void motorControls::writeServoAngle(int angle, uint8_t channel) {
    int _pulse = angleTomicroseconds(angle);
    int _duty  = microsecondsToDuty(_pulse);
    ledcWrite(channel, _duty);
}

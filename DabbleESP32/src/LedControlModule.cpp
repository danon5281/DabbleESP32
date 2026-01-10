#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "LedControlModule.h"
#include <driver/ledc.h>

#ifdef ESP32
#define MAX_PWM_CHANNELS 8
uint8_t freeChannel = 0, prevPin = 0, currentChannel = 0;
bool pinType = 0;
#endif

LedControlModule::LedControlModule() : ModuleParent(LEDCONTROL_ID) {}

void LedControlModule::processData()
{
    byte functionId = getDabbleInstance().getFunctionId();

    if(functionId == 0x01)   // выбор пина
    {
        pin = getDabbleInstance().getArgumentData(0)[0]; 

        if(prevPin != pin)
        {
            if(freeChannel < MAX_PWM_CHANNELS)
            {
                currentChannel = freeChannel;

                // Настройка канала PWM
                ledcSetup(currentChannel, 12000, 8);
                ledcAttachPin(pin, currentChannel);

                freeChannel++;
                pinType = 1;       // PWM
            }
            else
            {
                pinMode(pin, OUTPUT);
                pinType = 0;       // цифровой вывод
            }
        }
        prevPin = pin;
    }

    else if(functionId == 0x03) // Включить с яркостью
    {
        brightness = getDabbleInstance().getArgumentData(0)[0];
        value = map(brightness, 0, 100, 0, 255);
    }

    else if(functionId == 0x02) // Выключить
    {
        brightness = 0;
        value = 0;
    }

    // Вывод
    if(pinType == 1 && pin != 0)
    {
        ledcWrite(currentChannel, value);
    }
    else
    {
        digitalWrite(pin, value == 255 ? HIGH : LOW);
    }
}

uint8_t LedControlModule::readBrightness()
{
    return brightness;
}

bool LedControlModule::getpinState()
{
    return (brightness > 0);
}

uint8_t LedControlModule::getpinNumber()
{
    return pin;
}

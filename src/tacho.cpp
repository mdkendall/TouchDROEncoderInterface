#include "tacho.h"

static gpio_num_t _pin;
static int _pulsesPerRev;
static unsigned long _pulseIntervalMax;
static volatile unsigned long _pulseTime;
static volatile unsigned long _pulseInterval;

static void IRAM_ATTR tachoISR();

void tachoInit(int pin, int pulsesPerRev) {
    _pin = (gpio_num_t)pin;
    _pulsesPerRev = pulsesPerRev;
    _pulseTime = 0;
    _pulseInterval = 0;
    _pulseIntervalMax = 3E6 / _pulsesPerRev; // 20 RPM
    gpio_pad_select_gpio(_pin);
    gpio_set_direction(_pin, GPIO_MODE_INPUT);
    gpio_pullup_en(_pin);
    attachInterrupt(_pin, tachoISR, RISING);
}

float tachoGetRPM(void) {
    if ((micros() - _pulseTime) > _pulseIntervalMax) return 0.;
    if (_pulseInterval == 0) return 0.;
    return 60.E6 / (_pulseInterval * _pulsesPerRev);
}

void IRAM_ATTR tachoISR() {
    unsigned long now = micros();
    unsigned long interval = now - _pulseTime;
    /* debounce; intervals less than 1ms are not believeable */
    if (interval > 1000) {
        _pulseInterval = (15 * _pulseInterval + interval) / 16;
        _pulseTime = now;
    }
}

#include <Arduino.h>
#include <ESP32Encoder.h>
#include <BluetoothSerial.h>
#include "tacho.h"

typedef struct axis_s {
    /* configuration */
    char            id;         /* identifier for axis in reports */
    ESP32Encoder    encoder;    /* encoder connected to axis */
    int             pinA;       /* GPIO pin A */
    int             pinB;       /* GPIO pin B */
    int             backlash;   /* total backlash in counts */
    /* state */
    int64_t         cnt;        /* most recent count */
    int             blCnt;      /* position within the backlash deadzone */
} axis_t;

axis_t  axis[] = {
    {'x', ESP32Encoder(), 18, 19, 20, 0, 0},
    {'y', ESP32Encoder(), 23, 25, 10, 0, 0},
    {'z', ESP32Encoder(), 26, 27, 7, 0, 0},
};

#define AXIS_NUM    (sizeof(axis)/sizeof(axis_t))
#define BT_NAME     "Sherline 5400 Mill"
#define TM_REPORT   40      /* reporting interval in ms */
#define PIN_TACHO   32

BluetoothSerial     dro;    /* Bluetooth serial connection to DRO */

int64_t clip(int64_t n, int64_t lim);

void setup(void) {

    Serial.begin(115200);
    dro.begin(BT_NAME);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    for (int i = 0; i < AXIS_NUM; i++) {
        axis[i].encoder.attachFullQuad(axis[i].pinA, axis[i].pinB);
        axis[i].encoder.clearCount();
    }

    tachoInit(PIN_TACHO, 12);
}

void loop(void) {

    /* Get current count for all axis and update backlash */
    for (int i = 0; i < AXIS_NUM; i++) {
        int64_t cnt = axis[i].encoder.getCount();
        axis[i].blCnt = clip(axis[i].blCnt + (cnt - axis[i].cnt), axis[i].backlash);
        axis[i].cnt = cnt;
    }

    /* Send report to attached DRO */
    if (dro.hasClient()) {
        String report = "";
        char axisReport[32];
        for (int i = 0; i < AXIS_NUM; i++) {
            snprintf(axisReport, sizeof(axisReport), "%c%lld;", axis[i].id, axis[i].cnt - axis[i].blCnt);
            report += String(axisReport);
        }
        snprintf(axisReport, sizeof(axisReport), "t%d;", (int)tachoGetRPM());
        report += String(axisReport);
        dro.println(report);
    }

    delay(TM_REPORT);
}

int64_t clip(int64_t n, int64_t lim) {
    if (n > lim) return lim;
    if (n < -lim) return -lim;
    return n;
}

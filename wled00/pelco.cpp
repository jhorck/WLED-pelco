#include "wled.h"
#include "Syslog.h"

static word pelcoPan = 0, pelcoTilt = 0;
static word lastPan = 0, lastTilt = 0;
static unsigned long lastPelco = 0, pelcoInterval = 300;

#ifdef DEBUG_SYSLOG
    static char buf[MAX_PACKET_SIZE];
    Syslog logger("WLED", "WLED", "192.168.188.146");
#endif
#define DEBUG(x...) DEBUG_PRINTF(x); SYSLOG(x);

void setupPTZ() {
#ifndef WLED_DEBUG
    Serial.begin(9600);
#endif
}

void setPTZ(byte pan, byte panFine, byte tilt, byte tiltFine) {
    word newPan, newTilt;

    newPan  = (pan << 8)  + panFine;
    newTilt = (tilt << 8) + tiltFine;
    //DEBUG("Raw P:%d, T:%d", newPan, newTilt);
    if (newTilt >= PELCO_HALF_SCALE / 2) {
        newTilt = PELCO_HALF_SCALE - newTilt;
        newPan += PELCO_HALF_SCALE;
        //DEBUG("Flip P:%d, T:%d", newPan, newTilt);
    }
    pelcoPan = newPan;
    pelcoTilt = newTilt;
}

void setInterval(byte interval) {
    pelcoInterval = interval;
}

void pelcoSend(byte *p) {
#ifndef WLED_DEBUG
    Serial.write(p,7);
#endif
}

void updatePelco() {
    byte pelco[7];
    word val, panDiv, tiltDiv;
    unsigned long now = millis();

    pelco[0] = PELCO_SYNC;
    pelco[1] = PELCO_ID;
    pelco[2] = 0;

    if (now - lastPelco >= pelcoInterval) {
        panDiv  = ((int16_t)lastPan  > (int16_t)pelcoPan  ? (int16_t)lastPan  - (int16_t)pelcoPan  : (int16_t)pelcoPan  - (int16_t)lastPan);
        tiltDiv = ((int16_t)lastTilt > (int16_t)pelcoTilt ? (int16_t)lastTilt - (int16_t)pelcoTilt : (int16_t)pelcoTilt - (int16_t)lastTilt);
        if (panDiv != 0 && panDiv >= tiltDiv) {
            lastPelco = now;
            lastPan = pelcoPan;
            val = lroundf((float)pelcoPan / (65536.0 / 36000.0));
            pelco[3] = PELCO_CMD_PAN;
            pelco[4] = val >> 8;
            pelco[5] = val & 0xff;
            pelco[6] = PELCO_CHECKSUM;
            pelcoSend(pelco);
            pelcoInterval = lroundf((float)panDiv / (65536.0 / 360.0)) + 180;
            if (pelcoInterval >= 360) pelcoInterval -= 180;
            DEBUG("Pan: %d, %ld\n", val, pelcoInterval);
        } else if (tiltDiv != 0) {
            lastPelco = now;
            lastTilt = pelcoTilt;
            val = lroundf((float)pelcoTilt / (65536.0 / 36000.0));
            pelco[3] = PELCO_CMD_TILT;
            pelco[4] = val >> 8;
            pelco[5] = val & 0xff;
            pelco[6] = PELCO_CHECKSUM;
            pelcoSend(pelco);
            pelcoInterval = lroundf((float)tiltDiv / (65536.0 / 360.0)) + 180;
            if (pelcoInterval >= 360) pelcoInterval -= 180;
            DEBUG("Tilt: %d, %ld\n", val, pelcoInterval);
        }
    }
/*
    int b;
    while (Serial.available() > 0) {
        b = Serial.read();
        DEBUG("Serial: %d", b);
    }
*/
}
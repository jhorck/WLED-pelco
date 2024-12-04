#ifndef PELCO_H
#define PELCO_H

#define PELCO_SYNC 0xff
#define PELCO_ID 1
#define PELCO_CMD_PAN 0x4b
#define PELCO_CMD_TILT 0x4d
#define PELCO_CMD_ZOOM 0x4f
#define PELCO_HALF_SCALE 32768

#define PELCO_CHECKSUM (pelco[1] + pelco[2] + pelco[3] + pelco[4] + pelco[5])

//#define DEBUG_SYSLOG
#ifdef DEBUG_SYSLOG
  #define SYSLOG(x...) snprintf(buf, MAX_PACKET_SIZE, x); logger.debug(buf);
#else
  #define SYSLOG(x...)
#endif

void setupPTZ();
void setPTZ(byte, byte, byte, byte);
void setInterval(byte);
void updatePelco();

#endif
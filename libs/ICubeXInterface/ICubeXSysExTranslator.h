///
///  \file ICubeXSysExTranslator.h
///
///  Master Dictionary of all SysEx firmware commands for the USB Micro-Dig
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///

#ifndef __ICubeXSysExTranslator__
#define __ICubeXSysExTranslator__

static const int kNUM_ICUBEX_SENSORS = 8;

//general
#define ICUBE_SX_SYSEX_ID       0x7D
#define ICUBE_SX_MODE_HOST      0x00
#define ICUBE_SX_MODE_SA        0x01

//programming commands
#define ICUBE_SX_MIDI_RESET     0xFF
#define ICUBE_SX_MUTE_T         0x20
#define ICUBE_SX_MUTE_SET       0x32
#define ICUBE_SX_SET_MUTE       0x32
#define ICUBE_SX_RESET          0x22
#define ICUBE_SX_DUMP_VERS      0x47
#define ICUBE_SX_SET_MODE       0x5A
#define ICUBE_SX_DUMP_MODE      0x5B
#define ICUBE_SX_SET_ID         0x5C
#define ICUBE_SX_MIDI_THRU      0x5D
#define ICUBE_SX_MIDI_RUN_STAT  0x5E

//transmitted messages
#define ICUBE_SX_RESET_ACK      0x23
#define ICUBE_SX_STATUS         0x25
#define ICUBE_SX_VERSION        0x47
#define ICUBE_SX_MODE           0x5B
#define ICUBE_SX_STREAM_DATA    0x00

//length of fixed size messages
#define ICUBE_SX_LEN_MODE           6
#define ICUBE_SX_LEN_VERS           10
#define ICUBE_SX_LEN_INTERVAL       7
#define ICUBE_SX_LEN_STREAM_ECHO    6
#define ICUBE_SX_LEN_RESOLUTION     6
#define ICUBE_SX_LEN_OUTPUT         6
#define ICUBE_SX_LEN_DATA_BASE      5


//standalone mode commands


//host mode commands
#define ICUBE_SX_STREAM         0x01
#define ICUBE_SX_RESOLUTION     0x02
#define ICUBE_SX_INTERVAL       0x03
#define ICUBE_SX_SAMPLE         0x04
#define ICUBE_SX_OUTPUT         0x30
#define ICUBE_SX_SENSOR_CURR_OV 0x6F

//other stuff

#define ICUBE_SX_DATA_INDEX     4;


#include <iostream>
class ICubeXSysExTranslator {
public:
    void createSysEx(char* data, int len);
    
private:
    
};

#endif /* defined(__ICubeXSysExTranslator__) */

///
///  \file ICubeXInteface.cpp
///  
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///
#include <stdio.h>
#include "ICubeXInteface.h"

ICubeXInterface::ICubeXInterface() {
	has_prev_ack_ = false;
    is_debug_ = false;
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseSysEx(std::vector<unsigned char>& data) {
        
    if (data.size() >= 5) { // even the shortest message should be more than 5 bytes long..
        switch (data[3]) {
            case ICUBE_SX_MODE:
                return ParseMode(data);
                break;
            case ICUBE_SX_VERSION:
                return ParseVersion(data);
                break;
            case ICUBE_SX_INTERVAL:
                return ParseIntervalEcho(data);
                return ICUBE_SX_PARSE_STATUS;
            case ICUBE_SX_STATUS:
                return ICUBE_SX_PARSE_STATUS;
                break;
            case ICUBE_SX_RESET_ACK:
                ResetAckReceived();
                return ICUBE_SX_PARSE_STATUS;
            case ICUBE_SX_STREAM:
                return ParseStreamEcho(data);
            case ICUBE_SX_RESOLUTION:
                return ParseResolutionEcho(data);
            case ICUBE_SX_STREAM_DATA:
                return ParseStreamData(data);
            case ICUBE_SX_OUTPUT:
                return ParseActuatorData(data);
            default:
                return ICUBE_SX_PARSE_UNKNOWN; //if we get here, its probably due to message not implemented yet
        }
    }
    else {
        return ICUBE_SX_PARSE_ERR;
    }
}

void ICubeXInterface::SendSetMode(bool isStandAlone) {

    SysExInit();
    my_sysex_data_.push_back(ICUBE_SX_SET_MODE);
    if (isStandAlone)
        my_sysex_data_.push_back(ICUBE_SX_MODE_SA);
    else
        my_sysex_data_.push_back(ICUBE_SX_MODE_HOST);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendDumpMode() {
    SysExInit();
    SysExAdd(ICUBE_SX_DUMP_MODE);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendDumpVersion() {
    SysExInit();
    SysExAdd(ICUBE_SX_DUMP_VERS);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendSampleInterval(int time_ms) {
    if (is_debug_) printf("set sample interval: %i ms\n", time_ms);
    SysExInit();
    SysExAdd(ICUBE_SX_INTERVAL);
    int x = time_ms / 128;
    int y = time_ms % 128;
    SysExAdd((unsigned char) x);
    SysExAdd((unsigned char) y);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendMute(bool is_muted) {
    if (is_debug_) printf("set mute %i \n", is_muted);
    SysExInit();
    SysExAdd(ICUBE_SX_MUTE_SET);
    if (is_muted)
        SysExAdd(1);
    else
        SysExAdd(0);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendToggleMute() {
    if (is_debug_) printf("toggle mute\n");
    SysExInit();
    SysExAdd(ICUBE_SX_MUTE_T);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendReset() {
    if (is_debug_) printf("send reset\n");
    SysExInit();
    SysExAdd(ICUBE_SX_RESET);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendStream(bool is_on, int port) {
    if (is_debug_) printf("send stream for port %i, %i\n", port, is_on);
    SysExInit();
    SysExAdd(ICUBE_SX_STREAM);
    char cmd = (int)is_on * 0x40 + port;
    SysExAdd(cmd);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendSetHires(bool is_hi, int port) {
    SysExInit();
    SysExAdd(ICUBE_SX_RESOLUTION);
    char cmd = (int)is_hi * 0x40 + port;
    SysExAdd(cmd);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SendSetOutput(bool is_on, int port) {
    SysExInit();
    SysExAdd(ICUBE_SX_OUTPUT);
    char cmd = (int)is_on * 0x40 + port;
    SysExAdd(cmd);
    SysExEnd();
    sendSysExCmd();
}

void ICubeXInterface::SetDebug(bool debug) {
    is_debug_ = debug;
    my_digitizer_state_.SetDebug(debug);
}


//PRIVATE internal helper functions:

void ICubeXInterface::SysExInit() {
    my_sysex_data_.clear();
    my_sysex_data_.push_back(0xF0);
    my_sysex_data_.push_back(ICUBE_SX_SYSEX_ID);
    my_sysex_data_.push_back(my_digitizer_state_.GetDigitizerIDState());
}

void ICubeXInterface::SysExAdd(unsigned char new_byte) {
    my_sysex_data_.push_back(new_byte);
}

void ICubeXInterface::SysExEnd() {
    //TODO: consider combine with send command? do we ever end msg and not send??
    my_sysex_data_.push_back(0xF7);
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseMode(std::vector<unsigned char>& data) {
    if (data.size() != ICUBE_SX_LEN_MODE) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        if (is_debug_) printf("Parser: mode set to %i\n", data[4]);
        my_digitizer_state_.SetModeState(data[4]);
        has_prev_ack_ = false; //this needs to be cleared for reset ack counting
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseVersion(std::vector<unsigned char> &data) {
    if (data.size() != ICUBE_SX_LEN_VERS) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        int x = data.at(4);
        int a = data.at(5);
        int b = data.at(6);
        int c = data.at(7);
        int d = data.at(8);
        
        float firmware = (float)x/10.0;
        float hardware = (float)a/10.0+(float)b/1000.0;
        int serial = c*100+d;
        //TODO: change to logging
        if (is_debug_) printf("Parser: Firmwave Version = %1.1f Hardware Version = %2.2f Serial = %i", firmware, hardware, serial);
        my_digitizer_state_.SetVersionState(firmware, hardware, serial);
        
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseIntervalEcho(std::vector<unsigned char> &data) {

    if (data.size() != ICUBE_SX_LEN_INTERVAL) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        int x = data.at(4);
        int y = data.at(5);
        int sample_interval_ms = x*128+y;
        //TODO: change to logging
        if (is_debug_) printf("Parser: sample interval: %i\n", sample_interval_ms);
        my_digitizer_state_.SetSampleIntervalState(sample_interval_ms);
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseStreamEcho(std::vector<unsigned char> &data) {
    if (data.size() != ICUBE_SX_LEN_STREAM_ECHO) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        int x = 0x40 & data.at(4);
        int y = 0x07 & data.at(4);
        if (is_debug_) printf("Parser: sensor %i is turned %i\n", y, x);
        my_digitizer_state_.SetSensorStreamState(y, (bool)x);
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseStreamData(std::vector<unsigned char> &data) {
    if (data.size() != my_digitizer_state_.GetExpectedDataMsgLen()) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        if (is_debug_) printf("Parser: valid data length!\n");
        //data starts at [4]
        int curr_index = ICUBE_SX_DATA_INDEX;
        for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
            //get value of sensor, if not on, set to -1.
            int value = -1;
            if (my_digitizer_state_.GetIsSensorOnState(i)) {
                if (my_digitizer_state_.IsSensorHiResState(i)) {
                    int y = data.at(curr_index);
                    int z = data.at(curr_index+1);
                    value = y*8 + (z>>2);
                    curr_index+=2; //if we had a hires sensor, must move index up by one more
                }
                else {
                    int y = data.at(curr_index);
                    value = y;
                    curr_index+=1;
                }
            }
            my_digitizer_state_.SetSensorDataState(i, value);
        }
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseResolutionEcho(std::vector<unsigned char> &data) {
    if (data.size() != ICUBE_SX_LEN_RESOLUTION) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        int x = 0x40 & data.at(4);
        int y = 0x07 & data.at(4);
        my_digitizer_state_.SetSensorResolution(y, (bool)x);
    }
}

ICUBE_SX_PARSE_RESULT ICubeXInterface::ParseActuatorData(std::vector<unsigned char> &data) {
    if (data.size() != ICUBE_SX_LEN_OUTPUT) {
        return ICUBE_SX_PARSE_ERR_LEN;
    }
    else {
        int x = 0x40 & data.at(4);
        int y = 0x07 & data.at(4);
        my_digitizer_state_.SetActuatorOutState(y, (bool)x);
    }
}

void ICubeXInterface::ResetAckReceived() {
    ftime(&curr_time_);
    printf("Parser: ResetAckRecv\n");
    if (has_prev_ack_ == false) {
        printf("Parser: 1st ack; assume host mode for now...\n");
        my_digitizer_state_.SetModeState(ICUBE_SX_MODE_HOST);
        has_prev_ack_ = true;
    }
    else {
        //if a previous one was sent within 1 second,
        // -->we're in Stand Alone mode!
        int diff = 1000* (curr_time_.time-prev_time_.time) + (curr_time_.millitm - prev_time_.millitm);
        printf("millis since last ack = %i\n", diff);
        if (diff <= 1200) { //needs a bit longer to get in it seems
            
            printf("Parser: second ack in 1 sec, standalone mode!\n");
            my_digitizer_state_.SetModeState(ICUBE_SX_MODE_SA);
        }
        else {
            //in host mode; arm for next cycle
            printf("Parser: second ack > 1 sec, host mode!\n");
            my_digitizer_state_.SetModeState(ICUBE_SX_MODE_HOST);
        }
        has_prev_ack_ = false;
    }
    //reset the output state
    my_digitizer_state_.SetOutputsState(true);
    if (my_digitizer_state_.GetModeState() == ICUBE_SX_MODE_HOST) {
        // in host mode, after a reset all inputs are off and the
        // resolution is set to low
        // this is important as the sensor data parser relies
        // on the internal state to determine content of raw data stream
        my_digitizer_state_.SetInputsOffState();
        my_digitizer_state_.SetLowResState();
    }
    prev_time_ = curr_time_;
    
}

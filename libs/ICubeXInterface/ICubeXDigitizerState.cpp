///
///  \file ICubeXDigitizerState.cpp
///  
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///
/// 

#include "ICubeXDigitizerState.h"

ICubeXDigitizerState::ICubeXDigitizerState() {
    digitizer_id_ = 0;
    digitizer_mode_ = 0;
    
    version_firmware_ = 0;
    version_hardware_ = 0;
    version_serial_ = 0;
    
    is_muted_ = false;
}

void ICubeXDigitizerState::SetOutputsState(bool is_on) {
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        actuator_out_on_[i] = is_on;
    }
}

void ICubeXDigitizerState::SetInputsOffState() {
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        sensor_is_on_[i] = false;
    }
}

void ICubeXDigitizerState::SetLowResState() {
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        sensor_is_hires_[i] = false;
    }
}

void ICubeXDigitizerState::SetVersionState(float firmware, float hardware, float serial) {
    version_firmware_ = firmware;
    version_hardware_ = hardware;
    version_serial_ = serial;
}

void ICubeXDigitizerState::SetSampleIntervalState(int interval_ms) {
    sampling_interval_ = interval_ms;
}

void ICubeXDigitizerState::SetModeState(int mode) {
    if (is_debug_) printf("State: set mode to %i\n",mode);
    digitizer_mode_ = mode;
}

void ICubeXDigitizerState::SetSensorStreamState(int port, bool is_on) {
    if (is_debug_) printf("State: port %i is on: %i\n", port, is_on);
    sensor_is_on_[port] = is_on;
}

void ICubeXDigitizerState::SetSensorResolution(int port, bool is_hires) {
    if (is_debug_) printf("State: port %i is hires: %i\n", port, is_hires);
    sensor_is_hires_[port] = is_hires;
}

void ICubeXDigitizerState::SetSensorDataState(int port, int value) {
    if (is_debug_) printf("State: port %i set to %i\n", port, value);
    sensor_values_[port] = value;
}

void ICubeXDigitizerState::SetActuatorOutState(int port, bool is_on) {
    if (is_debug_) printf("State: actuator %i set to %i\n", port, is_on);
    actuator_out_on_[port] = is_on;
}

int ICubeXDigitizerState::GetSensorValState(int sensorIndex) {
    
    // only return value if index is within range and sensor is on
    if ( (sensorIndex <0) || (sensorIndex >= kNUM_ICUBEX_SENSORS) ) {
        return -1;
    }
    else {
        //if (!sensor_is_on_[sensorIndex]) {
        //    return -1;
        //}
        //else {
            return (sensor_values_[sensorIndex]);
        //}
    }
}

int ICubeXDigitizerState::GetExpectedDataMsgLen() {
    int len = ICUBE_SX_LEN_DATA_BASE;
    for (int i=0; i<kNUM_ICUBEX_SENSORS; i++) {
        if (sensor_is_on_[i]) {
            //either 1 byte, or two if hi-res
            len+=1 + (int) sensor_is_hires_[i];
        }
    }
    return len;
}

bool ICubeXDigitizerState::IsSensorHiResState(int sensorIndex) {
    if ( (sensorIndex <0) || (sensorIndex >= kNUM_ICUBEX_SENSORS) ) {
        return false;
    }
    return sensor_is_hires_[sensorIndex];
}

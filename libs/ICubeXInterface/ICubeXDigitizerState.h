///
///  \file ICubeXDigitizerState.h
///
///  A class for holding the state of the digitizer. Allows
///  software interface to keep track of the state of the digitizer.
///
///  It is up to the ICubeXInterface class to parse data coming from
///  a phyiscal digitizer and update this structure.
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///

#ifndef __ICubeXDigitizerState__
#define __ICubeXDigitizerState__

#include <iostream>
#include "ICubeXSysExTranslator.h"

class ICubeXDigitizerState {
public:
    
    ICubeXDigitizerState();
    
    /// Returns the sensor value at particular index in the internal state
    /// \param sensorIndex the index of sensor
    /// \return value of sensor; -1 if index out of range or sensor is off
    int GetSensorValState(int sensorIndex);

    
    /// Returns whether the internal state of the sensor at index is hi res or not
    /// \param sensorIndex the index of sensor
    /// \return true if hi res; false if sensor is low res or index out of range
    bool IsSensorHiResState(int sensorIndex);
    
    /// Retreives the ID of the digitizer stored in internal state
    /// \return the ID of the current digitizer
    int GetDigitizerIDState() {return digitizer_id_;}

    /// Sets the operating mode internal state of the digitizer: standa alone or host
    void SetModeState(int mode);
    
    /// Gets the operating mode internal state
    int GetModeState() {return digitizer_mode_;}
    
    /// Sets the version info internal state
    void SetVersionState(float firmware, float hardware, float serial);
    
    /// Sets the sampling interval in internal state
    void SetSampleIntervalState(int interval_ms);
    
    /// Sets the internal port streaming state
    void SetSensorStreamState(int port, bool is_on);

    /// Sets the internal port resolution state
    void SetSensorResolution(int port, bool is_hires);
    
    /// Resets the outputs in internal state
    /// default is true
    void SetOutputsState(bool is_on = true);
    
    /// Sets all inputs to off in internal state
    void SetInputsOffState();
    
    /// Sets all outputs to lowres in internal state
    void SetLowResState();
    
    /// Sets sensor data value in internal state
    void SetSensorDataState(int port, int value);
    
    /// Sets actuator out value in internal state
    void SetActuatorOutState(int port, bool is_on);
    
    void SetDebug(bool debug) {is_debug_ = debug;}
    
    int GetExpectedDataMsgLen();
    
    bool GetIsSensorOnState(int port) {return sensor_is_on_[port]; }
    
    bool GetIsActuatorOnState(int port) {return actuator_out_on_[port];}
    
    float GetVersionFWState() {return version_firmware_;}
    
    float GetVersionHWState() {return version_hardware_;}
    
    int GetVersionSerialState() {return version_serial_;}
    
    bool getDebug() {return is_debug_;}
    
    
private:
    int sensor_values_[kNUM_ICUBEX_SENSORS];
    bool sensor_is_on_[kNUM_ICUBEX_SENSORS];
    bool sensor_is_hires_[kNUM_ICUBEX_SENSORS];
    bool actuator_out_on_[kNUM_ICUBEX_SENSORS];
    
    int digitizer_id_;
    
    int digitizer_mode_;
    
    int sampling_interval_;
    
    float version_firmware_;
    float version_hardware_;
    int version_serial_;
    
    bool is_muted_;
    bool is_debug_;
    
};

#endif /* defined(__ICubeXDigitizerState__) */

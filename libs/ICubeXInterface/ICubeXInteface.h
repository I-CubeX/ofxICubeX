///
///  \file ICubeXInteface.h
///
///  The header file for the I-CubeX Digitizer Interface Class.
///  The current version is primarily for HOST MODE interaction. Standalone
///  messages can easily be parsed at the application level.
///
///  The interface object performs the following:
///
///  INPUT:
///  - Takes in MIDI data (SysEx) from implementation class
///  - Translates from the raw SysEx data to an internal representation of
///  the digitzer (ICubeXDigitizerState class) containing operating mode, sensor configuration, and sensor data
///
///  OUTPUT:
///  - Exposes operational commands via function calls, and
///  translates them into appropriate SysEx messages
///     (e.g. toggleMute() and its SysEx equivalent: 0xF0 0x7D 0x00 0x20 0xF7)
///  - The packed SysEx message is stored in an internal buffer, and expected to be sent using
///  the overridden sendSysExCmd in the implementation class (that handles the MIDI in/out ports, etc).
///
///  see implementations in the ofxICubeX example, or JUCE example (tbd).
///
///
///  Created by Johnty Wang on 2014-09-16. johnty@infusionsystems.com
///
///
///  ** complete list of implemeted ([x]) firmware commands **
///  ** based on FIRMWARE VERSION 7.0 USB MICRODIG **
///  (General data format: 0xF0 0x7D {dev} {cmd} [body] 0xF7)
/// **** send (to digitizer)
///  - [ ] MIDI system reset:       0xFF
///  - [x] MUTE (toggle):           0x20
///  - [x] SET MUTE:                0x32 [0 = unmute, 1 = mute]
///  - [x] RESET:                   0x22
///  - [x] DUMP VERSION:            0x47
///  - [x] SET MODE:                0x5A [0 = host 1 = stand alone]
///  - [x] DUMP MODE:               0x5B
///  - [ ] SET ID:                  0x5C [0..127 for new ID]
///  - [ ] MIDI THRU:               0x5D [0 = off, >0 = on]
///  - [ ] MIDI RUNNING STATUS:     0x5E [0 = off, >0 = on]
///  - [ ]
/// **** recv (from digitizer)
///  - [x] RESET ACK:               0x23
///  - [ ] STATUS:                  0x25 [0x5A = conf error
///                                         0x5C = MIDI protocol error/invalid cmd
///                                         0x5E = MIDI byte scrambled
///                                         0x5F = recv buffer full
///  - [x] VERSION                  0x47 [ x = firmware v * 10
///                                         a = hardware board v * 10
///                                         b = hardware board .v * 1000
///                                         c = 1st serial number digits
///                                         d = last serial number digits
///  - [x] MODE                     0x5B [0 = host, 1 = standalone]
///  **** TODO: standalone mode commands here ****
///  ****
///  ****
///  **** hostmode commands *****
///  - [x] STREAM                   0x01 [0x000yyy   x=1->on; x=0->off; yyy=sensor number]
///  - [x] RESOLUTION               0x02 [0x000yyy   x=1->hi res; x=0->low res; yyy = sens number]
///  - [x*] INTERVAL                 0x03 [0xxxxxxx 0yyyyyyy] interval = x*128 + y
///  - [ ] SAMPLE                   0x04 [00000yyy] yyy = sensor number (stream must be off)
///  - [X] OUTPUT                   0x30 [0x000yyy] yyy = output number, x = on/off
///  - [ ] SENSOR CURRENT OVERLOAD  0x6F
///  - [ ] 12C features...
///
///   - [*] == need to test

#ifndef __ICubeXInteface__
#define __ICubeXInteface__

#include <iostream>
#include <vector>
#include <sys/timeb.h>
#include <time.h>
#include "ICubeXSysExTranslator.h"
#include "ICubeXDigitizerState.h"

enum ICUBE_SX_PARSE_RESULT {
    ICUBE_SX_PARSE_STATUS,
    ICUBE_SX_PARSE_DATA,
    ICUBE_SX_PARSE_UNKNOWN,
    ICUBE_SX_PARSE_ERR_LEN,
    ICUBE_SX_PARSE_ERR,
};

class ICubeXInterface {
    
public:
    
    ICubeXInterface();

    ICubeXDigitizerState my_digitizer_state_;
    
    //main parser
    
    /// The main parser for incoming SysEx messages
    ///    simply forwards it to specific parser functions depending on the CMD tag.
    /// \param data the raw MIDI data containing sysex message
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseSysEx(std::vector<unsigned char>& data);
    

    /// Send command for setting operational mode of digitizer
    void SendSetMode(bool isStandAlone);
    /// Send command for setting sampling interval in milliseconds
    void SendSampleInterval(int time_ms);
    
    /// Sends a DUMP command to get mode from digitizer
    void SendDumpMode();
    
    /// Sends a DUMP version command to get version info from digitizer
    void SendDumpVersion();
    
    /// Sends a set MUTE command to mute/unmute all sensors
    void SendMute(bool is_muted);
    /// Sends a MUTE TOGGLE for all sensors
    void SendToggleMute();
    
    /// Sends reset message to digitizer
    void SendReset();
    
    /// Sends STREAM command to turn on/off a port
    /// \param is_on whether port should be on or off
    /// \param port the port index (0-7)
    void SendStream(bool is_on, int port);

    /// Sends RES command to change resolution of port
    /// \param is_hi whether port should be hi resolution or not
    /// \param port the port index (0-7)
    void SendSetHires(bool is_hi, int port);
    
    
    /// Sends a set output cmd to turn on/off an output port
    /// \param is_on turns output on or off
    /// \param port the port index (0-7)
    void SendSetOutput(bool is_on, int port);
    
    
    /// The virtual method that is meant to be overloaded by the implementation class.
    /// The SysEx data can be created using the internal ICubeXSysExTranslator class,
    /// and then sent to the MIDI port opened by the implementation class in the host application.
    /// \param data the packed SysEx data to be sent to the MIDI port
    virtual void sendSysExCmd() = 0;
    
    /// Set Debug flag for printf output
    /// \param debug true = ON, false = OFF
    void SetDebug(bool debug);
    
    
    /// Returns the current SysEx buffer (containing formatted command) for sending to the
    /// digitizer
    const std::vector<unsigned char>& getSysExBuffer() const {return my_sysex_data_;}

private:

    /// Helper method for initializing (clearing) internal SysEx buffer and adding headers
    /// (including the Dev ID ofcurrent interface)
    void SysExInit();
    
    /// Helper method for adding a single byte into SysEx buffer
    void SysExAdd(unsigned char new_byte);
    
    /// Helper method for appending ending byte to internal SysEx buffer
    void SysExEnd();
    
    //specific parsers

    /// Parses the mode of operation of digitizer, stores result in
    /// local digitizer state
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseMode(std::vector<unsigned char>& data);

    /// Parses the version information of operation of digitizer, stores result in
    /// local digitizer state
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseVersion(std::vector<unsigned char>& data);
    
    /// Parses the sampling interval echoed by digitizer, stores result in
    /// local digitizer state
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseIntervalEcho(std::vector<unsigned char>& data);
    
    /// Parses the stream command that is echoed by the digitizer
    /// updates local digitizer state with new streaming status
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseStreamEcho(std::vector<unsigned char>& data);
    
    /// Parses the resolution command that is echoed by the digitizer
    /// updates digitizer state with new resolution status
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseResolutionEcho(std::vector<unsigned char>& data);
    
    /// Parses the stream data message sent by digitizer
    /// fills sensor data structure in internal state
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseStreamData(std::vector<unsigned char>& data);
    
    /// Parses the actuator output cmd echoed by digitizer
    /// fills digitizer state
    /// \param data incoming SysEx data
    /// \return status for error checking
    ICUBE_SX_PARSE_RESULT ParseActuatorData(std::vector<unsigned char>& data);
    
    /// Counter for detecting whether we're in Host or Standalone mode
    /// based on on number and timing of reset acks received
    void ResetAckReceived();
    
    std::vector<unsigned char> my_sysex_data_;
    
    timeb curr_time_;
    timeb prev_time_;
    bool has_prev_ack_;
    
    bool is_debug_;
    

};

#endif /* defined(__ICubeXInteface__) */

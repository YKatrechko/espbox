#define EEPROMVER 12      // for eeprom layout verification

struct __flags {
  uint8_t ident;
  uint8_t box;
  uint8_t reset;
  uint8_t signaltype;
}
flags;



  
//////////////////////////////////////////////////////////////////////////////
// MSP_STATUS               101   //out message         cycletime & errors_count & sensor present & box activation & current setting number
typedef  struct {
  uint16_t cycleTime, i2c_errors_count, sensor;
} status_t;

// MSP_RAW_IMU              102   //out message         9 DOF
typedef struct {
  int16_t  accSmooth[3];
  int16_t  gyroData[3];
  int16_t  magADC[3];
  int16_t  gyroADC[3];
  int16_t  accADC[3];
} imu_t;

// MSP_RAW_GPS              106   //out message         fix, numsat, lat, lon, alt, speed, ground course
typedef  struct {
  uint8_t fix, num;
  int32_t lat, lon;
  int16_t alt;
  uint16_t speed, gcourse;
} raw_gps_t;

// MSP_COMP_GPS             107   //out message         distance home, direction home
typedef  struct {
  uint16_t distoHome;
  int16_t dirtoHome;
  uint8_t upd;
} comp_gps_t;

// MSP_ALTITUDE             109   //out message         altitude, variometer
typedef struct {
  int32_t  EstAlt;             // in cm
  int16_t  vario;              // variometer in cm/s
} alt_t;

// MSP_ATTITUDE             108   //out message         2 angles 1 heading
typedef struct {
  int16_t angle[2];            // absolute angle inclination in multiple of 0.1 degree    180 deg = 1800
  int16_t heading;             // variometer in cm/s
} att_t;

// MSP_ANALOG               110   //out message         vbat, powermetersum, rssi if available on RX
typedef struct {
  uint8_t  vbat;               // battery voltage in 0.1V steps
  uint16_t intPowerMeterSum;
  uint16_t rssi;              // range: [0;1023]
} analog_t;

// MSP_DEBUG                254   //out message         debug1,debug2,debug3,debug4
typedef struct {
  int16_t  debug[4];
} debug_t;

// MSP_TIME_GPS             99   //out message         gps_time
typedef struct {
  uint32_t  date;
  uint32_t  time;
} time_gps_t;


struct {
  status_t status;              // 101
  // imu_t imu;                 // 102
  msp_raw_gps_t msp_raw_gps;    // 106
  msp_comp_gps_t msp_comp_gps;  // 107
  att_t att;                    // 108
  alt_t alt;                    // 109
  analog_t analog;              // 110
  debug_t debug;                // 254
} msp_data;


#define REQ_MSP_STATUS    (1 <<  0)
#define REQ_MSP_RAW_IMU   (1 <<  1)
#define REQ_MSP_ALTITUDE  (1 <<  2)
#define REQ_MSP_ATTITUDE  (1 <<  3)
#define REQ_MSP_COMP_GPS  (1 <<  4)
#define REQ_MSP_RAW_GPS   (1 <<  5)
#define REQ_MSP_ANALOG    (1 <<  6)
#define REQ_MSP_DEBUG     (1 <<  7)
#define REQ_MSP_TIME_GPS  (1 <<  8)

// Settings Locations
enum Setting_ {
  S_CHECK_,    // used for check
  S_ESPTIME,
  S_GPSTIME,
  S_CYCLETIME,
  S_I2C_ERROR,
  S_RAW_IMU,
  S_GPSFIX,
  S_GPSNUM,
  S_GPSCOORD,
  S_GPSALT,
  S_GPSSPEED,
  S_GPSDIST_HOME,
  S_GPSDIR_HOME,
  S_GPSDIR_UPD,
  S_ATT_ANGLE,
  S_ATT_HEAD,
  S_VBAT,
  S_POWER,
  S_RSSI,
  S_AMPERAGE,
  S_DEBUG0,
  S_DEBUG1,
  S_DEBUG2,
  S_DEBUG3,
  S_CS0,
  S_CS1,
  S_CS2,
  S_CS3,
  // EEPROM_SETTINGS must be last!
  EEPROM_SETTINGS
};


// For Settings Defaults
PROGMEM const uint8_t EEPROM_DEFAULT[EEPROM_SETTINGS] = {
  EEPROMVER,   // used for check              0
  1, //  S_ESPTIME,
  1, //  S_GPSTIME,
  0, //  S_CYCLETIME,
  1, //  S_I2C_ERROR,
  0, //  S_RAW_IMU,
  0, //  S_GPSFIX,
  1, //  S_GPSNUM,
  1, //  S_GPSCOORD,
  0, //  S_GPSALT,
  1, //  S_GPSSPEED,
  1, //  S_GPSDIST_HOME,
  1, //  S_GPSDIR_HOME,
  1, //  S_GPSDIR_UPD,
  1, //  S_ATT_ANGLE,
  1, //  S_ATT_HEAD,
  1, //  S_VBAT,
  0, //  S_POWER,
  0, //  S_RSSI,
  0, //  S_AMPERAGE,
  1, //  S_DEBUG0,
  0, //  S_DEBUG1,
  0, //  S_DEBUG2,
  0, //  S_DEBUG3,
  0, //  S_CS0,
  0, //  S_CS1,
  0, //  S_CS2,
  0, //  S_CS3,
};

uint8_t  Settings[EEPROM_SETTINGS];


// ---------------------------------------------------------------------------------------
// Defines imported from Multiwii Serial Protocol MultiWii_shared svn r1337
#define MSP_VERSION              0

//to multiwii developpers/committers : do not add new MSP messages without a proper argumentation/agreement on the forum
#define MSP_IDENT                100   //out message         multitype + multiwii version + protocol version + capability variable
#define MSP_STATUS               101   //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RAW_IMU              102   //out message         9 DOF
#define MSP_RAW_GPS              106   //out message         fix, numsat, lat, lon, alt, speed, ground course
#define MSP_COMP_GPS             107   //out message         distance home, direction home
#define MSP_ATTITUDE             108   //out message         2 angles 1 heading
#define MSP_ALTITUDE             109   //out message         altitude, variometer
#define MSP_ANALOG               110   //out message         vbat, powermetersum, rssi if available on RX

#define MSP_DEBUG                254   //out message         debug1,debug2,debug3,debug4

// End of imported defines from Multiwii Serial Protocol MultiWii_shared svn r1333
// ---------------------------------------------------------------------------------------


uint16_t  MwSensorPresent = 0;
uint32_t  MwSensorActive = 0;
#define hi_speed_cycle  500  // updates everything approx 1.3 times per second




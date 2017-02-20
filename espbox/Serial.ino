 

  #define SERIALBUFFERSIZE 150

static uint8_t serialBuffer[SERIALBUFFERSIZE]; // this hold the imcoming string from serial O string
static uint8_t receiverIndex;
static uint8_t dataSize;
static uint8_t cmdMSP;
static uint8_t rcvChecksum;
static uint8_t readIndex;
static uint8_t txChecksum;


uint32_t read32() {
  uint32_t t = read16();
  t |= (uint32_t)read16()<<16;
  return t;
}

uint16_t read16() {
  uint16_t t = read8();
  t |= (uint16_t)read8()<<8;
  return t;
}

uint8_t read8()  {
  return serialBuffer[readIndex++];
}

#define skip8() {readIndex++;}
#define skip16() {readIndex+=2;}
#define skip32() {readIndex+=4;}
#define skipn(n) {readIndex+=n;}

//
// Legacy mspWrite*()
//

void mspWriteRequest(uint8_t mspCommand, uint8_t txDataSize){
  //return;
  Serial.write('$');
  Serial.write('M');
  Serial.write('<');
  txChecksum = 0;
  mspWrite8(txDataSize);
  mspWrite8(mspCommand);
  if(txDataSize == 0)
    mspWriteChecksum();
}

void mspWrite8(uint8_t t){
  Serial.write(t);
  txChecksum ^= t;
}

void mspWrite16(uint16_t t){
  mspWrite8(t);
  mspWrite8(t>>8);
}

void mspWrite32(uint32_t t){
  mspWrite8(t);
  mspWrite8(t>>8);
  mspWrite8(t>>16);
  mspWrite8(t>>24);
}

void mspWriteChecksum(){
  Serial.write(txChecksum);
}

// Writes to GUI (OSD_xxx) is distinguished from writes to FC (MSP_xxx) by
// cfgWrite*() and mspWrite*().
//
// If I2C is not used, then all cfgWrite*() will be mspWrite*().

# define cfgWriteRequest mspWriteRequest
# define cfgWrite8 mspWrite8
# define cfgWrite16 mspWrite16
# define cfgWrite32 mspWrite32
# define cfgWriteChecksum mspWriteChecksum


// --------------------------------------------------------------------------------------
// Here are decoded received commands from MultiWii
void serialMSPCheck()
{
//  #ifdef DEBUGDPOSPACKET
//    timer.packetcount++;
//  #endif
//  readIndex = 0;
//
//  if (cmdMSP==MSP_IDENT)
//  {
//    flags.ident=1;
//    MwVersion= read8();                             // MultiWii Firmware version
//  }
//
//  if (cmdMSP==MSP_STATUS)
//  {
//    cycleTime=read16();
//    I2CError=read16();
//    MwSensorPresent = read16();
//    MwSensorActive = read32();
//    #if defined FORCESENSORS
//      MwSensorPresent=GPSSENSOR|BAROMETER|MAGNETOMETER|ACCELEROMETER;
//    #endif  
//    armed = (MwSensorActive & mode.armed) != 0;
//    FCProfile = read8();
//    if (!configMode){
//      CurrentFCProfile=FCProfile;
//      PreviousFCProfile=FCProfile;
//     }
//  }
//
//  if (cmdMSP==MSP_RC)
//  {
//    for(uint8_t i=1;i<=TX_CHANNELS;i++)
//      MwRcData[i] = read16();
//    handleRawRC();
//  }
//
//  if (cmdMSP==MSP_RAW_GPS)
//  {
//    uint8_t GPS_fix_temp=read8();
//    if (GPS_fix_temp){
//      GPS_fix=1;
//    }
//    GPS_numSat=read8();
//    GPS_latitude = read32();
//    GPS_longitude = read32();
//    GPS_altitude = read16();
//    #if defined RESETGPSALTITUDEATARM
//      if (!armed){
//        GPS_home_altitude=GPS_altitude;
//      } 
//      GPS_altitude=GPS_altitude-GPS_home_altitude;
//    #endif // RESETGPSALTITUDEATARM  
//    #if defined I2CGPS_SPEED
//      GPS_speed = read16()*10;
//      //gpsfix(); untested
//    #else
//      GPS_speed = read16();
//    #endif // I2CGPS_SPEED
//    GPS_ground_course = read16();
//  }
//
//  if (cmdMSP==MSP_COMP_GPS)
//  {
//    GPS_distanceToHome=read16();
//
//    
//    GPS_directionToHome=read16();
//#ifdef GPSTIME
//    read8(); //missing
//    GPS_time = read32();        //local time of coord calc - haydent
//#endif
//  }
//
//  if (cmdMSP==MSP_NAV_STATUS)
//  {
//     read8();
//     read8();
//     read8();
//     GPS_waypoint_step=read8();
//  }
//
//  if (cmdMSP==MSP_ATTITUDE)
//  {
//    for(uint8_t i=0;i<2;i++){
//      MwAngle[i] = read16();
//    }
//      MwHeading = read16();
//    #if defined(USEGPSHEADING)
//      MwHeading = GPS_ground_course/10;
//    #endif
//    #ifdef HEADINGCORRECT
//      if (MwHeading >= 180) MwHeading -= 360;
//    #endif
//  }
//
//#if defined DEBUGMW
//  if (cmdMSP==MSP_DEBUG)
//  {
//    for(uint8_t i=0;i<4;i++)
//      debug[i] = read16();
// }
//#endif
//  if (cmdMSP==MSP_ALTITUDE)
//  {
//    #if defined(USEGPSALTITUDE)
//      MwAltitude = (int32_t)GPS_altitude*100;
//      gpsvario();
//    #else    
//      MwAltitude =read32();
//      MwVario = read16();
//    #endif
//  }
//
//  if (cmdMSP==MSP_ANALOG)
//  {
//    MwVBat=read8();
//    pMeterSum=read16();
//    MwRssi = read16();
//    MWAmperage = (int16_t)read16();
// }
}
// End of decoded received commands from MultiWii
// --------------------------------------------------------------------------------------


void serialMSPreceive(uint8_t loops)
{
  uint8_t c;
  uint8_t loopserial=0;

  static enum _serial_state {
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD,
  }
  c_state = IDLE;

  if (Serial.available()) loopserial=1;

  while(loopserial==1)
  {

    c = Serial.read();

    if (c_state == IDLE)
    {
      c_state = (c=='$') ? HEADER_START : IDLE;
    }
    else if (c_state == HEADER_START)
    {
      c_state = (c=='M') ? HEADER_M : IDLE;
    }
    else if (c_state == HEADER_M)
    {
      c_state = (c=='>') ? HEADER_ARROW : IDLE;
    }
    else if (c_state == HEADER_ARROW)
    {
      if (c > SERIALBUFFERSIZE)
      {  // now we are expecting the payload size
        c_state = IDLE;
      }
      else
      {
        dataSize = c;
        c_state = HEADER_SIZE;
        rcvChecksum = c;
      }
    }
    else if (c_state == HEADER_SIZE)
    {
      c_state = HEADER_CMD;
      cmdMSP = c;
      rcvChecksum ^= c;
      receiverIndex=0;
    }
    else if (c_state == HEADER_CMD)
    {
      rcvChecksum ^= c;
      if(receiverIndex == dataSize) // received checksum byte
      {
        if(rcvChecksum == 0) {
            serialMSPCheck();
        }
        c_state = IDLE;
      }
      else
        serialBuffer[receiverIndex++]=c;
    }
    if (loops==0) loopserial=0;
    if (!Serial.available()) loopserial=0;
  }
}




#ifdef MSP_LOG_FEATURE

//------------------------------------------------------------------------
void proccess_msp()
{
  //  alarms.active = 0;
  //  timer.loopcount++;
  //  if (flags.reset) {
  //    resetFunc();
  //  }

  //  if (screenlayout!=oldscreenlayout){
  //    readEEPROM_screenlayout();
  //  }
  //  oldscreenlayout=screenlayout;
  //
  //  // Blink Basic Sanity Test Led at 0.5hz
  //  if(timer.tenthSec>5)
  //    digitalWrite(LEDPIN,HIGH);
  //  else
  //    digitalWrite(LEDPIN,LOW);

  //---------------  Start Timed Service Routines  ---------------------------------------
  unsigned long currentMillis = millis();
  //  if ((currentMillis - previous_millis_low) >= lo_speed_cycle) // 10 Hz (Executed every 100ms)
  //  {
  //    previous_millis_low = previous_millis_low + lo_speed_cycle;
  //    timer.tenthSec++;
  //    timer.halfSec++;
  //    timer.Blink10hz = !timer.Blink10hz;
  //    calculateTrip();
  //    if (Settings[S_AMPER_HOUR])
  //      amperagesum += amperage;
  //  }  // End of slow Timed Service Routine (100ms loop)

  if ((currentMillis - previous_millis_high) >= hi_speed_cycle) // 20 Hz or 100hz in MSP high mode
  {
    previous_millis_high = previous_millis_high + hi_speed_cycle;
    uint8_t MSPcmdsend = 0;
    if (queuedMSPRequests == 0)
      queuedMSPRequests = modeMSPRequests;
    uint32_t req = queuedMSPRequests & -queuedMSPRequests;
    queuedMSPRequests &= ~req;
    switch (req) {
      case REQ_MSP_STATUS:
        MSPcmdsend = MSP_STATUS;
        break;
      case REQ_MSP_RAW_GPS:
        MSPcmdsend = MSP_RAW_GPS;
        break;
      case REQ_MSP_COMP_GPS:
        MSPcmdsend = MSP_COMP_GPS;
        break;
      case REQ_MSP_ATTITUDE:
        MSPcmdsend = MSP_ATTITUDE;
        break;
      case REQ_MSP_ANALOG:
        MSPcmdsend = MSP_ANALOG;
        break;
#if defined DEBUGMW
      case REQ_MSP_DEBUG:
        MSPcmdsend = MSP_DEBUG;
        break;
#endif
#ifdef MULTIWII_V24
      case REQ_MSP_NAV_STATUS:
        if (MwSensorActive & mode.gpsmission)
          MSPcmdsend = MSP_NAV_STATUS;
        break;
#endif
    }

    //    if (!fontMode)
    mspWriteRequest(MSPcmdsend, 0);


    //  ProcessSensors();       // using analogue sensors


    //    if (armed) {
    //      previousarmedstatus = 1;
    //      if (configMode == 1)
    //        configExit();
    //    }
    //    if (previousarmedstatus && !armed) {
    //      configPage = 0;
    //      ROW = 10;
    //      COL = 1;
    //      configMode = 1;
    //      setMspRequests();
    //    }
    //    if (configMode)
    //    {
    //      displayConfigScreen();
    //    }
    //    else {
    setMspRequests();
    //#if defined USE_AIRSPEED_SENSOR
    //      useairspeed();
    //#endif //USE_AIRSPEED_SENSOR
    //      if (MwSensorPresent & ACCELEROMETER)
    //        displayHorizon(MwAngle[0], MwAngle[1]);
    //#if defined FORCECROSSHAIR
    //      displayForcedCrosshair();
    //#endif //FORCECROSSHAIR
    //      if (Settings[S_DISPLAYVOLTAGE])
    //        displayVoltage();
    //      if (Settings[S_VIDVOLTAGE])
    //        displayVidVoltage();
    //      if (Settings[S_DISPLAYRSSI] && ((rssi > Settings[S_RSSI_ALARM]) || (timer.Blink2hz)))
    //        displayRSSI();
    //      if (Settings[S_AMPERAGE] && (((amperage / 10) < Settings[S_AMPERAGE_ALARM]) || (timer.Blink2hz)))
    //        displayAmperage();
    //      if (Settings[S_AMPER_HOUR] && ((!ampAlarming()) || timer.Blink2hz))
    //        displaypMeterSum();
    //      displayTime();
    //#if defined (DISPLAYWATTS)
    //      displayWatt();
    //#endif //DISPLAYWATTS
    //#if defined (DISPLAYEFFICIENCY)
    //      displayEfficiency();
    //#endif //DISPLAYWATTS
    //#ifdef TEMPSENSOR
    //      if (((temperature < Settings[TEMPERATUREMAX]) || (timer.Blink2hz))) displayTemperature();
    //#endif
    //#ifdef VIRTUAL_NOSE
    //      displayVirtualNose();
    //#endif
    //#ifdef DISPLAYDOP
    //      displayDOP();
    //#endif
    //      displayArmed();
    //      if (Settings[S_THROTTLEPOSITION])
    //        displayCurrentThrottle();
    //#ifdef CALLSIGNALWAYS
    //      if (Settings[S_DISPLAY_CS]) displayCallsign(getPosition(callSignPosition));
    //#elif  FREETEXTLLIGHTS
    //      if (MwSensorActive & mode.llights) displayCallsign(getPosition(callSignPosition));
    //#elif  FREETEXTGIMBAL
    //      if (MwSensorActive & mode.camstab) displayCallsign(getPosition(callSignPosition));
    //#else
    //      if ( (onTime > (timer.lastCallSign + CALLSIGNINTERVAL)))
    //      {
    //        // Displays 4 sec every 5min (no blink during flight)
    //        if ( onTime > (timer.lastCallSign + CALLSIGNINTERVAL + CALLSIGNDURATION)) timer.lastCallSign = onTime;
    //        if (Settings[S_DISPLAY_CS]) displayCallsign(getPosition(callSignPosition));
    //      }
    //#endif
    //      if (MwSensorPresent & MAGNETOMETER) {
    //        displayHeadingGraph();
    //        displayHeading();
    //      }
    //      if (MwSensorPresent & BAROMETER) {
    //        displayAltitude();
    //        displayClimbRate();
    //      }
    //      if (MwSensorPresent & GPSSENSOR)
    //        if (Settings[S_DISPLAYGPS]) {
    //          displayNumberOfSat();
    //          displayDirectionToHome();
    //          displayDistanceToHome();
    //          displayAngleToHome();
    //#ifdef USEGLIDESCOPE
    //          // displayfwglidescope(); //note hook for this is in display horizon function
    //#endif //USEGLIDESCOPE
    //          displayGPS_speed();
    //          displayGPSPosition();
    //
    //#ifdef GPSTIME
    //          displayGPS_time();
    //#endif
    //#ifdef MAPMODE
    //          mapmode();
    //#endif
    //        }
    //      displayMode();
    //#ifdef I2CERROR
    //      displayI2CError();
    //#endif
    //#ifdef SPORT
    //      if (MwSensorPresent)
    //        displayCells();
    //#endif
    //      displayDebug();
    //#ifdef HAS_ALARMS
    //      displayAlarms();
    //#endif
    //    }
  }  // End of fast Timed Service Routine (50ms loop)




  //  if (timer.halfSec >= 5) {
  //    timer.halfSec = 0;
  //    timer.Blink2hz = ! timer.Blink2hz;
  //  }
  //
  //  if (millis() > timer.seconds + 1000)  // this execute 1 time a second
  //  {
  //    timer.seconds += 1000;
  //    timer.tenthSec = 0;
  //#ifdef DEBUGDPOSLOOP
  //    framerate = timer.loopcount;
  //    timer.loopcount = 0;
  //#endif
  //#ifdef DEBUGDPOSPACKET
  //    packetrate = timer.packetcount;
  //    timer.packetcount = 0;
  //#endif
  //#ifdef DEBUGDPOSRX
  //    serialrxrate = timer.serialrxrate;
  //    timer.serialrxrate = 0;
  //#endif
  //    onTime++;
  //
  //#ifdef ALARM_GPS
  //    if (timer.GPS_active == 0) {
  //      GPS_numSat = 0;
  //    }
  //    else {
  //      timer.GPS_active--;
  //    }
  //#endif // ALARM_GPS
  //    if (timer.MSP_active > 0) {
  //      timer.MSP_active--;
  //    }
  //    if (!armed) {
  //      //      setMspRequests();
  //#ifndef MAPMODENORTH
  //      armedangle = MwHeading;
  //#endif
  //    }
  //    else {
  //      flyTime++;
  //      flyingTime++;
  //      configMode = 0;
  //      setMspRequests();
  //    }
  //    allSec++;
  //    /*
  //        if((timer.accCalibrationTimer==1)&&(configMode)) {
  //          mspWriteRequest(MSP_ACC_CALIBRATION,0);
  //          timer.accCalibrationTimer=0;
  //        }
  //    */
  //    if ((timer.magCalibrationTimer == 1) && (configMode)) {
  //      mspWriteRequest(MSP_MAG_CALIBRATION, 0);
  //      timer.magCalibrationTimer = 0;
  //    }
  //    if (timer.magCalibrationTimer > 0) timer.magCalibrationTimer--;
  //    if (timer.rssiTimer > 0) timer.rssiTimer--;
  //  }
  //  //  setMspRequests();
  //  serialMSPreceive(1);
  //  //  delay(1);

}  // End of main loop

//------------------------------------------------------------------------
// MISC

//void resetFunc(void)
//{
//  asm volatile ("  jmp 0");
//}

void setMspRequests() {
  // if(configMode) {
  //    modeMSPRequests =
  //      REQ_MSP_STATUS|
  //      REQ_MSP_RAW_GPS|
  //      REQ_MSP_ATTITUDE|
  ////      REQ_MSP_RAW_IMU|
  //      REQ_MSP_ALTITUDE|
  //#ifdef DEBUGMW
  //      REQ_MSP_DEBUG|
  //#endif
  //
  //  }
  //  else {

  modeMSPRequests =
    REQ_MSP_STATUS |
#ifdef DEBUGMW
    REQ_MSP_DEBUG |
#endif
    REQ_MSP_ATTITUDE |
    0; // Sigh...

  //    if(MwSensorPresent&BAROMETER){
  //      modeMSPRequests |= REQ_MSP_ALTITUDE;
  //    }
  //    if(flags.ident!=1){
  //      modeMSPRequests |= REQ_MSP_IDENT;
  //    }
  //    if(MwSensorPresent&GPSSENSOR)
  modeMSPRequests |= REQ_MSP_RAW_GPS | REQ_MSP_COMP_GPS;

#if defined MULTIWII_V24
  if (MwSensorActive & mode.gpsmission)
    modeMSPRequests |= REQ_MSP_NAV_STATUS;
#endif
  //  }

  //  if(Settings[S_MAINVOLTAGE_VBAT] ||  Settings[S_MWRSSI]) {
  modeMSPRequests |= REQ_MSP_ANALOG;
  //  }

  queuedMSPRequests &= modeMSPRequests;   // so we do not send requests that are not needed.
}


#endif

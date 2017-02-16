/*
  command.cpp - ESP3D configuration class

  Copyright (c) 2014 Luc Lebosse. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "config.h"
#include "sdcard.h"
//#include "webinterface.h"

#ifdef SDCARD_FEATURE
#include <SPI.h>
#include <SdFat.h>
//#include <FS.h>
const uint8_t SD_CHIP_SELECT = SS;
#endif

//String SDCARD::buffer_serial;
//String SDCARD::buffer_tcp;

bool SDCARD::_sdinit = false; //
bool SDCARD::_volinit = false; //
SdFat sd;
//Sd2Card* card;
//SdVolume* volume;
//SdFile* root;

bool SDCARD::hasSD()
{
  return SDCARD::_sdinit & SDCARD::_volinit;
}


#ifdef SDCARD_FEATURE
void SDCARD::initSD()
{
  _sdinit = sd.begin(SD_CHIP_SELECT);
}

String SDCARD::cardInfo()
{
  String sdinfo = "Error";
  if (_sdinit) {
    String sdtype;
    uint32_t cardSize = sd.card()->cardSize();
    switch (sd.card()->type()) {
      case SD_CARD_TYPE_SD1:
        sdtype = F("SD1");
        break;
      case SD_CARD_TYPE_SD2:
        sdtype = F("SD2");
        break;
      case SD_CARD_TYPE_SDHC:
        if (cardSize < 70000000)
          sdtype = F("SDHC");
        else
          sdtype = F("SDXC");
        break;
      default:
        sdtype = F("Unknown");
    }
    sdinfo = F("Card type: ");
    sdinfo += sdtype;
    sdinfo += F(", volume type: FAT");
    sdinfo += String(sd.vol()->fatType(), DEC);
    sdinfo += F(", size: ");
    sdinfo += String(0.000512 * cardSize + 0.5, DEC);
    sdinfo += F("Mb");
  }
  return sdinfo;
}

char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

String SDCARD::jsonDirectory(String &path, String &sstatus)
{
  SdFile file;
  LOG("JSON File\r\n");
  String jsonfile = "{\"files\":[" ;
  if (_sdinit) {
    bool reschdir = sd.chdir(string2char(path), true);

    LOG("path - ")
    LOG(string2char(path))
    char bufs[50];
    sd.vwd()->getName(bufs, 50);
    LOG("\r\nsd.vwd() - ")
    LOG(bufs)
    LOG("\r\n")

    if (reschdir) {
      int i = 0;
      char buf[50];
      sd.vwd()->rewind();
      while (file.openNext(sd.vwd(), O_READ)) {
        if (i++ > 0) jsonfile += ",";
        if (file.getName(buf, 50)) {
          LOG("name:")
          LOG(String(buf))
          LOG(", size:")

          jsonfile += "{\"name\":\"";
          jsonfile += String(buf);
          jsonfile += "\",\"size\":\"";
          if (file.isDir()) {
            jsonfile += "-1";
            LOG("-1")
          } else { //it is a file
            LOG(CONFIG::formatBytes(file.fileSize()))
            jsonfile += CONFIG::formatBytes(file.fileSize());
          }
          jsonfile += "\"}";
          LOG("\r\n")
        }
        file.close();
      }
    } else {
      LOG("chdir() failed \r\n");
    }
  }

  //  float freeSpace = 0.000512* sd.vol()->freeClusterCount() *sd.vol()->blocksPerCluster();
  //  uint32_t freeKB = sd.vol()->freeClusterCount()*sd.vol()->blocksPerCluster()/2;

  jsonfile += "],\"path\":\"";
  jsonfile += path + "\",\"mode\":\"serial\",\"status\":\"";
  jsonfile += sstatus + "\",\"total\":\"";
  jsonfile += "-1";
  jsonfile += "\",\"used\":\"";
  jsonfile += "-1";
  jsonfile += "\",\"occupation\":\"";
  jsonfile +=  "-1";
  jsonfile += "\"";
  jsonfile += "}";

  LOG("JSON done\r\n");

  return jsonfile;
}

bool SDCARD::rmFile(String &filename)
{
  if (!_sdinit) return false;
  if (!sd.chdir()) LOG("chdir failed for /\r\n");
  bool res = sd.remove(string2char(filename));
  LOG("remove \"");
  LOG(filename);
  if (!res) LOG("\" failed\r\n")
    else LOG("\" success\r\n");
  return res;
}
bool SDCARD::rmDirectory(String &path)
{
  if (!_sdinit) return false;
  if (!sd.chdir(true)) LOG("chdir failed for /\r\n");
  bool res = sd.rmdir(string2char(path));
  LOG("remove dir \"");
  LOG(path);
  if (!res) LOG("\" failed\r\n")
    else LOG("\" success\r\n");
  return res;
}
bool SDCARD::mkDirectory(String &path)
{
  if (!_sdinit) return false;
  if (!sd.chdir(true)) LOG("chdir failed for /\r\n");
  bool res = sd.mkdir(string2char(path));
  LOG("create \"");
  LOG(path);
  if (!res) LOG("\" failed\r\n")
    else LOG("\" success\r\n");
  return res;
}

bool SDCARD::readFile(String filename, File &file)
{
  if (!_sdinit) return false;
  if (!sd.chdir(true)) LOG("chdir failed for /\r\n");
  
  bool res = sd.exists(string2char(filename)) && file.open(string2char(filename), O_READ);
  LOG("read \"");
  LOG(filename);
  if (!res) LOG("\" failed\r\n")
    else LOG("\" success\r\n");
  //      myFile.close();
  return res;
}
#endif

/////////////////


#ifdef TCP_IP_DATA_FEATURE
//read buffer as char
void SDCARD::read_buffer_tcp(uint8_t b)
{
  static bool previous_was_char = false;
  static bool iscomment = false;

}

//read a buffer in an array
void SDCARD::read_buffer_serial(uint8_t *b, size_t len)
{
  for (long i = 0; i < len; i++) {
    read_buffer_serial(*b);
    *b++;
  }
}

//read buffer as char
void SDCARD::read_buffer_serial(uint8_t b)
{
}
#endif



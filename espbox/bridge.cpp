/*
  bridge.cpp - esp3d bridge serial/tcp class

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

#include "bridge.h"
#include "webinterface.h"

bool BRIDGE::header_sent = false;
String BRIDGE::buffer_web = "";
void BRIDGE::print (const __FlashStringHelper *data, tpipe output){
    String tmp = data;
    BRIDGE::print(tmp.c_str(), output);
}
void BRIDGE::print (String & data, tpipe output){
    BRIDGE::print(data.c_str(), output);
}
void BRIDGE::print (const char * data, tpipe output){
    switch(output){
        case SERIAL_PIPE:
            header_sent = false;
            Serial.print(data);
        break;
        case SERIAL1_PIPE:
            header_sent = false;
            Serial1.print(data);
        break;
        case WEB_PIPE:
            if (!header_sent){
                web_interface->WebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
                web_interface->WebServer.sendHeader("Content-Type","text/html");
                web_interface->WebServer.sendHeader("Cache-Control","no-cache");
                web_interface->WebServer.send(200);
                header_sent = true;
            }
            buffer_web+=data;
            if (buffer_web.length() > 1200)
                {
                    //send data
                    web_interface->WebServer.sendContent(buffer_web);
                    //reset buffer
                    buffer_web="";
                }
        break;
        default:
        break;
    }
}
void BRIDGE::println (const __FlashStringHelper *data, tpipe output){
    BRIDGE::print(data,output);
    BRIDGE::print("\n",output);
}
void BRIDGE::println (String & data, tpipe output){
    BRIDGE::print(data,output);
    BRIDGE::print("\n",output);
}
void BRIDGE::println (const char * data, tpipe output){
    BRIDGE::print(data,output);
    BRIDGE::print("\n",output);
}
void BRIDGE::flush (tpipe output){
    switch(output){
        case SERIAL_PIPE:
            Serial.flush();
        break;
        case SERIAL1_PIPE:
            Serial1.flush();
        break;
        case WEB_PIPE:
            if(header_sent){
                //send data
                web_interface->WebServer.sendContent(buffer_web);
                //close line
                web_interface->WebServer.sendContent("");
                }
        break;
        default:
        break;
    }
    header_sent = false;
    buffer_web = String();
}


/*
  bridge.h - esp3d bridge serial/tcp class

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

#ifndef BRIDGE_H
#define BRIDGE_H
#include <WiFiServer.h>
#include "config.h"

class BRIDGE
{
public:
    static bool header_sent;
    static String buffer_web;
    static void print (const __FlashStringHelper *data, tpipe output);
    static void print (String & data, tpipe output);
    static void print (const char * data, tpipe output);
    static void println (const __FlashStringHelper *data, tpipe output);
    static void println (String & data, tpipe output);
    static void println (const char * data, tpipe output);
    static void flush (tpipe output);

};
#endif

/*
    You should have received a copy of the GNU General Public License
    along with this Firmware.  If not, see <http://www.gnu.org/licenses/>.

    This firmware is using the standard arduino IDE with module to support ESP8266:
    https://github.com/esp8266/Arduino from Bootmanager
*/
//be sure correct IDE and settings are used for ESP8266
#ifndef ARDUINO_ARCH_ESP8266
#error Oops!  Make sure you have 'ESP8266' compatible board selected from the 'Tools -> Boards' menu.
#endif
#include <EEPROM.h>
#include "config.h"
#ifdef MSP_LOG_FEATURE
#include "global.h"
#endif
#include "wifi.h"
#include "bridge.h"
#include "webinterface.h"
#ifdef SDCARD_FEATURE
#include "sdcard.h"
#endif

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#ifdef MDNS_FEATURE
#include <ESP8266mDNS.h>
#endif
#ifdef CAPTIVE_PORTAL_FEATURE
#include <DNSServer.h>
const byte DNS_PORT = 53;
DNSServer dnsServer;
#endif
#ifdef SSDP_FEATURE
#include <ESP8266SSDP.h>
#endif


char *letters = "abcdefghijklmnopqrstuvwxyz0123456789";
unsigned long previousMillis;
unsigned long previous_millis_high = 0;
uint32_t modeMSPRequests;
uint32_t queuedMSPRequests;


void setup()
{
  bool breset_config = false;
  long baud_rate = 0;
  web_interface = NULL;

  // init:
#ifdef DEBUG_ESP3D
  Serial.begin(DEFAULT_BAUD_RATE);
  delay(2000);
  LOG("\r\nDebug Serial set\r\n")
#endif
  //WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
#ifdef RECOVERY_FEATURE
  delay(8000);
  //check if reset config is requested
  pinMode(RESET_CONFIG_PIN, INPUT);
  if (digitalRead(RESET_CONFIG_PIN) == 0) {
    breset_config = true;  //if requested =>reset settings
  }
#endif
  //check if EEPROM has value
  if ( CONFIG::read_buffer(EP_BAUD_RATE,  (byte *)&baud_rate , INTEGER_LENGTH) && CONFIG::read_buffer(EP_WEB_PORT,  (byte *) & (wifi_config.iweb_port) , INTEGER_LENGTH) && CONFIG::read_buffer(EP_DATA_PORT,  (byte *) & (wifi_config.idata_port) , INTEGER_LENGTH)) {
    //check if baud value is one of allowed ones
    if ( ! (baud_rate == 9600 || baud_rate == 19200 || baud_rate == 38400 || baud_rate == 57600 || baud_rate == 115200 || baud_rate == 230400 || baud_rate == 250000) ) {
      LOG("Error for EEPROM baud rate\r\n")
      breset_config = true;  //baud rate is incorrect =>reset settings
    }
    if (wifi_config.iweb_port < 1 || wifi_config.iweb_port > 65001 || wifi_config.idata_port < 1 || wifi_config.idata_port > 65001) {
      breset_config = true;  //out of range =>reset settings
      LOG("Error for EEPROM port values\r\n")
    }

  } else {
    breset_config = true;  //cannot access to config settings=> reset settings
    LOG("Error no EEPROM access\r\n")
  }

  //reset is requested
  if (breset_config) {
    //update EEPROM with default settings
    Serial.begin(DEFAULT_BAUD_RATE);
    //        Serial.setRxBufferSize(SERIAL_RX_BUFFER_SIZE);
    delay(2000);
    Serial.println(F("M117 ESP EEPROM reset"));
#ifdef DEBUG_ESP3D
    CONFIG::print_config(DEBUG_PIPE);
    delay(1000);
#endif
    CONFIG::reset_config();
    delay(1000);
    //put some default value to a void some exception at first start
    WiFi.mode(WIFI_AP);
    WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    CONFIG::esp_restart();
  }
#if defined(DEBUG_ESP3D) && defined(DEBUG_OUTPUT_SERIAL)
  LOG("\r\n");
  delay(500);
  Serial.flush();
#endif
  //setup serial
  Serial.begin(baud_rate);
  //    Serial.setRxBufferSize(SERIAL_RX_BUFFER_SIZE);
  delay(1000);
  LOG("Serial Set\r\n");
  wifi_config.baud_rate = baud_rate;
  //Update is done if any so should be Ok
  SPIFFS.begin();

#ifdef SDCARD_FEATURE
  SDCARD::initSD();

  LOG(SDCARD::cardInfo())
  LOG("\r\n")
#endif
  //setup wifi according settings
  if (!wifi_config.Setup()) {
    Serial.println(F("M117 Safe mode 1"));
    //try again in AP mode
    if (!wifi_config.Setup(true)) {
      Serial.println(F("M117 Safe mode 2"));
      wifi_config.Safe_Setup();
    }
  }
  delay(1000);
  //start web interface
  web_interface = new WEBINTERFACE_CLASS(wifi_config.iweb_port);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  web_interface->WebServer.collectHeaders(headerkeys, headerkeyssize );
#ifdef CAPTIVE_PORTAL_FEATURE
  if (WiFi.getMode() != WIFI_STA ) {
    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  }
#endif
  web_interface->WebServer.begin();


#ifdef MDNS_FEATURE
  // Check for any mDNS queries and send responses
  wifi_config.mdns.addService("http", "tcp", wifi_config.iweb_port);
#endif
#ifdef SSDP_FEATURE
  String shost;
  if (!CONFIG::read_string(EP_HOSTNAME, shost , MAX_HOSTNAME_LENGTH)) {
    shost = wifi_config.get_default_hostname();
  }
  String stmp;
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort( wifi_config.iweb_port);
  SSDP.setName(shost.c_str());
  stmp = String(ESP.getChipId());
  SSDP.setSerialNumber(stmp.c_str());
  SSDP.setURL("/");
  SSDP.setModelName("ESP8266 01");
  SSDP.setModelNumber("01");
  SSDP.setModelURL("http://espressif.com/en/products/esp8266/");
  SSDP.setManufacturer("Espressif Systems");
  SSDP.setManufacturerURL("http://espressif.com");
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.begin();
#endif
  LOG("Setup Done\r\n");
}


//main loop
void loop()
{
#ifdef MSP_LOG_FEATURE
  proccess_msp();
#endif

#ifdef CAPTIVE_PORTAL_FEATURE
  if (WiFi.getMode() != WIFI_STA ) {
    dnsServer.processNextRequest();
  }
#endif
  //web requests
  web_interface->WebServer.handleClient();

  if (web_interface->restartmodule) {
    CONFIG::esp_restart();
  }


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    //    previousMillis = currentMillis;
    String randString = "";
    int numBytes = random(0, 20);
    for (int i = 0; i < numBytes; i++)
      randString += letters[random(0, 36)];

    randString += " | ";
    randString += String(previousMillis);
    randString += " - ";
    randString += String(currentMillis);

    //    LOG("Write randString \"")
    //    LOG(randString)
    //    LOG("\"\r\n")

   // SDCARD::append_file("/logs/logfile.txt", randString);
    previousMillis = currentMillis;
  }

}

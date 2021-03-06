ESPNode
=========
Based on ESPClock : https://github.com/spapadim/ESPClock

Features :
* Web-based configuration UI
* Clock synchronization over NTP with timezone/daylight saving options
* Network scanner

#### Dependencies
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [Time](https://github.com/PaulStoffregen/Time)
* Arduino ESP built-in libraries (WiFi, WebServer)
* [Esp8266-Arduino-Makefile](https://github.com/thunderace/Esp8266-Arduino-Makefile) for linux command line build

## Install
git clone --recursive https://github.com/thunderace/ESPNode.git

## Build
if you change a html file : 
*   ./tools/xxd_html.sh settings.html
*   ./tools/xxd_html.sh basicsetup.html
build and upload : make upload
 
![screenshot](https://github.com/thunderace/ESPNode/raw/master/screenshots/ESPNode.png)



 


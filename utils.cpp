#include "utils.h"


String getAPSSID() {
	uint8_t mac[6];
	char macStr[18];
	WiFi.macAddress(mac);
  sprintf(macStr, "ESP%02X%02X%02X%02X%02X%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}


String getEncryptionStr(unsigned char enc) {
	switch(enc) {
		case ENC_TYPE_WEP:
			return String("WEP");
		case ENC_TYPE_TKIP:
			return String("TKIP");
		case ENC_TYPE_CCMP:
			return String("CCMP");
		case ENC_TYPE_NONE:
			return String("None");
		case ENC_TYPE_AUTO:
			return String("AUTO");
		default:
			return String("????");
	}
}

String scanWifi() {
  String networks;
	int n = WiFi.scanNetworks();
 	if (n == 0) {
 		networks = "<font color='#FF0000'>No networks found!</font>";
	} else {
		networks = "Found " + String(n) + " Networks<br>";
		networks += "<table width='100%' border='0' cellspacing='0' cellpadding='3'>";
		networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i) {
			int quality=0;
			if (WiFi.RSSI(i) <= -100) {
				quality = 0;
			} else {
				if (WiFi.RSSI(i) >= -50) {
					quality = 100;
				} else {
					quality = 2 * (WiFi.RSSI(i) + 100);
				}
			}
			networks += "<tr><td>"  +  WiFi.SSID(i)  + "</td><td>" +  String(quality) + "%</td><td>" +  getEncryptionStr(WiFi.encryptionType(i))  + "</td></tr>";
		}
		networks += "</table>";
	}
	return networks;
}

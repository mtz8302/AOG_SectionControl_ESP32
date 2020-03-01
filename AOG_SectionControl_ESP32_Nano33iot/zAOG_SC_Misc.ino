//-------------------------------------------------------------------------------------------------

void WiFi_LED_blink()
{
	//no data for more than 2 secs = blink
	if (millis() > (DataFromAOGTime + 3000L)) {
		if (!LED_WIFI_ON) {
			if (millis() > (LED_WIFI_time + LED_WIFI_pause)) {
				LED_WIFI_time = millis();
				LED_WIFI_ON = true;
#if useLED_BUILTIN
				digitalWrite(LED_BUILTIN, HIGH);
#endif
				digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
			}
		}
		if (LED_WIFI_ON) {
			if (millis() > (LED_WIFI_time + LED_WIFI_pulse)) {
				LED_WIFI_time = millis();
				LED_WIFI_ON = false;
#if useLED_BUILTIN
				digitalWrite(LED_BUILTIN, LOW);
#endif
				digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
			}
		}
	}

	else {
		digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
#if useLED_BUILTIN
		digitalWrite(LED_BUILTIN, HIGH);
#endif			
	}
}

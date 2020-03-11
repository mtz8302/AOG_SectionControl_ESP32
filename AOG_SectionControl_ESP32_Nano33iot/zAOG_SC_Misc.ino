//-------------------------------------------------------------------------------------------------

void WiFi_LED_blink(byte blkSpeed)   //8. März 2020
{
	unsigned long currentTime = millis();
	if (!LED_WIFI_ON) {
		if (currentTime > (LED_WIFI_time + (LED_WIFI_pause >> blkSpeed))) {
			LED_WIFI_time = currentTime;
			LED_WIFI_ON = true;
#if useLED_BUILTIN
			digitalWrite(LED_BUILTIN, HIGH);
#endif
			digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
		}
	}
	if (LED_WIFI_ON) {
		if (currentTime > (LED_WIFI_time + (LED_WIFI_pulse >> blkSpeed))) {
			LED_WIFI_time = currentTime;
			LED_WIFI_ON = false;
#if useLED_BUILTIN
			digitalWrite(LED_BUILTIN, LOW);
#endif
			digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
		}
	}
}

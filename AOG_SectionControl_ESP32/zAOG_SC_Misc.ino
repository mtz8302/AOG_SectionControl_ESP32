//-------------------------------------------------------------------------------------------------
// 12. Mai 2022

void WiFi_LED_blink_and_Connection_check(void* pvParameters)
{
    unsigned long now, saveTime = 0, last_connection = 0;
    byte blkSpeed, noConnectionCounter = 0;

    for (;;) {

        now = millis();

        if (WiFi_connect_step > 0) { blkSpeed = 2; }
        else {
            if (now > (DataFromAOGTime + 1000L)) {
                //no new data
                if ((Set.DataTransVia > 5) && (Set.DataTransVia < 10)) {
                    if (WiFi.status() == WL_CONNECTED) { noConnectionCounter = 0; }
                    else {
                        //WiFi lost
                        noConnectionCounter++;
                        if (noConnectionCounter > 15) {
                            //no connection for about 20 sec
                            Serial.print("no WiFi connection for more than 30s to WiFi Network ");
                            switch (WiFi_netw_nr) {
                            case 1: Serial.println(Set.ssid1); break;
                            case 2: Serial.println(Set.ssid2); break;
                            case 3: Serial.println(Set.ssid3); break;
                            case 4: Serial.println(Set.ssid4); break;
                            case 5: Serial.println(Set.ssid5); break;
                            }
                            Serial.println("reconnecting to WiFi");
                            Serial.println();
                            WiFi_STA_connect_call_nr = 0;//to enable DHCP for WiFi
                            WiFi_connect_step = 3;
                            my_WiFi_Mode = 0;
                            WiFiUDPRunning = false;

                            //search longer for lost network, before creating access point
                            if (Set.timeoutRouter < 60) { Set.timeoutRouter = 90; }

                            if (!task_WiFiConnectRunning) {
                                //start WiFi
                                xTaskCreate(WiFi_handle_connection, "WiFiConnectHandle", 3072, NULL, 1, &taskHandle_WiFi_connect);
                                delay(500);
                            }
                        }
                    }
                }//WiFi

                if ((SectAuto) && (DataFromAOGTime != 0)) { blkSpeed = 3; }
                else { blkSpeed = 0; }//first run or manual mode

            }
            else { //all OK = LED on
                vTaskDelay(1200);
                blkSpeed = 255;
                LED_WIFI_ON = true;
                digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
#if useLED_BUILTIN
                digitalWrite(LED_BUILTIN, HIGH);
#endif
            }
        }

        if (blkSpeed != 255) {
            if (!LED_WIFI_ON) {
                LED_WIFI_time = now;
                LED_WIFI_ON = true;
#if useLED_BUILTIN
                digitalWrite(LED_BUILTIN, HIGH);
#endif
                digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);

                vTaskDelay(LED_WIFI_pause >> blkSpeed);

            }
            if (LED_WIFI_ON) {
                LED_WIFI_time = now;
                LED_WIFI_ON = false;
#if useLED_BUILTIN
                digitalWrite(LED_BUILTIN, LOW);
#endif
                digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);

                vTaskDelay(LED_WIFI_pulse >> blkSpeed);
            }
        }
    }
}


/*//-------------------------------------------------------------------------------------------------
//9. Maerz 2021

void WiFi_LED_blink(void* pvParameters)
{
	unsigned long now;
	byte blkSpeed;

	for (;;) {

		now = millis();

		if (WiFi_connect_step > 0) { blkSpeed = 2; }
		else {
			if (now > (DataFromAOGTime + 1000L)) {
				if ((SectAuto) && (DataFromAOGTime != 0)) { blkSpeed = 3; }
				else { blkSpeed = 0; }
			}
			else { //all OK = LED on
				vTaskDelay(800);
				blkSpeed = 255;
				LED_WIFI_ON = true;
				digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
#if useLED_BUILTIN
				digitalWrite(LED_BUILTIN, HIGH);
#endif
			}
		}

		if (blkSpeed != 255) {
			if (!LED_WIFI_ON) {
				LED_WIFI_time = now;
				LED_WIFI_ON = true;
#if useLED_BUILTIN
				digitalWrite(LED_BUILTIN, HIGH);
#endif
				digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);

				vTaskDelay(LED_WIFI_pause >> blkSpeed);

			}
			if (LED_WIFI_ON) {
				LED_WIFI_time = now;
				LED_WIFI_ON = false;
#if useLED_BUILTIN
				digitalWrite(LED_BUILTIN, LOW);
#endif
				digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);

				vTaskDelay(LED_WIFI_pulse >> blkSpeed);
			}
		}
	}
}*/

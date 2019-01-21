//core2 does UDP communication
void Core2code(void * pvParameters) {
	delay(3000);
	for (;;)
	{ // MAIN LOOP FOR THIS CORE
		//delay(0);

		UDP_read();
		if (NewDataToAOG) { UDP_write(); NewDataToAOG = false; }
	}
}

void UDP_read()
{
	//delay(0); //do WIFI
	UDP.onPacket([](AsyncUDPPacket packet)
	{   UDPFromAOGLength = packet.length();
		if (UDPFromAOGLength == DataFromAOGLength)
		{
			//Serial.print("UPD from AOG items: ");
			//Serial.print(UDPFromAOGLength);
			UDPFromAOGData[0] = packet.data()[0];
			UDPFromAOGData[1] = packet.data()[1];
			if ((UDPFromAOGData[0] == DataFromAOGHeader[0]) && (UDPFromAOGData[1] == DataFromAOGHeader[1]))  //Do we have a match?
			{
				Serial.print("                                                                    UDP from AOG data:  ");
				for (byte i = 2; i < UDPFromAOGLength; i++)
				{
					Serial.print(UDPFromAOGData[i]);
					Serial.print(",");
					UDPFromAOGData[i] = packet.data()[i];
				}
				isDataFound = true;
				Serial.println();
			}
			if ((UDPFromAOGData[0] == SettingsHeader[0]) && (UDPFromAOGData[1] == SettingsHeader[1]))
			{
				for (byte i = 2; i < UDPFromAOGLength; i++)
				{
					Serial.print(".");
					UDPFromAOGData[i] = packet.data()[i];
				}
				isSettingFound = true;
			}
		}
	//	UDP.flush();
	});//end packet

	
	//DATA Header 
		if (isDataFound)
		{
			isDataFound = false;
			RelayFromAOG[1] = UDPFromAOGData[2];   // read relay control from AgOpenGPS
			RelayFromAOG[0] = UDPFromAOGData[3];
			groundSpeed = UDPFromAOGData[4] >> 2;  //actual speed times 4, single byte
			rateSetPointLeft = (float)(UDPFromAOGData[5] << 8 | UDPFromAOGData[6]);   rateSetPointLeft *= 0.01; //high,low bytes; sent as 100 times value in liters per minute
			rateSetPointRight = (float)(UDPFromAOGData[7] << 8 | UDPFromAOGData[8]); rateSetPointRight *= 0.01; //high,low bytes
			uTurnRelay = UDPFromAOGData[9];
			//Serial.println(RelayFromAOG[0]);
			//reset watchdog as we just heard from AgOpenGPS
			watchdogTimer = 0;
			RelayFromAOGTime = millis();			
		}

		//SETTINGS Header has been found, 6 bytes are the settings
		if (isSettingFound)
		{
			isSettingFound = false;  //reset the flag

			//accumulated volume, 0 it if this is 32700 sent
			float tempf = (float)(UDPFromAOGData[2] << 8 | UDPFromAOGData[3]);   //high,low bytes
			if (tempf == 32700) accumulatedCountsDual = 0;

			//flow meter cal factor in counts per Liter
			flowmeterCalFactorLeft = ((float)(UDPFromAOGData[4] << 8 | UDPFromAOGData[5]));   //high,low bytes
			flowmeterCalFactorRight = ((float)(UDPFromAOGData[6] << 8 | UDPFromAOGData[7]));   //high,low bytes
		}
			
	//UDPFromAOGLength = 0; //process data only once
}//----------------------UDP read


void UDP_write()
{
	//delay(0);
	UDPSC.writeTo(RelayToAOG, UDPToAOGLength, AOGIP, PortToAOG);
}

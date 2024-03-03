
//getData 7. Maerz 2021
void getDataFromAOGUSB(void* pvParameters)
{
	byte nextincommingBytesArrayNr;
	unsigned int packetLength;

	USBDataTaskRunning = true;
	Serial.println(); Serial.println("started Task get Data via USB"); Serial.println();
	for (;;) {		
		//get Data
		packetLength = Serial.available();
		if (packetLength > 0) {
			nextincommingBytesArrayNr = (incommingBytesArrayNr + 1) % incommingDataArraySize;
			for (int i = 0; i < packetLength; i++) { incommingBytes[nextincommingBytesArrayNr][i] = Serial.read(); }
			incommingBytesArrayNr = nextincommingBytesArrayNr;
			incommingDataLength[incommingBytesArrayNr] = packetLength;
		}
		else { vTaskDelay(5); }
	}
}


//-------------------------------------------------------------------------------------------------


void getDataFromAOGEth(void* pvParameters)
{
	byte nextincommingBytesArrayNr;
	unsigned int packetLength;

	EthDataTaskRunning = true;

	for (;;) {
		if (!EthUDPRunning) { vTaskDelay(3000); }
		else { break; }
	}
	 Serial.println(); Serial.println("started Task get Data via Ethernet");  Serial.println();
	for (;;) {
		//get Data		
		packetLength = EthUDPFromAOG.parsePacket();
		if (packetLength) {
			nextincommingBytesArrayNr = (incommingBytesArrayNr + 1) % incommingDataArraySize;
			EthUDPFromAOG.read(incommingBytes[nextincommingBytesArrayNr], packetLength);
			incommingBytesArrayNr = nextincommingBytesArrayNr;
			incommingDataLength[incommingBytesArrayNr] = packetLength;
		}
		else { vTaskDelay(5); }
	}
}

//-------------------------------------------------------------------------------------------------
//parseData 7. Maerz 2021, 25. Feb 2023: AgIO request added, V17 deleted

void parseDataFromAOG() {
	for (int i = 0; i < incommingDataLength[incommingBytesArrayNrToParse]; i++) {
		//sentence comming? V5: 80 81 7F PGN
		if (incomSentenceDigit < 3) {
			if (incommingBytes[incommingBytesArrayNrToParse][i] == FromAOGSentenceHeader[incomSentenceDigit]) {
				//Serial.println("first 3 Bytes fit: sentence");
				SentenceFromAOG[incomSentenceDigit] = incommingBytes[incommingBytesArrayNrToParse][i];
				incomSentenceDigit++;
			}
			else { incomSentenceDigit = 0; }
		}//<3
		else {
			//write incoming Data to sentence array if it fits in
			if (incomSentenceDigit <= SentenceFromAOGMaxLength) {
				SentenceFromAOG[incomSentenceDigit] = incommingBytes[incommingBytesArrayNrToParse][i];
			}
			if (incomSentenceDigit == 3) {
				incomSentenceDigit++;
				//which sentence comming? PGN
				switch (incommingBytes[incommingBytesArrayNrToParse][i]) {
				case SteerDataFromAOGHeader:
					isSteerDataFound = true; //SC uses steer data in V5
					break;
				case SCSettingsFromRCHeader:
					isRCSettingFound = true;
					break;
				case AgIO_heartbeat:
					if (Set.AgIOHeartbeat_answer > 0) { SCHeartbeatSend(); }//called here, as AgIO checksum is not correct
					isAgIOHeartbeatFound = true;
					break;
				case AgIO_ScanRequest:
					isAgIOScanRequestFound = true;
					break;
				default:
					//Serial.println("no matching PGN");
					incomSentenceDigit = 0;
					break;
				}//switch
			}//==3
			else {// >3
				if (incomSentenceDigit == 4) {//lenght
					SentenceFromAOGLength = incommingBytes[incommingBytesArrayNrToParse][i];
					incomSentenceDigit++;

				}//==4
				else
				{//>4	
					if (incomSentenceDigit == (SentenceFromAOGLength + 5)) { //sentence complete Length: + 4 byte header + 1 length + 1 CRC - 1 (starting at 0) 
						//sentence complete
						int CRCDataFromAOG = 0;
						for (byte chk = 2; chk < (SentenceFromAOGLength + 5); chk++)
						{
							CRCDataFromAOG = (CRCDataFromAOG + SentenceFromAOG[chk]);
						}
						if (byte(CRCDataFromAOG) != incommingBytes[incommingBytesArrayNrToParse][i])
						{//checksum error
							if (Set.debugmodeDataFromAOG) {
								Serial.print("Checksum failed: exp: ");
								Serial.print(byte(CRCDataFromAOG));
								Serial.print(" chk rvd: ");
								Serial.print(incommingBytes[incommingBytesArrayNrToParse][SentenceFromAOGLength + 5]);
								Serial.print(" AOG data lgth: "); Serial.println(SentenceFromAOGLength);
							}
							isSteerDataFound = false;
							isRCSettingFound = false;
							isAgIOHeartbeatFound = false;
							isAgIOScanRequestFound = false;
							incomSentenceDigit = 255;
						}//checksum error

						if (Set.debugmodeDataFromAOG) {
							for (byte b = 0; b <= (SentenceFromAOGLength + 5); b++) {
								Serial.print(SentenceFromAOG[b]); Serial.print(" ");
							}
							Serial.println();
						}

						if (isSteerDataFound) {
							SectGrFromAOG[0] = SentenceFromAOG[11];   // read Section control from AgOpenGPS 
							SectGrFromAOG[1] = SentenceFromAOG[12];   // read Section control from AgOpenGPS 

							gpsSpeed = ((float)(SentenceFromAOG[6] << 8 | SentenceFromAOG[5])) * 0.1;

							//newDataFromAOG = true;
							isSteerDataFound = false;
							incomSentenceDigit = 255;
							DataFromAOGTime = millis();
							if (Set.debugmodeDataFromAOG) {
								Serial.print("speed: "); Serial.print(gpsSpeed);
								Serial.print(" SectGr[0]: "); Serial.print(SectGrFromAOG[0]);
								Serial.print(" SectGr[1]: "); Serial.println(SectGrFromAOG[1]);
							}
						}
						else {

							if (isRCSettingFound) {
								isRCSettingFound = false;
								incomSentenceDigit = 255;
							}
							else {
								if (isAgIOHeartbeatFound) {
									SCHeartbeatSend();
									isAgIOHeartbeatFound = false;
									incomSentenceDigit = 255;
									DataFromAOGTime = millis();
								}
								else {
									if (AgIO_ScanRequest) {
										SCScanRequestReply();
										isAgIOScanRequestFound = false;
										incomSentenceDigit = 255;
										DataFromAOGTime = millis();
									}
								}
							}
						}
					}//sentence complete


					incomSentenceDigit++;

					//sentence too long
					if (incomSentenceDigit > (SentenceFromAOGLength + 6)) { incomSentenceDigit = 0; }

				}//>4
			}//==3
		}//<3
	}//for packetLength
	incommingBytesArrayNrToParse = (incommingBytesArrayNrToParse + 1) % incommingDataArraySize;
}


//-------------------------------------------------------------------------------------------------
// 7. Maerz 2021, Feb 2023: AsyncUDP for WiFi

void AOGDataSend()
{
	//USB
	if (Set.DataTransVia < 5) { Serial.write(SCToAOG, DataToAOGLength); }
	else {
		if (Set.DataTransVia < 10)//WiFi UDP
		{
			WiFiUDPToAOG.writeTo(SCToAOG, sizeof(SCToAOG), WiFi_ipDestination, Set.PortDestination);
		}
		else //Ethernet
		{
			EthUDPToAOG.beginPacket(Eth_ipDestination, Set.PortDestination);
			EthUDPToAOG.write(SCToAOG, DataToAOGLength);
			EthUDPToAOG.endPacket();
		}
	}
}

//-------------------------------------------------------------------------------------------------
// 23. Feb 2023

void SCHeartbeatSend()
{
	if (Set.AgIOHeartbeat_answer > 0) {
		helloFromSC[5] = RelayOUT[0];
		helloFromSC[6] = RelayOUT[1];
		//checksum
		int16_t CK_A = 0;
		for (uint8_t i = 2; i < sizeof(helloFromSC) - 1; i++)
		{
			CK_A = (CK_A + helloFromSC[i]);
		}
		helloFromSC[sizeof(helloFromSC) - 1] = CK_A;

		//USB
		if (Set.DataTransVia < 5) {
			Serial.write(helloFromSC, sizeof(helloFromSC));
		}
		else {
			if (Set.DataTransVia < 10)//WiFi UDP
			{
				WiFiUDPToAOG.writeTo(helloFromSC, sizeof(helloFromSC), WiFi_ipDestination, Set.PortDestination);
			}
			else //Ethernet
			{
					EthUDPToAOG.beginPacket(Eth_ipDestination, Set.PortDestination);
					EthUDPToAOG.write(helloFromSC, sizeof(helloFromSC));
					EthUDPToAOG.endPacket();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
// 23. Feb 2023

void SCScanRequestReply()
{
	uint8_t SCScanReply[] = { 128, 129, 123, 203, 7,
								WiFi_ipDestination[0],WiFi_ipDestination[1],WiFi_ipDestination[2],Set.WiFi_myip[3],0,0,0, 23 };
	if (Set.DataTransVia >= 10) {
		SCScanReply[5] = Eth_ipDestination[0]; SCScanReply[6] = Eth_ipDestination[1];
		SCScanReply[7] = Eth_ipDestination[2]; SCScanReply[8] = Set.Eth_myip[3];
	}
	//checksum
	int16_t CK_A = 0;
	for (uint8_t i = 2; i < sizeof(SCScanReply) - 1; i++)
	{
		CK_A = (CK_A + SCScanReply[i]);
	}
	SCScanReply[sizeof(SCScanReply) - 1] = CK_A;

	//USB
	if (Set.DataTransVia < 5) {
		Serial.write(SCScanReply, sizeof(SCScanReply));
	}
	else {
		if (Set.DataTransVia < 10)//WiFi UDP
		{
			WiFiUDPToAOG.writeTo(SCScanReply, sizeof(SCScanReply), WiFi_ipDestination, Set.PortDestination);
		}
		else //Ethernet
		{
				EthUDPToAOG.beginPacket(Eth_ipDestination, Set.PortDestination);
				EthUDPToAOG.write(SCScanReply, sizeof(SCScanReply));
				EthUDPToAOG.endPacket();
		}
	}
}

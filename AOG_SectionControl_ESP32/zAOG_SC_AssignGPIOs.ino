void assignGPIOs() {
	//#if HardwarePlatform == 0
	for (int i = 0; i < Set.SectNum; i++)
	{
		if (Set.SectSWInst) {
			if (Set.SectSW_PIN[i] < 255) {
				if (Set.SectSW_PIN[i] < 100) pinMode(Set.SectSW_PIN[i], INPUT_PULLUP);
				else PCAinUse = true;
			}
			else { Serial.print("no PIN set for section switch #"); Serial.println(i + 1); }
		}
		if (Set.SectRelaysInst) {
			if (Set.Relay_PIN[i] < 255) {
				if (Set.Relay_PIN[i] < 100) pinMode(Set.Relay_PIN[i], OUTPUT);
				else PCAinUse = true;
			}
			else { Serial.print("no PIN set for section relay #"); Serial.println(i + 1); }
		}
	}
	if (Set.SectSWInst) {
		if (Set.SectAutoManSW_PIN < 255) { pinMode(Set.SectAutoManSW_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for Auto/Manual toggle switch / buttons"); }
	}
	if (Set.SectMainSWType != 0) {
		if (Set.SectMainSW_PIN < 255) { pinMode(Set.SectMainSW_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for Main ON/OFF toggle switch / buttons"); }
	}
	if (Set.RateSWLeftInst) {
		if (Set.RateSWLeft_PIN < 255) { pinMode(Set.RateSWLeft_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for left Rate +/- left"); }
	}
	if (Set.RateSWRightInst) {
		if (Set.RateSWRight_PIN < 255) { pinMode(Set.RateSWRight_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for right Rate +/- left"); }
	}
	if (Set.RateControlLeftInst && (Set.FlowDirLeft_PIN == 255 || Set.FlowPWMLeft_PIN == 255)) {
		Serial.println("no rate control left: no PIN set for flowmeter, or for PWM outout");
		Set.RateControlLeftInst = false;
	}
	if (Set.RateControlRightInst && (Set.FlowDirRight_PIN == 255 || Set.FlowPWMRight_PIN == 255)) {
		Serial.println("no rate control right: no PIN set for flowmeter, or for PWM outout");
		Set.RateControlRightInst = false;
	}

	if (Set.FlowDirLeft_PIN < 255) { pinMode(Set.FlowDirLeft_PIN, OUTPUT); }
	if (Set.FlowPWMLeft_PIN < 255) { pinMode(Set.FlowPWMLeft_PIN, OUTPUT); }
	if (Set.FlowDirRight_PIN < 255) { pinMode(Set.FlowDirRight_PIN, OUTPUT); }
	if (Set.FlowPWMRight_PIN < 255) { pinMode(Set.FlowPWMRight_PIN, OUTPUT); }

	if (Set.Relais_MainValve_PIN < 255) { pinMode(Set.Relais_MainValve_PIN, OUTPUT); }


	if (PCAinUse) {
		if (!Wire.begin()) { Serial.println("error INIT wire, PCA9555 will not work"); }
		delay(500);//needed for wire to init

		//MCP23017 init
		/*PCA9555 set input by default
		Wire.beginTransmission(Set.I2CAddrPCAInput);
		Wire.write(0x00); // IODIRA register
		Wire.write(0xFF); // set all of port A to inputs
		Wire.endTransmission();
		Wire.beginTransmission(Set.I2CAddrPCAInput);
		Wire.write(0x01); // IODIRB register
		Wire.write(0xFF); // set all of port B to inputs
		Wire.endTransmission();
		Wire.beginTransmission(Set.I2CAddrPCAInput);
		Wire.write(0x0C); // GPPUA register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();
		Wire.beginTransmission(Set.I2CAddrPCAInput);
		Wire.write(0x0D); // GPPUB register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();*/

		//PCA9555 output
		Wire.beginTransmission(Set.I2CAddrPCAOutput);
		Wire.write(0x06); // config port 0 register
		Wire.write(0x00); // set all of port A to outputs
		Wire.endTransmission();
		Wire.beginTransmission(Set.I2CAddrPCAOutput);
		Wire.write(0x07); // config port 1 register
		Wire.write(0x00); // set all of port B to outputs
		Wire.endTransmission();

	}





	RelayOUT[0] = 0;
	RelayOUT[1] = 0;
	SetRelays();

	if (Set.LEDWiFi_PIN != 255) { pinMode(Set.LEDWiFi_PIN, OUTPUT); }


	/* good, but won't work with hitch level sensor or switch connected with hitch
	if (Set.SectMainSWType==1)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++) {
			MainSWVal = analogRead(Set.SectMainSW_PIN); delay(10);
			if (MainSWVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { Set.SectSWInst = false; MainSWVal = 500; }
	}
	*/

	//check if MainSw and RateSw are realy in use, because if not the INPUT_PULLUP would deliver always ON or Rate +
	if (Set.RateSWLeftInst)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++)
		{
			RateSWLeftVal = analogRead(Set.RateSWLeft_PIN); delay(10);
			if (RateSWLeftVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { Set.RateSWLeftInst = false; RateSWLeftVal = 500; }
	}
	if (Set.RateSWRightInst)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++) {
			RateSWRightVal = analogRead(Set.RateSWRight_PIN); delay(10);
			if (RateSWRightVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { Set.RateSWRightInst = false; RateSWRightVal = 500; }
	}

	//#endif



		//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
		//if (Set. RateControlLeftInst && !Set.FlowEncALeft_PIN == 0) { Set. RateControlLeftInst = true; attachInterrupt(digitalPinToInterrupt(Set.FlowEncALeft_PIN), pinLeftChangeISR, CHANGE); }
		//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
		//if (Set.RateControlRightInst && !Set.FlowEncARight_PIN == 0) { Set. RateControlLeftInst = true; attachInterrupt(digitalPinToInterrupt(Set.FlowEncARight_PIN), pinRightChangeISR, CHANGE); }

		//PWM rate settings Adjust to desired PWM Rate
		//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
		//TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)

}

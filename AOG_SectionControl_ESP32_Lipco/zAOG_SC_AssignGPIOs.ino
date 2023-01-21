void assignGPIOs() {
	//Lipco mod	
	if (Set.FronttankSW_PIN < 255) { pinMode(Set.FronttankSW_PIN, INPUT_PULLUP); }
	else { Serial.println("no PIN set for Fronttank switch"); }
	if (Set.TunnelLeftSW_PIN < 255) { pinMode(Set.TunnelLeftSW_PIN, INPUT_PULLUP); }
	else { Serial.println("no PIN set for left Tunnel switch"); }
//	if (Set.TunnelRightNarrowerSW_PIN < 255) { pinMode(Set.TunnelRightNarrowerSW_PIN, INPUT_PULLUP); }
//	else { Serial.println("no PIN set for right tunnel narrower switch"); }
//	if (Set.TunnelRightWiderSW_PIN < 255) { pinMode(Set.TunnelRightWiderSW_PIN, INPUT_PULLUP); }
//	else { Serial.println("no PIN set for right tunnel wider switch"); }

	if (Set.MCPInUse) {
		if (!Wire.begin()) { Serial.println("error INIT wire, MCP23017 will not work"); }
		delay(500);//needed for wire to init

		//cmp23017 init
		//input
		Wire.beginTransmission(0x20);
		Wire.write(0x00); // IODIRA register
		Wire.write(0xFF); // set all of port A to inputs
		Wire.endTransmission();
		Wire.beginTransmission(0x20);
		Wire.write(0x01); // IODIRB register
		Wire.write(0xFF); // set all of port B to inputs
		Wire.endTransmission();
		Wire.beginTransmission(0x20);
		Wire.write(0x0C); // GPPUA register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();
		Wire.beginTransmission(0x20);
		Wire.write(0x0D); // GPPUB register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();

		//output
		Wire.beginTransmission(0x21);
		Wire.write(0x00); // IODIRA register
		Wire.write(0x00); // set all of port A to outputs
		Wire.endTransmission();
		Wire.beginTransmission(0x21);
		Wire.write(0x01); // IODIRB register
		Wire.write(0x00); // set all of port B to outputs
		Wire.endTransmission();
		Wire.beginTransmission(0x21);
		Wire.write(0x0C); // GPPUA register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();
		Wire.beginTransmission(0x21);
		Wire.write(0x0D); // GPPUB register pullup
		Wire.write(0xFF); // set all pullups active
		Wire.endTransmission();


		/*	mcpForInput.init();
			delay(100);
			mcpForInput.portMode(MCP23017Port::A,
				0b11111111,   // direction (IODIRB) - set direction of all pins as inputs
				0b11111111,   // pull-up (GPPUB) - enable 100K pull-up resistors on all inputs
				0b00000000);  // polarity (IPOLB) - don't invert logic polarity for all inputs
			mcpForInput.portMode(MCP23017Port::B,
				0b11111111,   // direction (IODIRB) - set direction of all pins as inputs
				0b11111111,   // pull-up (GPPUB) - enable 100K pull-up resistors on all inputs
				0b00000000);  // polarity (IPOLB) - don't invert logic polarity for all inputs
			delay(20);
			mcpForOutput.init();
			delay(100);
			mcpForOutput.portMode(MCP23017Port::A,
				0b00000000);  // direction (IODIRA) - set direction of all pins as outputs
			mcpForOutput.portMode(MCP23017Port::B,
				0b00000000);  // direction (IODIRA) - set direction of all pins as outputs
			mcpForOutput.writeRegister(MCP23017Register::GPIO_A, 0x00);  //Reset port A
			mcpForOutput.writeRegister(MCP23017Register::GPIO_B, 0x00);  //Reset port B
			*/
	}

	for (int i = 0; i < Set.SectNum; i++)
	{
		if (Set.SectSWInst) {
			if (Set.SectSW_PIN[i] < 100) { pinMode(Set.SectSW_PIN[i], INPUT_PULLUP); }
			else {
				if (Set.SectSW_PIN[i] == 255) { Serial.print("no PIN set for section switch #"); Serial.println(i + 1); }
			}
		}
		if (Set.Relay_PIN[i] < 100) { pinMode(Set.Relay_PIN[i], OUTPUT); }
		else {
			if (Set.SectSW_PIN[i] == 255) { Serial.print("no PIN set for section relay #"); Serial.println(i + 1); }
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
	if (Set.RateControlAutoManSW_PIN < 100) { pinMode(Set.RateControlAutoManSW_PIN, INPUT_PULLUP); }
	else {
		if (Set.RateControlAutoManSW_PIN < 255) {//mcpForOutput.pinMode(Set.RateControlAutoManSW_PIN - 100, INPUT_PULLUP, false);
		}
		else { Serial.println("no PIN set for Rate control Auto/Manual switch"); }
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
	if (Set.FlowDirLeft_PIN < 100) { pinMode(Set.FlowDirLeft_PIN, OUTPUT); }
	//		else {if (Set.FlowDirLeft_PIN < 255) { mcpForOutput.pinMode(Set.FlowDirLeft_PIN - 100, OUTPUT, false); }}

	if (Set.FlowPWMLeft_PIN < 100) { pinMode(Set.FlowPWMLeft_PIN, OUTPUT); }
	//		else { if (Set.FlowPWMLeft_PIN < 255) { mcpForOutput.pinMode(Set.FlowPWMLeft_PIN - 100, OUTPUT, false); } }

	if (Set.FlowDirRight_PIN < 100) { pinMode(Set.FlowDirRight_PIN, OUTPUT); }
	//		else { if (Set.FlowDirRight_PIN < 255) { mcpForOutput.pinMode(Set.FlowDirRight_PIN - 100, OUTPUT, false); } }

	if (Set.FlowPWMRight_PIN < 100) { pinMode(Set.FlowPWMRight_PIN, OUTPUT); }
	//	else { if (Set.FlowPWMRight_PIN < 255) { mcpForOutput.pinMode(Set.FlowPWMRight_PIN - 100, OUTPUT, false); } }

	if (Set.SectMainSW_PIN < 100) { pinMode(Set.Relais_MainValve_PIN, OUTPUT); }
	//	else { if (Set.SectMainSW_PIN < 255) { mcpForOutput.pinMode(Set.SectMainSW_PIN - 100, OUTPUT, false); } }


	/*	//Lipco mods
		mcpForInput.pinMode(Set.AgitatorSW_PIN - 100, INPUT_PULLUP, false);
		mcpForOutput.pinMode(Set.AgitatorRel_PIN - 100, OUTPUT, false);
		mcpForInput.pinMode(Set.RecyclingSW_PIN - 100, INPUT_PULLUP, false);
		mcpForOutput.pinMode(Set.RecyclingRel_PIN - 100, OUTPUT, false);
		mcpForInput.pinMode(Set.FanSW_PIN - 100, INPUT_PULLUP, false);
		mcpForOutput.pinMode(Set.FanRel_PIN - 100, OUTPUT, false);
	*/

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


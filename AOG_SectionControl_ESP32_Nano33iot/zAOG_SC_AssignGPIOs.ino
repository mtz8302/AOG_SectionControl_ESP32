void assignGPIOs() {
//#if HardwarePlatform == 0
	for (int i = 0; i < SCSet.SectNum; i++)
	{
		if (SCSet.SectSWEquiped) {
			if (SCSet.SectSW_PIN[i] < 255) { pinMode(SCSet.SectSW_PIN[i], INPUT_PULLUP); }
			else { Serial.print("no PIN set for section switch #"); Serial.println(i+1); }
			if (SCSet.Relay_PIN[i] < 255) { pinMode(SCSet.Relay_PIN[i], OUTPUT); }
			else { Serial.print("no PIN set for section relay #"); Serial.println(i+1); }
		}
	}
	if (SCSet.SectSWEquiped) {
		if (SCSet.SectAutoManSW_PIN < 255) { pinMode(SCSet.SectAutoManSW_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for Auto/Manual toggle switch / buttons"); }
	}
	if (SCSet.SectMainSWType != 0) {
		if (SCSet.SectMainSW_PIN < 255) { pinMode(SCSet.SectMainSW_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for Main ON/OFF toggle switch / buttons"); }
	}
	if (SCSet.RateSWLeftEquiped) {
		if (SCSet.RateSWLeft_PIN < 255) { pinMode(SCSet.RateSWLeft_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for left Rate +/- left"); }
	}
	if (SCSet.RateSWRightEquiped) {
		if (SCSet.RateSWRight_PIN < 255) { pinMode(SCSet.RateSWRight_PIN, INPUT_PULLUP); }
		else { Serial.println("no PIN set for right Rate +/- left"); }
	}
	if (SCSet.RateControlLeftEquiped && (SCSet.FlowDirLeft_PIN == 255 | SCSet.FlowPWMLeft_PIN == 255)) {
		Serial.println("no rate control left: no PIN set for flowmeter, or for PWM outout");
		SCSet.RateControlLeftEquiped = false;
	}
	if (SCSet.RateControlRightEquiped && (SCSet.FlowDirRight_PIN == 255 | SCSet.FlowPWMRight_PIN == 255)) {
		Serial.println("no rate control right: no PIN set for flowmeter, or for PWM outout");
		SCSet.RateControlRightEquiped = false;
	}
	if (SCSet.FlowDirLeft_PIN < 255) { pinMode(SCSet.FlowDirLeft_PIN, OUTPUT); }
	if (SCSet.FlowPWMLeft_PIN < 255) { pinMode(SCSet.FlowPWMLeft_PIN, OUTPUT); }
	if (SCSet.FlowDirRight_PIN < 255) { pinMode(SCSet.FlowDirRight_PIN, OUTPUT); }
	if (SCSet.FlowPWMRight_PIN < 255) { pinMode(SCSet.FlowPWMRight_PIN, OUTPUT); }

	RelayOUT[0] = 0;
	RelayOUT[1] = 0;
	SetRelays();

	if (SCSet.LEDWiFi_PIN != 255) { pinMode(SCSet.LEDWiFi_PIN, OUTPUT); }




	/* good, but won't work with hitch level sensor or switch connected with hitch
	if (SCSet.SectMainSWType==1)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++) {
			MainSWVal = analogRead(SCSet.SectMainSW_PIN); delay(10);
			if (MainSWVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet.SectSWEquiped = false; MainSWVal = 500; }
	}
	*/

	//check if MainSw and RateSw are realy in use, because if not the INPUT_PULLUP would deliver always ON or Rate +
	if (SCSet.RateSWLeftEquiped)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++)
		{
			RateSWLeftVal = analogRead(SCSet.RateSWLeft_PIN); delay(10);
			if (RateSWLeftVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet.RateSWLeftEquiped = false; RateSWLeftVal = 500; }
	}
	if (SCSet.RateSWRightEquiped)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++) {
			RateSWRightVal = analogRead(SCSet.RateSWRight_PIN); delay(10);
			if (RateSWRightVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet.RateSWRightEquiped = false; RateSWRightVal = 500; }
	}
//#endif



	//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
	//if (SCSet. RateControlLeftEquiped && !SCSet.FlowEncALeft_PIN == 0) { SCSet. RateControlLeftEquiped = true; attachInterrupt(digitalPinToInterrupt(SCSet.FlowEncALeft_PIN), pinLeftChangeISR, CHANGE); }
	//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
	//if (SCSet.RateControlRightEquiped && !SCSet.FlowEncARight_PIN == 0) { SCSet. RateControlLeftEquiped = true; attachInterrupt(digitalPinToInterrupt(SCSet.FlowEncARight_PIN), pinRightChangeISR, CHANGE); }

	//PWM rate settings Adjust to desired PWM Rate
	//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
	//TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)

}

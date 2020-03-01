void SectSWRead()
{
	SectSWcurrentTime = millis();

	SectSWOffToAOG[0] = 0;
	SectSWOffToAOG[1] = 0;

	//read all switches
	if (SCSet.debugmodeSwitches) Serial.println("reading Section switches:");
	for (int i = 0; i < SCSet.SectNum; i++)
	{
		if (SCSet.SectSW_PIN[i] < 255) { SectSWVal[i] = digitalRead(SCSet.SectSW_PIN[i]); }
		else { SectSWVal[i] = HIGH; }//set auto if switch is missing
		//Documentation: spray on LOW
		if (SCSet.DocumentationOnly && !SCSet.DocSwitchSprayOn) { SectSWVal[i] = !SectSWVal[i]; }
		if (SCSet.debugmodeSwitches) {
			Serial.print("  #"); Serial.print(i + 1); Serial.print(" "); Serial.print(SectSWVal[i]);
		}
	}
	if (SCSet.debugmodeSwitches) Serial.println();


	if (SCSet.SectMainSWType != 0) { MainSWVal = analogRead(SCSet.SectMainSW_PIN); }
	if (SCSet.debugmodeSwitches) { Serial.print("Main Switch value: "); Serial.println(MainSWVal); }

	AutoSWVal = digitalRead(SCSet.SectAutoManSW_PIN);
	if (SCSet.debugmodeSwitches) { Serial.print("Auto/Manual switch: "); Serial.println(AutoSWVal); }



	//AUTO / MAN SW
	if (AutoSWVal == LOW)  SectAuto = false; //pullup = auto if open or not connected
	else SectAuto = true;
	if (SectAuto != SectAutoOld)
	{
		if (!SectAutoSWpressed) { SectAutoSWTime = SectSWcurrentTime; delay(1); } //catch time, first time switch is moved, delay to debounce
		SectAutoSWpressed = true;
		if (SectSWcurrentTime > SectAutoSWTime + SectSWDelayTime) { SectAutoSWpressed = false; SectAutoOld = SectAuto; }
	}

//Main ON/OFF	
	//checking MainSWType an "level" hitch level input to "normal" toggle switch values
	// 0 = not equiped 1 = (ON)-OFF-(ON) toggle switch or push buttons 2 = connected to hitch level sensor 3 = inverted hitch level sensor
	if (SCSet.SectMainSWType== 2)  
	{
		//using hitch sensor low value = hitch down = working -> so invert signal
		if (MainSWVal < SCSet.HitchLevelVal) { MainSWVal = SWON + 100; }
		else { MainSWVal = SWOFF - 100; }
		if (MainSWVal == MainSWValOld) { MainSWVal = ((SWOFF + SWON) / 2); }//set "switch" to middle, when same status as last time
		else { MainSWValOld = MainSWVal; } //first time when changed
	}
	if (SCSet.SectMainSWType == 3)  //3 = inverted hitch level sensor
	{
		//using hitch sensor high value = hitch down = working -> so fit signal level to standard
		if (MainSWVal > SCSet.HitchLevelVal) { MainSWVal = SWON + 100; }
		else { MainSWVal = SWOFF - 100; }
		if (MainSWVal == MainSWValOld) { MainSWVal = ((SWOFF + SWON) / 2); }//set "switch" to middle, when same status as last time
		else { MainSWValOld = MainSWVal; } //first time when changed
	}
	
	//if Main toggle switch pressed, use it for delay time and signal to AGO stays for SectSWDelayTime
	if (MainSWVal > SWON) { SectMainOn = true; SectMainSWpressed = true; SectMainSWlastTime = SectSWcurrentTime; }
	if (MainSWVal < SWOFF) { SectMainOn = false; SectMainSWpressed = true; SectMainSWlastTime = SectSWcurrentTime; }
	if (SectSWcurrentTime > SectMainSWlastTime + SectSWDelayTime) { SectMainSWpressed = false; }

	//if any section is spraying by AOG, and MainSW is not used set MainOn = ON, else OFF
	if (!SectMainSWpressed && SectAuto) {
		if ((RelayFromAOG[0] > 0) || (RelayFromAOG[1] > 0)) { SectMainOn = true; }
		else SectMainOn = false;
	}
	
	//Documentation only = set to manual, Main = ON so every section is on, or off by sections switch
	if (SCSet.DocumentationOnly == 1) {
		SectAuto = false;
		SectMainOn = true;
	}

	//set relay bytes -----------------------
	//SectSWOffToAOGOld[0] = SectSWOffToAOG[0]; SectSWOffToAOGOld[1] = SectSWOffToAOG[1];
	SectSWOffToAOG[0] = 0; SectSWOffToAOG[1] = 0;
	byte a = 0;
	byte SectNumLoop = 0;
	boolean allDone = false;
	if (SCSet.SectNum > 8) { SectNumLoop = SCSet.SectNum - 8; a = 1; }
	else { SectNumLoop = SCSet.SectNum; a = 0; }
	do
	{
		for (int i = 0;i < SectNumLoop;i++) {
			if (SectSWVal[i + a * 8] == HIGH) {
				//ON or Open = pullup
				if (SectAuto) {
					//if auto then do what AOG says
					if (bitRead(RelayFromAOG[a], i) == HIGH) { bitSet(RelayOUT[a], i); }
					else bitClear(RelayOUT[a], i);
				}
				else {
					//Manu
					if (SectMainOn) { bitSet(RelayOUT[a], i); }
					else bitClear(RelayOUT[a], i);
				} //else
			}//if on
			if (SectSWVal[i + a * 8] == LOW) {
				//switch OFF: Relays off, tell AOG: OFF
				bitClear(RelayOUT[a], i);
				bitSet(SectSWOffToAOG[a], i);
			}//if
		}//for 
		if (a > 0)//if more than 8 sections: run for loop again for byte [1]
		{a = 0; SectNumLoop = 8;}
		else { allDone = true; }
	}//while
	while (!allDone);

	//Set Bytes for AGO communication-------------------

	 //send Relais to AGO  !! x High/low Byte
	if (!SectAuto) { RelayToAOG[6] = RelayOUT[0]; RelayToAOG[5] = RelayOUT[1]; }
	else { RelayToAOG[6] = 0; RelayToAOG[5] = 0; }

	//Set Main SW Bits
	bitClear(SectMainToAOG, 0);
	bitClear(SectMainToAOG, 1);
	//if Main sw moved send to AOG
	if (SectMainSWpressed) {
		if (SectMainOn && SectAuto) { bitSet(SectMainToAOG, 0); }
		if (!SectMainOn) { bitSet(SectMainToAOG, 1); }
	}

	//MAN and all sections OFF by MAIN then send SW OFF to AOG
	if (!SectAuto && !SectMainOn) {
		if (SCSet.SectNum > 8)
		{
			for (byte k = 0; k < 8; k++) { bitSet(SectSWOffToAOG[0], k); }
			for (byte k = 7; k < SCSet.SectNum;k++) { bitSet(SectSWOffToAOG[1], k - 7); }
		}
		else for (byte k = 0; k < SCSet.SectNum;k++) { bitSet(SectSWOffToAOG[0], k); }
	}

	//if AUTO/Man SW moved to Auto and Main on, send Main on to AOG to turn all auto and wait for new RelayByte
	if (SectAutoSWpressed && SectAuto && SectMainOn)
	{
		bitSet(SectMainToAOG, 0); RelayOUT[0] = RelayOUTOld[0];RelayOUT[1] = RelayOUTOld[1];
	}
	//if AUTO/Man SW moved to Manu and Main on, keep spraying same sections for delaytime then
	if (SectAutoSWpressed && !SectAuto && SectMainOn)
	{
		RelayOUT[0] = RelayOUTOld[0];RelayOUT[1] = RelayOUTOld[1];
	}
	if (!SectAutoSWpressed) { RelayOUTOld[0] = RelayOUT[0];RelayOUTOld[1] = RelayOUT[1]; }

	//if AUTO/Man SW moved to MAN, send Main on to AOG to turn all off
	if (SectAutoSWpressed && !SectAuto && !SectMainOn)
	{
		bitClear(SectMainToAOG, 0); bitSet(SectMainToAOG, 1); RelayOUT[0] = 0;RelayOUT[1] = 0;
	}
}//end of SectSWRead

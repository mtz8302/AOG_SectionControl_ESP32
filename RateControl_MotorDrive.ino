void motorDrive(void)
{
	if (RateControlLeftEquiped) {
	//if (pwmDrive > 0) bitSet(PORTB, 4);  //set the correct direction
	//analogWrite(FlowPWMLeft_PIN, pwmDrive);
	}
	else 
	//manual motor drive
	{
		if (!RateControlPWM) 
		{	//if rate switch is toggled -> set Output high
			if (RateManUpLeft) { digitalWrite(FlowDirLeft_PIN, HIGH); digitalWrite(FlowPWMLeft_PIN, LOW); }
			if (RateManDownLeft) { digitalWrite(FlowDirLeft_PIN, LOW); digitalWrite(FlowPWMLeft_PIN, HIGH); }
			if (RateManUpRight) { digitalWrite(FlowDirRight_PIN, HIGH); digitalWrite(FlowPWMRight_PIN, LOW); }
			if (RateManDownRight) { digitalWrite(FlowDirRight_PIN, LOW); digitalWrite(FlowPWMRight_PIN, HIGH); }
			//toggle switch not pressed = set all low
			if (!(RateManUpLeft | RateManDownLeft | RateManUpRight | RateManDownRight))
				{ digitalWrite(FlowDirRight_PIN, LOW); digitalWrite(FlowPWMRight_PIN, LOW); digitalWrite(FlowDirLeft_PIN, LOW); digitalWrite(FlowPWMLeft_PIN, LOW);
				}
		}
	}
}

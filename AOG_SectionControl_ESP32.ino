
struct Storage {
//User config: ***********************************************************************************

//WiFi settings
	char ssid[24] = "Fendt_209V";			// WiFi network Client name
	char password[24] = "";					// WiFi network password

	byte AOGIP[4] = { 192,168,10,255 };
	byte GatewayIP[4] = { 192,168,10,1 };
	byte MyIP[4] = { 192,168,10,71 };
	byte DNS[4] = { 192,168,10,1 };
	byte SubNet[4] = { 255,255,255,0 };

	byte WifiRouterResponseTime = 10;//60;     //time (approx seconds) to try connecting router, should be higher as time for router to boot, afterwards creating an AP,

//the following lines should be configed by the user to fit the programm to the sprayer/ESP32
//GPIOs of the ESP32 (current setting is for the layout shown as example WIKI)

// if only 1 flowrate is used, use left
//Example1: motor valve is controled only by Switch not by AOG, no Flowmeter, : RateControl..Equiped = false; RateSW..Equiped = true; RateControlPRM = false;
//Example2: PWM valve, with flowmeter all controled by AOG:   RateControl..Equiped = true; RateSW..Equiped = true; RateControlPRM = true;	
	boolean RateControlLeftEquiped = false;		//true if Rate control is there, else: false
	boolean RateSWLeftEquiped = true;			//true if Rate control Pressure switch is there, else: false
	byte    RateSWLeft_PIN = 34;				//Rate +/- switch
	boolean RateControlPWM = false;				//true if PWM valve, false if Motor drive for pressure change		

	byte	FlowDirLeft_PIN = 19;				//Rate-Control Valve/Motor Direktion
	byte	FlowPWMLeft_PIN = 23;				//Rate-Control Valve PWM/Motor ON/OFF
	byte	FlowEncALeft_PIN = 36;				//Flowmeter left/1

	boolean RateControlRightEquiped = false;	//true if Rate control is there, else: false
	boolean RateSWRightEquiped = false;			//true if Rate control Pressure switch is there, else: false	
	byte	RateSWRight_PIN = 0;				//Rate +/- switch
	byte	FlowDirRight_PIN = 0;				//0  = unused Rate-Control Valve Direktion
	byte	FlowPWMRight_PIN = 0;				//0  = unused Rate-Control Valve PWM
	byte	FlowEncARight_PIN = 0;				//Flowmeter right/2 
	

	byte SectNum = 7;								// number of sections equiped max 12 in AOG (JAN 2019), might be enlarged to 16
	int Relay_PIN[12] = { 15,2,4,16,17,5,18 };		//GPIOs of ESP32 OUT to sections of sprayer HIGH/3.3V = ON

	boolean SectSWequiped = true;					//true if section input switches are equiped, else: false	
	int SectSW_PIN[12] = { 13,12,14,27,26,25,33 };  //section switches to GPIOs of ESP32 GND = section off, open/+3.3V section auto/on

	byte	SectMainSWType = 1;						// 0 = not equiped 1 = (ON)-OFF-(ON) toggle switch or push buttons 2 = connected to hitch level sensor 3 = inverted hitch level sensor
	int		HitchLevelVal = 2000;					// Value for hitch level: switch AOG section control to Auto if lower than...
	byte	SectMainSW_PIN = 32;					//ESP32 to AOG Main auto toggle switch open=nothing/AOG button GND=OFF +3,3=AOG Auto on	OR connected to hitch level sensor	
	byte	SectAutoManSW_PIN = 39;					// Main Auto/Manual switch 39:!!no internal pullup!!


 // END of user config ****************************************************************************
}; Storage SCSet;


TaskHandle_t Core1;
TaskHandle_t Core2;


//ToDo: - Rate / flowmeter doesn't work, interrupt mode ect not defined
//      - line 260 PWM + Interrupt mode ect...
//		- Right side Rate caluclation / output / Motordrive
//      - Manual mode for Rate Control
//		- what to do if no WIFI??


/*	IPAddress AOGIP(192, 168, 10, 255);
	IPAddress GatewayIP(192, 168, 10, 1);
	IPAddress MyIP(192, 168, 10, 71);
	IPAddress DNS(192, 168, 10, 1);
*/

//Daten for UDP Broadcast to AOG
const unsigned int PortFromAOG = 8888;//port to listen, AOG sends on 8888
const unsigned int PortToAOG = 9999;  //AOG listens to 9999
const unsigned int PortSCToAOG = 5555;//section control sends as 5555
const char* ssidAP = "SectionControl_AP";   // WIFI name if access point, password = "" none
#define BaudRate 38400                   //Baudrate = speed of serial port or USB or Bluetooth. AOG uses 38400 for UART
const byte SettingsHeader[] = { 0x7F, 0xF8 };
byte RelayToAOG[] =    { 127,249,0,0,0,0,0,0,0,0 };  //7F F9
byte RelayToAOGOld[] = { 127,249,0,0,0,0,0,0,0,0 };
const byte DataFromAOGHeader[] = { 127,250 };  // 0x7F, 0xFA };
const byte UDPToAOGLength = 10; //items per UDP packet
uint8_t UDPFromAOGLength = 0;
const uint8_t DataFromAOGLength = 10; //items comming from AOG
uint8_t UDPFromAOGData[255];
IPAddress AOGIPAdr(SCSet.AOGIP[0], SCSet.AOGIP[1], SCSet.AOGIP[2],SCSet.AOGIP[3]);


#include <WiFi.h>
#include <AsyncUDP.h>
//#include <HardwareSerial.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
//#include <BluetoothSerial.h>
#include "EEPROM.h"
//#include <base64.h>

//needed to feed the watchdog timer
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


WiFiServer server(80);
//WiFiClient ntripCl;
WiFiClient client_page;
AsyncUDP UDP = AsyncUDP();
AsyncUDP UDPSC = AsyncUDP();

//values to decide position of section switch
#define SWOFF 350 // analog in is lower than .. for off
#define SWON  2900 // analog in is lower than .. for auto/on

//state of the switches HIGH = ON/Auto LOW = OFF								 
boolean SectSWVal[16] = { HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH };
boolean AutoSWVal = HIGH;

//analog value of the toggle switches
int MainSWVal = 2000;
int MainSWValOld = 2000;
int RateSWLeftVal = 2000;
int RateSWRightVal = 2000;

//loop time variables in microseconds
const unsigned long LOOP_TIME = 200;// 400; //in msec; 1000 = 1 Hz
const unsigned long SectSWDelayTime = 1400;//1400; //time the arduino waits after manual Switch is used before acception command from AOG in msec
unsigned long lastTime = LOOP_TIME;
unsigned long currentTime = LOOP_TIME;
unsigned long RelayFromAOGTime = LOOP_TIME;
unsigned long SectAutoSWTime = LOOP_TIME;
unsigned long SectAutoSWlastTime = LOOP_TIME;
unsigned long SectMainSWlastTime = LOOP_TIME;
unsigned long RateSWlastTimeLeft = LOOP_TIME;
unsigned long RateSWDelayTime = 250; //time to pass before a new toggle of switch is accepted = if hold down, time between steps
unsigned long RateSWlastTimeRight = LOOP_TIME;
unsigned long SectSWcurrentTime = LOOP_TIME;
unsigned int dT = 100;
byte watchdogTimer = 0;

//Kalman variables Left
float rateKLeft = 0;
float PcLeft = 0.0;
float GLeft = 0.0;
float PLeft = 1.0;
float XpLeft = 0.0;
float ZpLeft = 0.0;
float XeRateLeft = 0; //the filtered flowrate
const float varRateLeft = 100; // variance, smaller, more filtering
const float varProcessLeft = 10;

//Kalman variables Right
float rateKRight = 0;
float PcRight = 0.0;
float GRight = 0.0;
float PRight = 1.0;
float XpRight = 0.0;
float ZpRight = 0.0;
float XeRateRight = 0; //the filtered flowrate
const float varRateRight = 100; // variance, smaller, more filtering
const float varProcessRight = 10;

//program flow
bool isDataFound = false, isSettingFound = false;
int header = 0, tempHeader = 0;

//bit 0 on byte[0] is section 1
byte RelayFromAOG[] = { 0, 0 }; //Sections ON / OFF Bitwise !!! LOWByte, HighByte

byte RelayOUT[] = { 0,0 }; //Sections ON / OFF Bitwise !!! LOWByte, HighByte
byte RelayOUTOld[] = { 0,0 };
byte SectSWOffToAOG[] = { 0,0 };
byte SectSWOffToAOGOld[] = { 0,0 };
byte SectMainToAOG = 0;
//byte SectMainFromAOG = 0;
byte uTurnRelay = 0;
//  byte flowRateSW1 = 0;
boolean SectMainSWpressed = false;
boolean SectSWpressed = false;
boolean SectSWpressedLoop = false;
boolean SectAuto = true;
boolean SectAutoOld = true;
boolean SectAutoSWpressed = false;
//boolean RateAuto = false;
boolean RateManUpLeft = false;
boolean RateManUpRight = false;
boolean RateManDownLeft = false;
boolean RateManDownRight = false;

boolean SectMainOn = false;
boolean BitVal = false;

boolean NewDataToAOG = false;

float groundSpeed = 0; //speed from AgOpenGPS is multiplied by 4
float rateSetPointLeft = 0.0;
float countsThisLoopLeft = 0;
float rateErrorLeft = 0; //for PID
int rateAppliedLPMLeft = 0;
float rateSetPointRight = 0.0;
float countsThisLoopRight = 0;
float rateErrorRight = 0; //for PID
int rateAppliedLPMRight = 0;

unsigned long accumulatedCountsLeft = 0;
float flowmeterCalFactorLeft = 50;
unsigned long accumulatedCountsRight = 0;
float flowmeterCalFactorRight = 50;
unsigned long accumulatedCountsDual = 0;

float pulseAverageLeft = 0;
float pulseAverageRight = 0;

//the ISR counter
volatile unsigned long pulseCountLeft = 0, pulseDurationLeft;
volatile unsigned long pulseCountRight = 0, pulseDurationRight;
bool state = false;

void setup()
{
	for (int i = 0; i < SCSet.SectNum; i++)
	{
		if (SCSet.SectSWequiped) { pinMode(SCSet.SectSW_PIN[i], INPUT_PULLUP); }
		pinMode(SCSet.Relay_PIN[i], OUTPUT);
	}
	if (SCSet.SectSWequiped) { pinMode(SCSet.SectAutoManSW_PIN, INPUT_PULLUP); }
	if (SCSet.SectMainSWType != 0) { pinMode(SCSet.SectMainSW_PIN, INPUT_PULLUP); }
	if (SCSet. RateSWLeftEquiped && !SCSet.RateSWLeft_PIN == 0) pinMode(SCSet.RateSWLeft_PIN, INPUT_PULLUP);
	if (SCSet.RateSWRightEquiped && !SCSet.RateSWRight_PIN == 0) pinMode(SCSet.RateSWRight_PIN, INPUT_PULLUP);
	if (SCSet. RateControlLeftEquiped && (SCSet.FlowDirLeft_PIN == 0 | SCSet.FlowPWMLeft_PIN == 0)) { SCSet. RateControlLeftEquiped = false; }
	if (SCSet.RateControlRightEquiped && (SCSet.FlowDirRight_PIN == 0 | SCSet.FlowPWMRight_PIN == 0)) { SCSet.RateControlRightEquiped = false; }
	if (!SCSet.FlowDirLeft_PIN == 0) { pinMode(SCSet.FlowDirLeft_PIN, OUTPUT); }
	if (!SCSet.FlowPWMLeft_PIN == 0) { pinMode(SCSet.FlowPWMLeft_PIN, OUTPUT); }
	if (!SCSet.FlowDirRight_PIN == 0) { pinMode(SCSet.FlowDirRight_PIN, OUTPUT); }
	if (!SCSet.FlowPWMRight_PIN == 0) { pinMode(SCSet.FlowPWMRight_PIN, OUTPUT); }
	
	RelayOUT[0] = 0;
	RelayOUT[1] = 0;
	SetRelays();


	//check if MainSw and RateSw are realy in use, because if not the INPUT_PULLUP would deliver always ON or Rate +
	
	
	/* good, but won't work with hitch level sensor or switch connected with hitch
	if (SCSet.SectMainSWType==1)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++) {
			MainSWVal = analogRead(SCSet.SectMainSW_PIN); delay(10);
			if (MainSWVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet.SectSWequiped = false; MainSWVal = 500; }
	}
	*/
	if (SCSet. RateSWLeftEquiped)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0; i < 5; i++)
		{
			RateSWLeftVal = analogRead(SCSet.RateSWLeft_PIN); delay(10);
			if (RateSWLeftVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet. RateSWLeftEquiped = false; RateSWLeftVal = 500; }
	}
	if (SCSet.RateSWRightEquiped)
	{
		byte no = 0;      //5 times check if pulled up
		for (byte i = 0;i < 5; i++) {
			RateSWRightVal = analogRead(SCSet.RateSWRight_PIN); delay(10);
			if (RateSWRightVal > SWON) { delay(500); no++; }
		}//for
		if (no > 4) { SCSet.RateSWRightEquiped = false; RateSWRightVal = 500; }
	}
		

	//set up communication
	Serial.begin(BaudRate);
	delay(50);
	Serial.flush();   // flush out buffer

	//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
	//if (SCSet. RateControlLeftEquiped && !SCSet.FlowEncALeft_PIN == 0) { SCSet. RateControlLeftEquiped = true; attachInterrupt(digitalPinToInterrupt(SCSet.FlowEncALeft_PIN), pinLeftChangeISR, CHANGE); }
	//attachInterrupt(0, ISR, mode); //below is preferred method - most compatible
	//if (SCSet.RateControlRightEquiped && !SCSet.FlowEncARight_PIN == 0) { SCSet. RateControlLeftEquiped = true; attachInterrupt(digitalPinToInterrupt(SCSet.FlowEncARight_PIN), pinRightChangeISR, CHANGE); }
			
	//PWM rate settings Adjust to desired PWM Rate
	//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
	//TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)

	
	// connect with existing WIFI  
	WiFi.mode(WIFI_STA);
	WiFi.begin(SCSet.ssid, SCSet.password);
	WiFi.config(SCSet.MyIP, SCSet.GatewayIP, SCSet.SubNet, SCSet.DNS);
	// wait for connection
	byte z = 0;
	Serial.print("WIFI: trying to connect to   ");
	Serial.println(SCSet.ssid);
	while ((WiFi.status() != WL_CONNECTED) & (z <SCSet. WifiRouterResponseTime))
	{
		delay(800);
		z++;
		Serial.print(".");
	}
	if (z <SCSet. WifiRouterResponseTime)
	{
		Serial.println("");
		Serial.print("connected: IPaddress: ");
		Serial.println(WiFi.localIP());
	}
	else
	{
		//work as access point  
		WiFi.mode(WIFI_AP);
		//   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); 
		WiFi.softAPConfig(SCSet.MyIP, SCSet.GatewayIP, SCSet.SubNet);
		WiFi.softAP(ssidAP, "");
		Serial.println("");
		Serial.print("working as access point: SSID: ");
		Serial.print(ssidAP);
		Serial.print("   IP:  ");
		Serial.println(WiFi.localIP());
		//  IPAddress apip = WiFi.softAPIP();
		//	Serial.println(WiFi.softAPIP());
	}

	//start server for settings homepage
	server.begin();
	//UDP starten
	UDPSC.listen(PortSCToAOG);
	UDP.listen(PortFromAOG);


	//RelayToAOG[1] = DataToAOGHeader[0];
	//RelayToAOG[1] = DataToAOGHeader[1];

  //------------------------------------------------------------------------------------------------------------  
  //by WEDER Nov 2018
  //create a task that will be executed in the Core1code() function, with priority 1 and executed on core 0
xTaskCreatePinnedToCore(Core1code, "Core1", 10000, NULL, 1, &Core1, 0);
// Core1code,   // Task function. 
// "Core1",     // name of task. 
// 10000,       // Stack size of task 
// NULL,        // parameter of the task 
// 1,           // priority of the task
// &Core1,      // Task handle to keep track of created task 
// 0);          // pin task to core 0                  
delay(500);

//create a task that will be executed in the Core2code() function, with priority 1 and executed on core 1
xTaskCreatePinnedToCore(Core2code, "Core2", 10000, NULL, 1, &Core2, 1);
delay(500);
//------------------------------------------------------------------------------------------------------------
}  //end setup

void loop() {
	//nothing because code will be run in Core1 and core2
	
	//feed the watchdogtimer to prevent ESP from reboot
	TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	TIMERG0.wdt_feed = 1;
	TIMERG0.wdt_wprotect = 0;
}

//------------------------------------------------------------------------------------------------------------------------------

 //ISR
void pinLeftChangeISR() {
	static unsigned long pulseStart = 0;
	pulseCountLeft++;

	state = !state;
	if (state)
	{
		// interrupt pin has risen, a pulse has started
		pulseStart = micros(); // store the current microseconds
	}
	else
	{    //risen again
		pulseDurationLeft = micros() - pulseStart; // get the pulse length
		pulseStart = 0;
	}
}

void pinRightChangeISR() {
	static unsigned long pulseStart = 0;
	pulseCountRight++;

	state = !state;
	if (state)
	{
		// interrupt pin has risen, a pulse has started
		pulseStart = micros(); // store the current microseconds
	}
	else
	{    //risen again
		pulseDurationRight = micros() - pulseStart; // get the pulse length
		pulseStart = 0;
	}
}
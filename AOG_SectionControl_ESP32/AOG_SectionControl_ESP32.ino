
// Section Control code for AgOpenGPS V5 or later for ESP32

// by MTZ8302 see GitHub https://github.com/mtz8302 and Youtube Ma Ha MTZ8302 https://www.youtube.com/channel/UCv44DlUXQJKbQjzaOUssVgw

// can be used as output only = relais for sections
// or for documentation = input only = input to ESP32 -> enables mapping for active (=switch to GND) section in AOG
// or any combination = section control with input switch
// main and pressure switches are (ON)-0-(ON) toggle switches (GND)- 1.75V - (3,3V). So use 2 resistors GND- 1K - (COM on toggle switch) - 1K - 3.3V
// section switches and auto/manual switch are ON-OFF switches

// connects to existing WiFi network or creates access point. Settings below or in WebInterface at 192.168.1.71 in access point mode Webinterface at 192.168.1.1

// while startup ESP32 sends debug infos to USB at 38400 baud

byte vers_nr = 58;
char VersionTXT[150] = " - 28. Feb 2024 by MTZ8302<br>PCA9555, AgIO Heartbeat, AsyncUDP for WiFi, multiple WiFi networks, Ethernet support";


struct set {
	//User config: ***********************************************************************************

	uint8_t DataTransVia = 10;		//transfer data via 0 = USB / 7 = WiFi / 10 = Ethernet UDP
	uint8_t aogVersion = 20;		// not used at the moment
	uint8_t AgIOHeartbeat_answer = 1;//0: don't send (default)
	uint8_t LEDWiFi_PIN = 2;		// WiFi Status LED 255 = off
	uint8_t LEDWiFi_ON_Level = 1;   // 1 = HIGH = LED on high, 0 = LOW = LED on low

	//WiFi---------------------------------------------------------------------------------------------
	//tractors WiFi or mobile hotspots. Connections are checked in this order
	char ssid1[24] = "Matthias A53";	// WiFi network Client name
	char password1[24] = "aveasillrac";                // WiFi network password//Accesspoint name and password
	char ssid2[24] = "Deutz_6006";			// WiFi network Client name
	char password2[24] = "";                // WiFi network password//Accesspoint name and password
	char ssid3[24] = "Fendt_209V_Tablet";					// WiFi network Client name
	char password3[24] = "Fendt_209V";                // WiFi network password//Accesspoint name and password
	char ssid4[24] = "Fendt_209V";					// WiFi network Client name
	char password4[24] = "";                // WiFi network password//Accesspoint name and password
	char ssid5[24] = "WLANHammer";					// WiFi network Client name
	char password5[24] = "aveasillrac";                // WiFi network password//Accesspoint name and password

	char ssid_ap[24] = "SectionControlNet"; // name of Access point, if no WiFi found, no password!!
	uint8_t timeoutRouter = 20;//s         // time (s) to search for existing WiFi, then starting Accesspoint 
	byte timeoutWebIO = 10;//min  		    // time (min) afterwards webinterface is switched off	
	uint8_t timeoutRouterEth = 255;//s		// time (s) to search for Ethernet cable, then changing datatransfer to WiFi

	//WiFi
	byte WiFi_myip[4] = { 192, 168, 137, 71 };    // Section Control 
	byte WiFi_gwip[4] = { 192, 168, 1, 1 };     // Gateway IP only used if Accesspoint created
	byte WiFi_ipDest_ending = 255;				//ending of IP address to send UDP data to
	byte mask[4] = { 255, 255, 255, 0 };
	byte myDNS[4] = { 8, 8, 8, 8 };				//optional

	//Ethernet
	byte Eth_myip[4] = { 192, 168, 5, 72 };     // Section Control 
	byte Eth_ipDest_ending = 255;				// ending of IP address to send UDP data to
	byte Eth_mac[6] = { 0x90,0xA2,0xDA,0x10,0xB3,0x1B };
	bool Eth_static_IP = false;					// false = use DHPC and set last number to 80 (x.x.x.80) / true = use IP as set above

	uint16_t PortSCToAOG = 5555;			// this is port of this module: Autosteer = 5577 IMU = 5566 Section Control = 5555 GPS = 5544
	uint16_t PortFromAOG = 8888;            // port to listen for AOG
	uint16_t PortDestination = 9999;        // Port of AOG that listens
	uint16_t BaudRate = 38400;              // Baudrate = speed of serial port or USB or Bluetooth. AOG uses 38400 for UART

//the following lines should be configed by the user to fit the programm to the sprayer/ESP32
//GPIOs of the ESP32 (current setting is for the layout shown as example WIKI)

	byte Eth_CS_PIN = 5;// 13;					// CS PIN with SPI Ethernet hardware  SPI config: MOSI 23 / MISO 19 / CLK18 / CS5
	byte Eth_INT_PIN = 255;                 //interupt pin not used at the moment, but needed if async ethernet lib will be used

// if only 1 flowrate is used, use left
//Example1: motor valve is controled only by Switch not by AOG, no Flowmeter, : RateControl..Equiped = false; RateSW..Equiped = true; RateControlPWM = false;
//Example2: PWM valve, with flowmeter all controled by AOG:   RateControl..Equiped = true; RateSW..Equiped = true; RateControlPWM = true;	
	uint8_t RateControlLeftInst = 0;		// 1 if Rate control is there, else: 0
	uint8_t RateSWLeftInst = 1;				// 1 if Rate control Pressure switch is there, else: 0
	uint8_t RateSWLeft_PIN = 39;// 34;			// Rate +/- switch (ON)-0-(ON) toggle switch (GND)- 1.75V - (3,3V)
	uint8_t RateControlPWM = 0;				// 1 if PWM valve, 0 if Motor drive for pressure change		

	uint8_t	FlowDirLeft_PIN = 32;// 23;			// Rate-Control Valve/Motor Direktion
	uint8_t	FlowPWMLeft_PIN = 33;// 22;			// Rate-Control Valve PWM/Motor ON/OFF
	uint8_t	FlowEncALeft_PIN = 255;			// Flowmeter left/1

	uint8_t RateControlRightInst = 0;	    // 1 if Rate control is there, else: 0
	uint8_t RateSWRightInst = 0;			// 1 if Rate control Pressure switch is there, else: 0	
	uint8_t	RateSWRight_PIN = 255;			// Rate +/- switch
	uint8_t	FlowDirRight_PIN = 255;			// 255  = unused Rate-Control Valve Direktion
	uint8_t	FlowPWMRight_PIN = 255;			// 255  = unused Rate-Control Valve PWM
	uint8_t	FlowEncARight_PIN = 255;		// Flowmeter right/2 

	uint8_t SectNum = 16;					// number of sections
	uint8_t SectRelaysInst = 1;				// relays for SC output are equiped (0=no output, only documentation)
	uint8_t SectRelaysON = 1;				// relays spray on 1 or 0 (high or low)
	//Relay_PIN = GPIOs of ESP32 OUT to sections of sprayer, 255 = unused, 100 = pin IO 1 on PCA9555 116 = pin IO 16 on PCA9555
	uint8_t Relay_PIN[16] = { 100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115 };// { 15, 2, 0, 4, 16, 17, 18, 19, 21, 255, 255, 255, 255, 255, 255, 255 };
	uint8_t Relais_MainValve_PIN = 0;// 255;		// PIN for Main fluid valve 255 = unused
	
	uint8_t SectSWInst = 1;					// 1 if section input switches are equiped, else: 0	
	uint8_t SectSWAutoOrOn = 1;				// Section switches spray/auto on 1 = high = used with pullup, 0 = low = pulldown 
	uint8_t SectSW255Stat = 2;				// not present switches (255) send 0 = auto, 1 = on, 2 = off, (comming soon: 3 = undefined) 
	//section switches to GPIOs of ESP32, 255 = unused, 100 = pin IO 1 on PCA9555 116 = pin IO 16 on PCA9555
	uint8_t SectSW_PIN[16] = { 100,101,102,103,255,105,106,107,108,255,110,111,112,113,114,115 };// { 255, 12, 14, 27, 26, 25, 33, 255, 36, 255, 255, 255, 255, 255, 255, 255 };
	uint8_t	SectMainSWType = 1;				// 0 = not equiped 1 = (ON)-OFF-(ON) toggle switch or push buttons 2 = connected to hitch level sensor 3 = inverted hitch level sensor
	uint16_t	HitchLevelVal = 2000;		// Value for hitch level: switch AOG section control to Auto if lower than... ESP:2000 nano 500
	uint8_t	SectMainSW_PIN = 35;// 32;			// ESP32 to AOG Main auto toggle switch open=+1,9V=nothing/AOG button GND=OFF +3,3=AOG Auto on	OR connected to hitch level sensor	
	uint8_t	SectAutoManSW_PIN = 34;// 39;			// Main Auto/Manual switch 39:!!no internal pullup!!

	uint8_t DualFluidSW_PIN = 255;			//not supported at the moment
	uint8_t OnOffDelayAutoManSW_PIN = 255;	//not supported at the moment
	uint8_t OnDelayDist_dm = 30;			//not supported at the moment
	uint8_t OffDelayDist_dm = 35;			//not supported at the moment

	uint8_t DocOnly = 0;					// 0: use as section control, 1: Documentation only = AOG writes the state of the input switches

	uint8_t I2CAddrPCAInput = 0x20;
	uint8_t I2CAddrPCAOutput = 0x21;

	uint8_t resByte0 = 0;
	uint8_t resByte1 = 0;
	uint8_t resByte2 = 0;
	uint8_t resByte3 = 0;

	uint16_t resInt0 = 0;
	uint16_t resInt1 = 0;

	bool debugmode = false;
	bool debugmodeRelay = false;
	bool debugmodeSwitches = false;
	bool debugmodeDataFromAOG = false;
	bool debugmodeDataToAOG = false;

	// END of user config ****************************************************************************
}; set Set;


bool EEPROM_clear = false;



byte SCToAOG[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, SCToAOGOld[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
uint8_t helloFromSC[] = { 128, 129, 123, 123, 5, 0, 0, 0, 0, 0, 71 }; //hello from SC sent back to AgIO

const byte FromAOGSentenceHeader[3] = { 0x80,0x81,0x7F };
#define FromSCHeader 0x7B
#define SCDataToAOGHeader  0xEA
#define SteerDataFromAOGHeader  0xFE  //take section info from steer packet
#define SCSettingsFromRCHeader  0xFC
#define AgIO_heartbeat 0xC8
#define AgIO_ScanRequest 0xCA//AgIO asking for IP -> Ethernet/WiFi GPS sends IP 
#define SCDataSentenceToAOGLength  14

//write incoming Data to sentence array if it fits in
#define SentenceFromAOGMaxLength 14
byte SentenceFromAOG[SentenceFromAOGMaxLength], SentenceFromAOGLength;

//global, as serial/USB may not come at once, so values must stay for next loop
byte incomSentenceDigit = 0,DataToAOGLength;
bool isSteerDataFound = false, isRCSettingFound = false, isAgIOHeartbeatFound = false, isAgIOScanRequestFound = false;

#define incommingDataArraySize 10
byte incommingBytes[incommingDataArraySize][500], incommingBytesArrayNr = 0, incommingBytesArrayNrToParse = 0;
unsigned int incommingDataLength[incommingDataArraySize] = { 0,0,0,0,0 };

//WIFI+Ethernet
IPAddress WiFi_ipDestination, Eth_ipDestination; //set in network.ino
byte Eth_connect_step, WiFi_connect_step = 10, WiFi_netw_nr = 0, WiFi_STA_connect_call_nr = 0, my_WiFi_Mode = 0; // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint
//int pingResult, WiFiWatchDog = 0;
bool WiFiUDPRunning = false, EthUDPRunning = false, task_WiFiConnectRunning = false;
unsigned long WebIOTimeOut = 0, WiFi_network_search_timeout = 0, Eth_network_search_timeout = 0;//PingToNetworkLastTime = 0,
//webpage
long argVal = 0;
bool WebIORunning = false;

// WiFi LED blink times: searching WIFI: blinking 4x faster; connected: blinking as times set; data available: light on; no data for 2 seconds: blinking
unsigned int LED_WIFI_time = 0;
unsigned int LED_WIFI_pulse = 2000;   //light on in ms 
unsigned int LED_WIFI_pause = 1500;   //light off in ms
boolean LED_WIFI_ON = false;
unsigned long timeout, timeout2;



//ToDo: - Rate / flowmeter doesn't work, interrupt mode ect not defined
//      - line 260 PWM + Interrupt mode ect...
//		  - Right side Rate caluclation / output / Motordrive
//      - Manual mode for Rate Control
//		  - what to do if no WIFI??


//libraries -------------------------------
#include "EEPROM.h"
#include "Update.h"
#include <WiFi.h>
#include <AsyncUDP.h>
#include <WebServer.h>
#include <Wire.h>
#include <Ethernet.h>
#include "zAOG_ESP32Ping.h"
#include "zAOG_ping.h"

WebServer WiFi_Server(80);
AsyncUDP WiFiUDPFromAOG;
AsyncUDP WiFiUDPToAOG;
EthernetUDP EthUDPToAOG;
EthernetUDP EthUDPFromAOG;
TaskHandle_t taskHandle_Eth_connect;
TaskHandle_t taskHandle_WiFi_connect;
TaskHandle_t taskHandle_DataFromAOGUSB; bool USBDataTaskRunning = false;
TaskHandle_t taskHandle_DataFromAOGEth; bool EthDataTaskRunning = false;
TaskHandle_t taskHandle_WebIO;
TaskHandle_t taskHandle_LEDBlink;


//values to decide position of section switch
#define SWOFF 650 // analog in is lower than .. for off
#define SWON 3000 // analog in is lower than .. for auto/on


//analog value of the toggle switches (default: middle=unpressed)
int MainSWVal = (SWOFF + SWON) / 2;
int MainSWValOld = (SWOFF + SWON) / 2;
int RateSWLeftVal = (SWOFF + SWON) / 2;
int RateSWRightVal = (SWOFF + SWON) / 2;

//state of the switches HIGH = ON/Auto LOW = OFF	
boolean SectSWVal[16] = { HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH };
boolean AutoSWVal = HIGH;
byte debugmodeSwitchesBak = 3;  //0 = false 1 = true 3 = not used

//loop time variables in microseconds
const unsigned long LOOP_TIME = 500; //in msec; 1000 = 1 Hz
const unsigned long SectSWDelayTime = 200;//time the arduino waits after manual Switch is used before acception command from AOG in msec
unsigned long lastTime = LOOP_TIME;
unsigned long now = LOOP_TIME;
unsigned long DataFromAOGTime = 0;
unsigned long SectAutoSWTime = LOOP_TIME;
unsigned long SectAutoSWlastTime = LOOP_TIME;
unsigned long SectMainSWlastTime = LOOP_TIME;
unsigned long RateSWlastTimeLeft = LOOP_TIME;
unsigned long RateSWDelayTime = 250; //time to pass before a new toggle of switch is accepted = if hold down, time between steps
unsigned long RateSWlastTimeRight = LOOP_TIME;
unsigned long SectSWcurrentTime = LOOP_TIME;


/* no rate control in AOG
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
*/

//program flow
//bool NewDataFromAOG = false, NewSettingFromAOG = false;

//bit 0 on byte[0] is section 1
byte SectGrFromAOG[] = { 0, 0 }; //Sections ON / OFF Bitwise !!! 
byte RelayOUT[] = { 0,0 }; //Sections ON / OFF Bitwise !!! 
byte RelayOUTOld[] = { 0,0 };
byte SectSWOffToAOG[] = { 0,0 };
byte SectMainToAOG = 0;
byte uTurnRelay = 0;
//  byte flowRateSW1 = 0;
boolean PCAinUse = false;
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

boolean newDataToAOG = false;// newDataFromAOG = false;

float gpsSpeed = 0; //speed from AgOpenGPS
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
{	delay(100);//wait for power to stabilize
	delay(100);//wait for IO chips to get ready
	//set up communication
	Serial.begin(Set.BaudRate);
	delay(10);
	delay(50);
	Serial.println();
	//get EEPROM data
	restoreEEprom();
	delay(100);

	SCToAOG[0] = FromAOGSentenceHeader[0];   //0x80
	SCToAOG[1] = FromAOGSentenceHeader[1];   //0x81
	SCToAOG[2] = FromSCHeader;				 //0x7B
	SCToAOG[3] = SCDataToAOGHeader;			 //0xEA
	SCToAOG[4] = SCDataSentenceToAOGLength - 6; //length of data = all - header - length - CRC
	SCToAOGOld[0] = FromAOGSentenceHeader[0];
	SCToAOGOld[1] = FromAOGSentenceHeader[1];
	SCToAOGOld[2] = FromSCHeader;
	SCToAOGOld[3] = SCDataToAOGHeader;
	SCToAOG[4] = SCDataSentenceToAOGLength - 6;
	DataToAOGLength = SCDataSentenceToAOGLength;
	incomSentenceDigit = 0;

	Serial.println();
	Serial.print("ESP code Version Nr ");
	Serial.print(vers_nr);
	Serial.println(VersionTXT);
	Serial.println();

	//set GPIOs
	assignGPIOs();
	delay(50);

	//switches -> set relais
	if ((Set.SectSWInst) || (Set.SectMainSWType != 0)) {
		SectSWRead();
	}
	SetRelays();

	//Rate switches and motor drive
	if ((Set.RateSWLeftInst == 1) || (Set.RateSWRightInst == 1)) { RateSWRead(); }
	if (Set.RateControlLeftInst == 0) { motorDrive(); } //if Manual do everytime, not only in timed loop

	//start Ethernet
	if (Set.DataTransVia == 10) {
		Eth_connect_step = 10;
		xTaskCreate(Eth_handle_connection, "EthConnectHandle", 3072, NULL, 1, &taskHandle_Eth_connect);
		delay(500);
	}
	else { Eth_connect_step = 255; }
	
	WiFi_connect_step = 10;//step 10 = begin of starting a WiFi connection
	//start WiFi
	xTaskCreate(WiFi_handle_connection, "WiFiConnectHandle", 3072, NULL, 1, &taskHandle_WiFi_connect);
	delay(500);

	//get Data via USB;   WiFi not needed as AsyncUDP, Ethernet started in Ethernet connection task
	if (Set.DataTransVia < 5) {//USB
		xTaskCreate(getDataFromAOGUSB, "DataFromAOGHandleUSB", 5000, NULL, 1, &taskHandle_DataFromAOGUSB);
	}
	delay(500);

	//handle WiFi LED status
	xTaskCreate(WiFi_LED_blink_and_Connection_check, "WiFiLEDBlink", 3072, NULL, 0, &taskHandle_LEDBlink);
	delay(500);

	vTaskDelay(500); //waiting for other tasks to start

	if (Set.DataTransVia > 5) {//waiting for other tasks to start (6 s) and read switches at 4 Hz
		for (byte i = 0; i < 24; i++) {
			//switches -> set relais
			if ((Set.SectSWInst) || (Set.SectMainSWType != 0)) { SectSWRead(); }
			SetRelays();

			//Rate switches and motor drive
			if ((Set.RateSWLeftInst == 1) || (Set.RateSWRightInst == 1)) { RateSWRead(); }
			if (Set.RateControlLeftInst == 0) { motorDrive(); } //if Manual do everytime, not only in timed loop

			vTaskDelay(250);
		}
	}

}  //end setup


//-------------------------------------------------------------------------------------------------

void loop() {

	//new data from AOG? Data comes via extra task and is written into byte array. Parsing called here
	for (;;) {
		if (incommingBytesArrayNr == incommingBytesArrayNrToParse) {
			vTaskDelay(4);//3 wait if no new data to give time to other tasks 
			break;
		}
		else { parseDataFromAOG(); }
	}

	//read switches/inputs
	if ((Set.SectSWInst) || (Set.SectMainSWType != 0))
	{
		SectSWRead(); //checks if section switch is toggled and sets RelayOUT

		//reset debugSwitches: has been change to true, if debugmodeDataToAOG = true for 1 loop
		if ((Set.debugmodeSwitches) && (debugmodeSwitchesBak == 0)) { Set.debugmodeSwitches = false; }

		SCToAOG[10] |= SectSWOffToAOG[0]; //LowByte if bit set -> AOG section forced off
		SCToAOG[12] |= SectSWOffToAOG[1]; //HiByte if bit set -> AOG section forced off
		SCToAOG[5] = SectMainToAOG; // Bits: AOG section control AUTO, Section control OFF, Rate L+R ...
		//new data?
		for (byte idx = 5; idx < DataToAOGLength; idx++) {
			if (SCToAOG[idx] != SCToAOGOld[idx]) {
				newDataToAOG = true;
				SCToAOGOld[idx] = SCToAOG[idx];
			}
		}
	}
	//no switches: set Relais as AOG commands
	else { RelayOUT[0] = SectGrFromAOG[0]; RelayOUT[1] = SectGrFromAOG[1]; }

	SetRelays();


	//Rate switches and motor drive
	if ((Set.RateSWLeftInst == 1) || (Set.RateSWRightInst == 1)) { RateSWRead(); }

	if (Set.RateControlLeftInst == 0) { motorDrive(); } //if Manual do everytime, not only in timed loop


//timed loop: runs with 1Hz or if new data from switches to send data to AOG
//must run, also if nothing changed on switches, to keep GUI syncron (important if a section switch is off)
	now = millis();
	if ((now > LOOP_TIME + lastTime) || (newDataToAOG)) {
		lastTime = now;

		//add the checksum
		int CRCtoAOG = 0;
		for (byte i = 2; i < sizeof(SCToAOG) - 1; i++)
		{
			CRCtoAOG = (CRCtoAOG + SCToAOG[i]);
		}
		SCToAOG[sizeof(SCToAOG) - 1] = CRCtoAOG;

		AOGDataSend();
		if (newDataToAOG) {
			vTaskDelay(5);
			newDataToAOG = false;
			AOGDataSend(); //send 2. time for safety
		}

		if (Set.debugmodeDataToAOG) {
			Serial.print("Data to AOG ");
			for (byte n = 0; n < DataToAOGLength; n++) {
				Serial.print(SCToAOG[n]); Serial.print(" ");
			}
			Serial.println();
			//printing swich input 1x:
			if (!Set.debugmodeSwitches) {
				debugmodeSwitchesBak = 0;
				Set.debugmodeSwitches = true;
			}
			else debugmodeSwitchesBak = 1;
		}

	}
}

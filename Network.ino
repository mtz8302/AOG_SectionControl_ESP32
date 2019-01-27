// code in this TAB based mainly on WEder = GitHub/coffeetrac

/*

#include <WiFi.h>
#include <AsyncUDP.h>

#include <WiFiAP.h>
#include <WiFiClient.h>

// Wifi variables & definitions

#define MAX_PACKAGE_SIZE 2048
char HTML_String[7000];
char HTTP_Header[160];

byte my_WiFi_Mode = 0;  // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint
						//---------------------------------------------------------------------
String _userAgent = "NTRIP CoffeetracNTRIPClient";

// program flow
bool AP_running = 0, EE_done = 0, restart = 0;
int value = 0;
unsigned long repeat_ser;

//int error = 0;
unsigned long repeatGGA, lifesign;

// Allgemeine Variablen
String _base64Authorization;
String _accept = "*/        //*";
/*
#define ACTION_SET_SSID        1  
#define ACTION_SET_AOGIP	   2
#define ACTION_SET_SENDPOS     3
#define ACTION_SET_RESTART     4
#define ACTION_SET_GGA         5
#define ACTION_SET_NMEAOUT     6
#define ACTION_SET_AHRS        7

int action;

// Radiobutton Select your Position type
char position_type[3][26] = { "Position Off", "Position Fixed via String", "GGA Position from GPS" };

// Radiobutton Select the time between repetitions.
char repeatPos[3][8] = { "1 sec.", "5 sec.", "10 sec." };

// Radiobutton Baudrate of serial1
char baud_output[6][7] = { "  9600", " 14400", " 19200", " 38400", " 57600", "115200" };

// Radiobutton Select if NMEA are transmitted via UDP.
char sendNmea[3][10] = { "OFF", "UDP-AOG", "Bluetooth" };

// Radiobutton Select if NTRIP Client is enabled. (Off to use only NMEA Transmission to AOG)
char ntripOn_type[2][4] = { "OFF", "ON" };

char AHRS_tab[2][21] = { "IMU BNO055", "MMA8452 Inclinometer" };
//---------------------------------------------------------------------








void SettingsPage() {

	char my_char;
	int htmlPtr = 0;
	int myIdx;
	int myIndex;
	unsigned long my_timeout;


	// Check if a client has connected
	client_page = server.available();
	if (client_page.connected()) { Serial.print(" NEW Client   "); delay(2000); }
	if (!client_page) 
	{
		Serial.print("nc"); return;
	}

	Serial.print("New Client:\n");           // print a message out the serial port
	delay(2000);

	my_timeout = millis() + 250L;
	while (!client_page.available() && (millis() < my_timeout)) delay(10);
	delay(10);
	if (millis() > my_timeout)
	{
		Serial.print("Client connection timeout!\n");
		return;
	}
	//---------------------------------------------------------------------
	//htmlPtr = 0;
	char c;
	if (client_page) {                        // if you get a client,
											  //Serial.print("New Client.\n");                   // print a message out the serial port
		String currentLine = "";                // make a String to hold incoming data from the client
		while (client_page.connected()) {       // loop while the client's connected
			if (client_page.available()) {        // if there's bytes to read from the client,
				char c = client_page.read();        // read a byte, then
				Serial.print(c);                             // print it out the serial monitor
				if (c == '\n') {                    // if the byte is a newline character

													// if the current line is blank, you got two newline characters in a row.
													// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {

						make_HTML01();  // create Page array
										//---------------------------------------------------------------------
										// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
										// and a content-type so the client knows what's coming, then a blank line:
						strcpy(HTTP_Header, "HTTP/1.1 200 OK\r\n");
						strcat(HTTP_Header, "Content-Length: ");
						strcati(HTTP_Header, strlen(HTML_String));
						strcat(HTTP_Header, "\r\n");
						strcat(HTTP_Header, "Content-Type: text/html\r\n");
						strcat(HTTP_Header, "Connection: close\r\n");
						strcat(HTTP_Header, "\r\n");

						client_page.print(HTTP_Header);
						delay(20);
						send_HTML();

						// break out of the while loop:
						break;
					}
					else {    // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				}
				else if (c != '\r')
				{ // if you got anything else but a carriage return character,
					currentLine += c;      // add it to the end of the currentLine
					if (currentLine.endsWith("HTTP"))
					{
						if (currentLine.startsWith("GET "))
						{
							currentLine.toCharArray(HTML_String, currentLine.length());
							Serial.print(""); //NL
							exhibit("Request : ", HTML_String);
							process_Request();
						}
					}
				}//end else
			} //end client available
		} //end while client.connected
		  // close the connection:
		client_page.stop();
		Serial.print("Pagelength : ");
		Serial.print((long)strlen(HTML_String));
		Serial.print("   --> Client Disconnected\n");
	}// end if client 
}


//---------------------------------------------------------------------
// Process given values
//---------------------------------------------------------------------
void process_Request()
{
	int myIndex;

	if (Find_Start("/?", HTML_String) < 0 && Find_Start("GET / HTTP", HTML_String) < 0)
	{
		//nothing to process
		return;
	}
	action = Pick_Parameter_Zahl("ACTION=", HTML_String);

	// WiFi access data
	if (action == ACTION_SET_SSID) {

		myIndex = Find_End("SSID_MY=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<24; i++) SCSet.ssid[i] = 0x00;
			Pick_Text(SCSet.ssid, &HTML_String[myIndex], 24);
			exhibit("SSID  : ", SCSet.ssid);
		}
		myIndex = Find_End("Password_MY=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<24; i++) SCSet.password[i] = 0x00;
			Pick_Text(SCSet.password, &HTML_String[myIndex], 24);
			exhibit("Password  : ", SCSet.password);
			//EEprom_write_all();
		}
	}


	if (action == ACTION_SET_AOGIP) {
		myIndex = Find_End("AOGIP=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<40; i++) SCSet.host[i] = 0x00;
			Pick_Text(SCSet.host, &HTML_String[myIndex], 40);
			exhibit("Caster:   ", SCSet.host);
		}

		SCSet.port = Pick_Parameter_Zahl("CASTERPORT=", HTML_String);
		exhibit("Port  : ", SCSet.port);

		myIndex = Find_End("MOUNTPOINT=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<40; i++) SCSet.mountpoint[i] = 0x00;
			Pick_Text(SCSet.mountpoint, &HTML_String[myIndex], 40);
			exhibit("Caster:   ", SCSet.mountpoint);
		}

		myIndex = Find_End("CASTERUSER=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<40; i++) SCSet.ntripUser[i] = 0x00;
			Pick_Text(SCSet.ntripUser, &HTML_String[myIndex], 40);
			exhibit("Username : ", SCSet.ntripUser);
		}
		myIndex = Find_End("CASTERPWD=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<40; i++) SCSet.ntripPassword[i] = 0x00;
			Pick_Text(SCSet.ntripPassword, &HTML_String[myIndex], 40);
			exhibit("Password  : ", SCSet.ntripPassword);
		//	EEprom_write_all();
		//	connectCaster(); //reconnect Caster + calc _Auth
		}
	}

	if (action == ACTION_SET_SENDPOS) {
		SCSet.sendGGAsentence = Pick_Parameter_Zahl("POSITION_TYPE=", HTML_String);
		if (Pick_Parameter_Zahl("REPEATTIME=", HTML_String) == 0) SCSet.GGAfreq = 1;
		if (Pick_Parameter_Zahl("REPEATTIME=", HTML_String) == 1) SCSet.GGAfreq = 5;
		if (Pick_Parameter_Zahl("REPEATTIME=", HTML_String) == 2) SCSet.GGAfreq = 10;

		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 0) SCSet.baudOut = 9600;
		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 1) SCSet.baudOut = 14400;
		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 2) SCSet.baudOut = 19200;
		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 3) SCSet.baudOut = 38400;
		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 4) SCSet.baudOut = 57600;
		if (Pick_Parameter_Zahl("BAUDRATESET=", HTML_String) == 5) SCSet.baudOut = 115200;
//		Serial.flush(); // wait for last transmitted data to be sent 
//		Serial1.flush(); // wait for last transmitted data to be sent 
//		Serial1.begin(SCSet.baudOut, SERIAL_8N1, RX1, TX1);  //set new Baudrate
		Serial.print("\nRTCM/NMEA Baudrate: ");
		Serial.print(SCSet.baudOut);
		//EEprom_write_all();
	}
	if (action == ACTION_SET_RESTART) {
		//if (EEPROM.read(2) == 0) {
		//	EEPROM.write(2, 1);
		//	EEPROM.commit();
		//	delay(2000);
		//	ESP.restart();
	//	}
	}

	if (action == ACTION_SET_GGA) {
		myIndex = Find_End("GGA_MY=", HTML_String);
		if (myIndex >= 0) {
			for (int i = 0; i<100; i++) SCSet.GGAsentence[i] = 0x00;
			Pick_Text(SCSet.GGAsentence, &HTML_String[myIndex], 100);
			exhibit("NMEA: ", SCSet.GGAsentence);
		}
		//EEprom_write_all();
	}

	if (action == ACTION_SET_NMEAOUT) {
		SCSet.send_UDP_AOG = Pick_Parameter_Zahl("SENDNMEA_TYPE=", HTML_String);
		byte old = SCSet.enableNtrip;
		SCSet.enableNtrip = Pick_Parameter_Zahl("ENABLENTRIP=", HTML_String);
		if (SCSet.enableNtrip == 1 && old == 0) restart == 0; // 
	//	EEprom_write_all();
	}

	if (action == ACTION_SET_AHRS) {

		SCSet.AHRSbyte = 0;
		char tmp_string[20];
		for (int i = 0; i < 2; i++) {
			strcpy(tmp_string, "AHRS_TAG");
			strcati(tmp_string, i);
			strcat(tmp_string, "=");
			if (Pick_Parameter_Zahl(tmp_string, HTML_String) == 1)SCSet.AHRSbyte |= 1 << i;
		}

	//	EEprom_write_all();

	}
}


//---------------------------------------------------------------------
// create HTML site 01
//---------------------------------------------------------------------
void make_HTML01() {

  strcpy( HTML_String, "<!DOCTYPE html>");
  strcat( HTML_String, "<html>");
  strcat( HTML_String, "<head>");
  strcat( HTML_String, "<title>AG NTRIP Client Config Page</title>");
  strcat( HTML_String, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0;\" />\r\n");
  //strcat( HTML_String, "<meta http-equiv=\"refresh\" content=\"10\">");
  strcat( HTML_String, "<style>divbox {background-color: lightgrey;width: 200px;border: 5px solid red;padding:10px;margin: 10px;}</style>");
  strcat( HTML_String, "</head>");
  strcat( HTML_String, "<body bgcolor=\"#ff9900\">");
  strcat( HTML_String, "<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">");
  strcat( HTML_String, "<h1>AG NTRIP-Client ESP</h1>");

  //-----------------------------------------------------------------------------------------
  // WiFi Client Access Data
  strcat( HTML_String, "(Rev. 1.1 by WEder and MTZ8302)<br>");
  strcat( HTML_String, "<hr><h2>WiFi Network Client Access Data</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "This Section Control Client requires access to your AgOpenGPS computer via Network !!<br><br>");
  strcat( HTML_String, "If access fails, an accesspoint will be created<br>");
  strcat( HTML_String, "SectionControl_AP no password<br><br>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Address:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"SSID_MY\" maxlength=\"22\" Value =\"");
  strcat( HTML_String, SCSet.ssid);
  strcat( HTML_String, "\"></td>");
  
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_SSID);
  strcat( HTML_String, "\">Submit</button></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Password:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
  strcat( HTML_String, SCSet.password);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");
  strcat( HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
  strcat( HTML_String, "<tr><td colspan=\"2\"><b>Restart Section Control client for changes to take effect</b></td>");
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_RESTART);
  strcat( HTML_String, "\">Restart</button></td>");
  strcat( HTML_String, "</tr>");
  
  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");

//-------------------------------------------------------------  
// AOG network
  strcat( HTML_String, "<h2>AgOpenGPS Network Settings</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);
  
  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>IP of AgOpenGPS Computer (should be xxx.xxx.xxx.255 for UDP broadcast)</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type = "number" min = "1" max = "255" style= \"width:80px\" name=\"AOGIP0\" maxlength=\"3\" Value =\"");  
  strcat( HTML_String, SCSet.AOGIP[0].ToString());
  strcat( HTML_String, "<input type = "number" min = "1" max = "255" style= \"width:80px\" name=\"AOGIP1\" maxlength=\"3\" Value =\"");  
  strcat(HTML_String, SCSet.AOGIP[1]);
  strcat( HTML_String, "<input type = "number" min = "1" max = "255" style= \"width:80px\" name=\"AOGIP2\" maxlength=\"3\" Value =\"");
  strcat(HTML_String, SCSet.AOGIP[2]);
  strcat( HTML_String, "\"></td>");
  
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_AOGIP);
  strcat( HTML_String, "\">Submit</button></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Port:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"CASTERPORT\" maxlength=\"4\" Value =\"");
  strcati( HTML_String, SCSet.port);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");
  
  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Mountpoint:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"MOUNTPOINT\" maxlength=\"40\" Value =\"");
  strcat( HTML_String, SCSet.mountpoint);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Username:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"CASTERUSER\" maxlength=\"40\" Value =\"");
  strcat( HTML_String, SCSet.ntripUser);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");
  
  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Password:</b></td>");
  strcat( HTML_String, "<td>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:200px\" name=\"CASTERPWD\" maxlength=\"40\" Value =\"");
  strcat( HTML_String, SCSet.ntripPassword);
  strcat( HTML_String, "\"></td>");
  strcat( HTML_String, "</tr>");
  strcat( HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
  strcat( HTML_String, "<tr><td colspan=\"2\"><b>Restart NTRIP client for changes to take effect</b></td>");
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_RESTART);
  strcat( HTML_String, "\">Restart</button></td>");
  strcat( HTML_String, "</tr>");
  
  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");
//-------------------------------------------------------------  
// GGA processing
  strcat( HTML_String, "<h2>Send my Position</h2>");
  strcat( HTML_String, "(Required if your Caster provides VRS (Virtual Reference Station)<br>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<br>");
  for (int i = 0; i < 3; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0)  strcat( HTML_String, "<td><b>Select Mode</b></td>");
    else strcat( HTML_String, "<td> </td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"POSITION_TYPE\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if (SCSet.sendGGAsentence == i)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, position_type[i]);
    strcat( HTML_String, "</label></td>");
    if (i == 0){
      strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
      strcati(HTML_String, ACTION_SET_SENDPOS);
      strcat( HTML_String, "\">Submit</button></td>");
      strcat( HTML_String, "</tr>");
     }
  }
  strcat( HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
  
  
  for (int i = 0; i < 3; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0) {
      //strcat( HTML_String, "<td colspan=\"6\">&nbsp;</td><br>");
      strcat( HTML_String, "<td><b>Repeat time</b></td>");
     }
    else strcat( HTML_String, "<td>&nbsp;</td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"REPEATTIME\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if (SCSet.GGAfreq == 1 && i==0)strcat( HTML_String, " CHECKED");
    if (SCSet.GGAfreq == 5 && i==1)strcat( HTML_String, " CHECKED");
    if (SCSet.GGAfreq == 10 && i==2)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, repeatPos[i]);
    strcat( HTML_String, "</label></td>");
  }
  strcat( HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
  
  for (int i = 0; i < 6; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0) strcat( HTML_String, "<td><b>Baudrate</b></td>");
    else strcat( HTML_String, "<td> </td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"BAUDRATESET\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if ((SCSet.baudOut == 9600)   && i==0)strcat( HTML_String, " CHECKED");
    if ((SCSet.baudOut == 14400)  && i==1)strcat( HTML_String, " CHECKED");
    if ((SCSet.baudOut == 19200)  && i==2)strcat( HTML_String, " CHECKED");
    if ((SCSet.baudOut == 38400)  && i==3)strcat( HTML_String, " CHECKED");
    if ((SCSet.baudOut == 57600)  && i==4)strcat( HTML_String, " CHECKED");
    if ((SCSet.baudOut == 115200) && i==5)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, baud_output[i]);
    strcat( HTML_String, "</label></td>");
  }
  
  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");

//-------------------------------------------------------------  
// NMEA Sentence
  strcat( HTML_String, "<form>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<h2>Fixed GGA or RMC Sentence of your Location:</h2>");
  strcat( HTML_String, "You can create your own from  <a href=\"https://www.nmeagen.org/\" target=\"_blank\">www.nmeagen.org</a><br><br>");
  strcat( HTML_String, "<input type=\"text\" style= \"width:650px\" name=\"GGA_MY\" maxlength=\"100\" Value =\"");
  strcat( HTML_String, SCSet.GGAsentence);
  strcat( HTML_String, "\"><br><br>");
  
  strcat( HTML_String, "<button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_GGA);
  strcat( HTML_String, "\">Submit</button>");
  strcat( HTML_String, "</tr>");

  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");
//-------------------------------------------------------------  
// NMEA transmission
  strcat( HTML_String, "<h2>Component Config</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  for (int i = 0; i < 2; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0)  strcat( HTML_String, "<td><b>NTRIP Client</b></td>");
    else strcat( HTML_String, "<td> </td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"ENABLENTRIP\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if (SCSet.enableNtrip == i)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, ntripOn_type[i]);
    strcat( HTML_String, "</label></td>");
    if (i == 0){
      strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
      strcati(HTML_String, ACTION_SET_NMEAOUT);
      strcat( HTML_String, "\">Submit</button></td>");
      strcat( HTML_String, "</tr>");
     } 
  }
  strcat( HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

  for (int i = 0; i < 3; i++) {
    strcat( HTML_String, "<tr>");
    if (i == 0)  strcat( HTML_String, "<td><b>Transmission Mode</b></td>");
    else strcat( HTML_String, "<td> </td>");
    strcat( HTML_String, "<td><input type = \"radio\" name=\"SENDNMEA_TYPE\" id=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value=\"");
    strcati( HTML_String, i);
    strcat( HTML_String, "\"");
    if (SCSet.send_UDP_AOG == i)strcat( HTML_String, " CHECKED");
    strcat( HTML_String, "><label for=\"JZ");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, sendNmea[i]);
    strcat( HTML_String, "</label></td>");
    
  }
  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");
 
  //-------------------------------------------------------------  
  // Checkboxes AHRS
  strcat( HTML_String, "<h2>Config AHRS Functions</h2>");
  strcat( HTML_String, "<form>");
  strcat( HTML_String, "<table>");
  set_colgroup(150, 270, 150, 0, 0);

  strcat( HTML_String, "<tr>");
  strcat( HTML_String, "<td><b>Select installed</b></td>");
  strcat( HTML_String, "<td>");
  for (int i = 0; i < 2; i++) {
    if (i == 1)strcat( HTML_String, "<br>");
    strcat( HTML_String, "<input type=\"checkbox\" name=\"AHRS_TAG");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" id = \"Part");
    strcati( HTML_String, i);
    strcat( HTML_String, "\" value = \"1\" ");
    if (SCSet.AHRSbyte & 1 << i) strcat( HTML_String, "checked ");
    strcat( HTML_String, "> ");
    strcat( HTML_String, "<label for =\"Part");
    strcati( HTML_String, i);
    strcat( HTML_String, "\">");
    strcat( HTML_String, AHRS_tab[i]);
    strcat( HTML_String, "</label>");
  }
  strcat( HTML_String, "</td>");
  strcat( HTML_String, "<td><button style= \"width:100px\" name=\"ACTION\" value=\"");
  strcati(HTML_String, ACTION_SET_AHRS);
  strcat( HTML_String, "\">Submit</button></td>");
  strcat( HTML_String, "</tr>"); 
 
  strcat( HTML_String, "</table>");
  strcat( HTML_String, "</form>");
  strcat( HTML_String, "<br><hr>");

  
//-------------------------------------------------------------  
  strcat( HTML_String, "</font>");
  strcat( HTML_String, "</font>");
  strcat( HTML_String, "</body>");
  strcat( HTML_String, "</html>");
}


//--------------------------------------------------------------------------
void send_not_found() {

	Serial.print("\nSend Not Found\n");

	client_page.print("HTTP/1.1 404 Not Found\r\n\r\n");
	delay(20);
	//client_page.stop();
}

//--------------------------------------------------------------------------
void send_HTML() {
	char my_char;
	int  my_len = strlen(HTML_String);
	int  my_ptr = 0;
	int  my_send = 0;

	//--------------------------------------------------------------------------
	// in Portionen senden
	while ((my_len - my_send) > 0) {
		my_send = my_ptr + MAX_PACKAGE_SIZE;
		if (my_send > my_len) {
			client_page.print(&HTML_String[my_ptr]);
			delay(22);

			//Serial.println(&HTML_String[my_ptr]);

			my_send = my_len;
		}
		else {
			my_char = HTML_String[my_send];
			// Auf Anfang eines Tags positionieren
			while (my_char != '<') my_char = HTML_String[--my_send];
			HTML_String[my_send] = 0;
			client_page.print(&HTML_String[my_ptr]);
			delay(22);

			//Serial.println(&HTML_String[my_ptr]);

			HTML_String[my_send] = my_char;
			my_ptr = my_send;
		}
	}
	//client_page.stop();
}

//----------------------------------------------------------------------------------------------
void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
	strcat(HTML_String, "<colgroup>");
	set_colgroup1(w1);
	set_colgroup1(w2);
	set_colgroup1(w3);
	set_colgroup1(w4);
	set_colgroup1(w5);
	strcat(HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
	if (ww == 0) return;
	strcat(HTML_String, "<col width=\"");
	strcati(HTML_String, ww);
	strcat(HTML_String, "\">");
}


//---------------------------------------------------------------------
void strcatf(char* tx, float f) {
	char tmp[8];

	dtostrf(f, 6, 2, tmp);
	strcat(tx, tmp);
}
//---------------------------------------------------------------------
//void strcatl(char* tx, long l) {
//char tmp[sizeof l];
//memcpy(tmp, l, sizeof l);
//strcat (tx, tmp);
//}

//---------------------------------------------------------------------
void strcati(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
void strcati2(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	if (strlen(tmp) < 2) strcat(tx, "0");
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
int Pick_Parameter_Zahl(const char * par, char * str) {
	int myIdx = Find_End(par, str);

	if (myIdx >= 0) return  Pick_Dec(str, myIdx);
	else return -1;
}
//---------------------------------------------------------------------
int Find_End(const char * such, const char * str) {
	int tmp = Find_Start(such, str);
	if (tmp >= 0)tmp += strlen(such);
	return tmp;
}

//---------------------------------------------------------------------
int Find_Start(const char * such, const char * str) {
	int tmp = -1;
	int ww = strlen(str) - strlen(such);
	int ll = strlen(such);

	for (int i = 0; i <= ww && tmp == -1; i++) {
		if (strncmp(such, &str[i], ll) == 0) tmp = i;
	}
	return tmp;
}
//---------------------------------------------------------------------
int Pick_Dec(const char * tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && (tx[p] >= '0' && tx[p] <= '9'); p++) {
		tmp = 10 * tmp + tx[p] - '0';
	}
	return tmp;
}
//----------------------------------------------------------------------------
int Pick_N_Zahl(const char * tx, char separator, byte n) {

	int ll = strlen(tx);
	int tmp = -1;
	byte anz = 1;
	byte i = 0;
	while (i < ll && anz < n) {
		if (tx[i] == separator)anz++;
		i++;
	}
	if (i < ll) return Pick_Dec(tx, i);
	else return -1;
}

//---------------------------------------------------------------------
int Pick_Hex(const char * tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && ((tx[p] >= '0' && tx[p] <= '9') || (tx[p] >= 'A' && tx[p] <= 'F')); p++) {
		if (tx[p] <= '9')tmp = 16 * tmp + tx[p] - '0';
		else tmp = 16 * tmp + tx[p] - 55;
	}

	return tmp;
}

//---------------------------------------------------------------------
void Pick_Text(char * tx_ziel, char  * tx_quelle, int max_ziel) {

	int p_ziel = 0;
	int p_quelle = 0;
	int len_quelle = strlen(tx_quelle);

	while (p_ziel < max_ziel && p_quelle < len_quelle && tx_quelle[p_quelle] && tx_quelle[p_quelle] != ' ' && tx_quelle[p_quelle] != '&') {
		if (tx_quelle[p_quelle] == '%') {
			tx_ziel[p_ziel] = (HexChar_to_NumChar(tx_quelle[p_quelle + 1]) << 4) + HexChar_to_NumChar(tx_quelle[p_quelle + 2]);
			p_quelle += 2;
		}
		else if (tx_quelle[p_quelle] == '+') {
			tx_ziel[p_ziel] = ' ';
		}
		else {
			tx_ziel[p_ziel] = tx_quelle[p_quelle];
		}
		p_ziel++;
		p_quelle++;
	}

	tx_ziel[p_ziel] = 0;
}
//---------------------------------------------------------------------
char HexChar_to_NumChar(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 55;
	return 0;
}
//---------------------------------------------------------------------
void exhibit(const char * tx, int v) {
	Serial.print(tx);
	Serial.print(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned int v) {
	Serial.print(tx);
	Serial.print((int)v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned long v) {
	Serial.print(tx);
	Serial.print((long)v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, const char * v) {
	Serial.print(tx);
	Serial.print(v);
}


*/
#pragma once
#include <U8g2lib.h>

class Oled {
private:
	String Cloud;
	String LastMessage;
	String LastTemp;
	String LastHumid;
	bool LampState;
	U8G2_SSD1306_128X64_NONAME_F_SW_I2C *display;
public:
	Oled() {
		display = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
		display->begin();
	}
	~Oled() {
		delete display;
	}
	void setLampState(bool setting) {
		LampState = setting;
	}
	void setCurrentCloud(String cloud) {
		this->Cloud = cloud;
	}
	void setMessage(String msg) {
		LastMessage = msg;
	}
	void WelcomeScreen() {
		display->setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf);
		display->firstPage();
		do {
			display->drawStr(22, 20, "Welcome");
			display->setFont(u8g2_font_crox1hb_tf);
			display->drawStr(11, 45, "Cloud 2 Device");
		} while (display->nextPage());
	}
	void ConnectingWifi(int dots, bool ifdots = false) {
		display->setFont(u8g2_font_crox1hb_tf);
		display->firstPage();
		do {
			display->drawStr(4, 20, "Connecting to WiFi..");
			if (ifdots) {
				display->setFont(u8g2_font_lucasarts_scumm_subtitle_o_tf);
				for (int i = 0; i < dots; i++) {
					display->drawStr(7 + (i * 5), 40, ".");
				}
			}
		} while (display->nextPage());
	}
	void wifiConnected(const char * ssid, const char * ip) {
		display->setFont(u8g2_font_crox1hb_tf);
		display->firstPage();
		do {
			display->drawStr(10, 15, "Connected to:");
			display->drawStr(10, 30, ssid);
			display->drawStr(10, 45, ip);
		} while (display->nextPage());
	}
	void UpdateData(const char * Temp, const char* Humid) {
		String writeTemp, writeHumid;
		writeTemp += "Temp: " + (String)Temp + " *C";
		writeHumid += "Humid: " + (String)Humid + " %";
		LastTemp = writeTemp;
		LastHumid = writeHumid;
	}
	void UpdateDisplay() {
		String LedText = "LED: ";
		if (LampState)
			LedText += "ON";
		else
			LedText += "OFF";
		String MsgText = "C2D Msg: " + LastMessage;

		display->setFont(u8g2_font_crox1hb_tf);
		display->firstPage();
		do {
			display->drawStr(10, 15, LastTemp.c_str());
			display->drawStr(10, 30, LastHumid.c_str());
			display->drawStr(10, 45, LedText.c_str());
			display->drawStr(10, 60, MsgText.c_str());
			display->drawStr(60, 45, Cloud.c_str());
			display->drawLine(5, 3, 123, 3);
			display->drawLine(5, 63, 123, 63);
		} while (display->nextPage());
	}
};
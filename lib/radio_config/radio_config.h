#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <ArduinoNvs.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Fsm.h>

//Declare LCD
extern hd44780_I2Cexp lcd;
constexpr int LCD_COLS = 16;
constexpr int LCD_ROWS = 2;

//Declare common FSM states and entries
void disp_mode_on_enter();
void reboot_on_enter();
extern State state_disp_mode;
extern State state_reboot;



//Define mode names
constexpr const char* CONF_MODE = "conf_mode";
constexpr const char* A2DP_MODE = "A2DP_mode";
constexpr const char* MENU_MODE = "menu_mode";
constexpr const char* WIFI_MODE = "wifi_mode";

//Declare BT name
constexpr const char* BT_NAME = "ESP32_BT";

//WiFi Stuff station list
/*
constexpr char *stations[][2];
constexpr int NUM_STATIONS;
constexpr char* ssid;
constexpr char* password;
*/

//Buttons
constexpr int SCROLL_TRIG = 1;
constexpr int SCROLL_PIN = 14;
constexpr int ENTER_TRIG = 2;
constexpr int ENTER_PIN = 13;

//define pins
constexpr int I2S_BCLK = 19;
constexpr int I2S_DOUT = 18;
constexpr int I2S_LRC = 5;
constexpr int I2C_SDA = 32;
constexpr int I2C_SCL = 33;
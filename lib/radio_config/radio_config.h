#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <ArduinoNvs.h>
#include <Wire.h>
#include <U8x8lib.h>
#include <Fsm.h>

//Declare LCD
extern U8X8_SH1106_128X64_NONAME_HW_I2C lcd;
constexpr int LCD_COLS = 16;
constexpr int LCD_ROWS = 2;

//NVS names
constexpr char NVS_MODE[] = "boot_mode";
constexpr char NVS_SSID[] = "SSID";
constexpr char NVS_PSWD[] = "PSWD";
constexpr char NVS_STAT[] = "STAT";


//Declare common FSM states and entries
void disp_mode_on_enter();
void reboot_on_enter();
extern State state_disp_mode;
extern State state_reboot;

//force boot to menu mode
void force_menu_boot();



//Define mode names
constexpr char CONF_MODE[] = "conf_mode";
constexpr char A2DP_MODE[] = "A2DP_mode";
constexpr char MENU_MODE[] = "menu_mode";
constexpr char WIFI_MODE[] = "wifi_mode";

//Declare BT name
constexpr const char BT_NAME[] = "ESP32_BT";

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
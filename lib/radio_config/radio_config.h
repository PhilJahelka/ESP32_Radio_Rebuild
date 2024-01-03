#include <Bounce2.h>
#include <ArduinoNvs.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Fsm.h>

//Define mode names

extern const char *WIFI_MODE;
extern const char *CONF_MODE;
extern const char *A2DP_MODE;
extern const char *MENU_MODE;

//Declare station list
extern const char *stations[][2];

//declare LCD
extern LiquidCrystal_I2C lcd;

//Buttons
extern Bounce scroll;
extern const int SCROLL_TRIG;
extern Bounce enter;
extern const int ENTER_TRIG;

//WiFi credentials
extern const char* ssid;
extern const char* password;
extern const int NUM_STATIONS;
extern const int bt_station;
extern const char* BT_name;

//define pins
extern const int I2S_DOUT;
extern const int I2S_BCLK;
extern const int I2S_LRC;
extern const int I2C_SDA;
extern const int I2C_SCL;
extern const int SCROLL;
extern const int ENTER;
/*
Have setup first detect which mode to boot into and then configure the board appropriately
*/
#include <Arduino.h>
#include <radio_config.h>
#include <A2DP_mode.h>
#include <wifi_mode.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

Bounce scroll = Bounce();
Bounce enter = Bounce();

const char* boot_mode;

void setup() {
  // Debugging output
  Serial.begin(115200);
  // Setup buttons
  scroll.attach(SCROLL, INPUT_PULLUP);
  scroll.interval(100);
  enter.attach(ENTER, INPUT_PULLUP);
  enter.interval(100);
  // Setup display
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  // Read boot option
  NVS.begin();
  boot_mode = NVS.getString("boot_mode").c_str();
  if ( strcmp(boot_mode, WIFI_MODE) == 0 )
  {
    wifi_config();
  } 
  else if ( strcmp(boot_mode, A2DP_MODE) == 0 )
  {
    A2DP_config();
  }
  else if ( strcmp(boot_mode, CONF_MODE) == 0 )
  {
    //init config
  }
  else if ( strcmp(boot_mode, MENU_MODE) == 0 )
  {
    //init to menu
  }
  else
  {
    Serial.println("Booted Into Invalid Mode");
    lcd.print("Bad Boot Mode");
    lcd.setCursor(0, 1);
    lcd.print("Boot Into Menu");
    delay(3000);
    //init to Menu
  }
}

void loop(){
  if ( strcmp(boot_mode, WIFI_MODE) == 0 )
  {
    wifi_loop();
  } 
  else if ( strcmp(boot_mode, A2DP_MODE) == 0 )
  {
    A2DP_loop();
  }
  else if ( strcmp(boot_mode, CONF_MODE) == 0 )
  {
    //loop config
  }
  else if ( strcmp(boot_mode, MENU_MODE) == 0 )
  {
    //loop to menu
  }
  else
  {
    Serial.println("Loop In Invalid Mode");
    lcd.print("Bad Loop Mode");
    lcd.setCursor(0, 1);
    lcd.print("Boot Into Menu");
    delay(3000);
    //init to Menu
  }
}
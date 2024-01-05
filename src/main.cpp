/*
Have setup first detect which mode to boot into and then configure the board appropriately
*/
//radio_config contains all the needed declarations for hardware passing stuff to mode libraries
#include <radio_config.h>
//the individual modes for the ESP32 to boot into
#include <A2DP_mode.h>
#include <wifi_mode.h>
#include <menu_mode.h>
#include <conf_mode.h>

//create buttons
Bounce scroll = Bounce();
Bounce enter = Bounce();
//boot mode read from flash
String read_boot_mode;

hd44780_I2Cexp lcd(0x27);
int lcd_status;

void setup() {
  // Debugging output
  Serial.begin(115200);
  Serial.println("ESP32 Booted!");
  //disable WDT to avoid stutter
  disableCore0WDT();
  disableCore1WDT();
  // Setup buttons
  scroll.attach(SCROLL_PIN, INPUT_PULLUP);
  Serial.print("Scroll attached to: ");
  Serial.println(SCROLL_PIN);
  scroll.interval(100);
  enter.attach(ENTER_PIN, INPUT_PULLUP);
  Serial.print("Enter attached to: ");
  Serial.println(ENTER_PIN);
  enter.interval(100);
  // Setup display
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd_status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (lcd_status)
  {
    hd44780::fatalError(lcd_status);
  }
  lcd.backlight();
  lcd.clear();
  // Read boot option
  NVS.begin();
  Serial.println("Started NVS");
  read_boot_mode = NVS.getString(NVS_MODE);
  Serial.println("Read NVS");
  Serial.println(read_boot_mode);
  /* if ( read_boot_mode == String(WIFI_MODE) )
  {
    wifi_config();
  } 
  else  */if ( read_boot_mode == String(A2DP_MODE) )
  {
    A2DP_config();
  }
  else if ( read_boot_mode == String(CONF_MODE) )
  {
    conf_config();
  }
  else if ( read_boot_mode == String(MENU_MODE) )
  {
    menu_config();
  }
  else
  {
    Serial.println("Booted Into Invalid Mode");
    lcd.print("Bad Boot Mode");
    delay(1000);
    force_menu_boot();
  }
}

void loop(){
  //feeding watchdog prevents audio stutters
  //detect button press
  int pressed_button = 0;
  scroll.update();
  enter.update();
  if (scroll.fell()) {
      Serial.println("caught scroll");
      pressed_button = SCROLL_TRIG;
  }
  if (enter.fell()) {
      Serial.println("caught enter");
      pressed_button = ENTER_TRIG;
  }
  /* if ( read_boot_mode == String(WIFI_MODE) )
  {
    //wifi_config();
  } 
  else  */if ( read_boot_mode == String(A2DP_MODE) )
  {
    A2DP_loop(pressed_button);
  }
  else if ( read_boot_mode == String(CONF_MODE) )
  {
    conf_loop(pressed_button);
  }
  else if ( read_boot_mode == String(MENU_MODE) )
  {
    menu_loop(pressed_button);
  }
  else
  {
    Serial.println("Loop In Invalid Mode");
    lcd.print("Bad Loop Mode");
    delay(1000);
    force_menu_boot();
  }
}

void force_menu_boot(){
  lcd.clear();
  lcd.print("Boot Into Menu");
  delay(1000);
  bool NVS_status = NVS.setString(NVS_MODE, String(MENU_MODE));
  if (NVS_status == false){
    Serial.println("mode write broken");
  }
  Serial.print("wrote mode: ");
  Serial.println(MENU_MODE);
  ESP.restart();
}
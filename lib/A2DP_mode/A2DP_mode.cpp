#include <A2DP_mode.h>

BluetoothA2DPSink a2dp_sink;

//define states
State state_disp_bt(&disp_bt_on_enter, NULL, NULL);
State state_disp_mode(&disp_mode_on_enter, NULL, NULL);
State state_reboot(&reboot_on_enter, NULL, NULL);
//init machine
Fsm fsm(&state_disp_bt);
//entry functions
void disp_bt_on_enter()
{
    lcd.clear();
    lcd.print("BT Name:");
    lcd.setCursor(0,1);
    lcd.print(BT_name);
}
void disp_mode_on_enter()
{
    lcd.clear();
    lcd.print("Enter Mode Select?");
}
void reboot_on_enter()
{
    NVS.setString("boot_mode", MENU_MODE);
    Serial.println("Booting into menu mode");
    ESP.restart();
}


int A2DP_config(){
    Serial.println("in BT mode");
    //delay(1000); 
    i2s_pin_config_t my_pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = -1
    };
    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.start(BT_name);
    //define transitions
    fsm.add_transition(&state_disp_bt, &state_disp_mode, SCROLL_TRIG, NULL);
    fsm.add_transition(&state_disp_mode, &state_disp_bt, SCROLL_TRIG, NULL);
    fsm.add_transition(&state_disp_bt, &state_reboot, ENTER_TRIG, NULL);
    return(0);
}

int A2DP_loop(){
    if (scroll.fell()) {
        fsm.trigger(SCROLL_TRIG);
    }
    if (enter.fell()) {
        fsm.trigger(ENTER_TRIG);
    }
    return(0);
}
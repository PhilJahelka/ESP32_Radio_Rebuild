#include <A2DP_mode.h>

BluetoothA2DPSink a2dp_sink;

//define states
State state_disp_bt("show_BT", disp_bt_on_enter);
//init machine
SimpleFSM a2dp_fsm(&state_disp_bt);
//entry functions
void disp_bt_on_enter()
{
    lcd.clear();
    lcd.print("BT Name:");
    lcd.setCursor(0,1);
    lcd.print(BT_NAME);
}


void A2DP_config(){
    Serial.println("in BT mode");
    //delay(1000); 
    i2s_pin_config_t my_pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = -1
    };
    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.start(BT_NAME);
    //define transitions
    a2dp_fsm.add( Transition(&state_disp_bt, &state_disp_mode, SCROLL_TRIG), 1);
    a2dp_fsm.add( Transition(&state_disp_mode, &state_disp_bt, SCROLL_TRIG), 1);
    a2dp_fsm.add( Transition(&state_disp_mode, &state_reboot, ENTER_TRIG), 1);
    a2dp_fsm.run();
}

void A2DP_loop(int trigger){
    switch (trigger)
    {
    case SCROLL_TRIG:
        a2dp_fsm.trigger(SCROLL_TRIG);
        break;
    case ENTER_TRIG:
        a2dp_fsm.trigger(ENTER_TRIG);
        break;
    default:
        break;
    }
}
///////// Execute preset

#include "defines.h"
#include "lib.h"

#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include "presetManager.h"

#include "IRReciever.h"

#define VER "V. 1.2.2"

using namespace PhotoPizza;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // select the pins used on the LCD panel
AccelStepper stepper(AccelStepper::DRIVER, 12, 13);

///////////  Presets
presetManager presets;

static bool bRun = false;

static void prvExecutePreset();

byte e_flag = 0;

int cur_mode = MENU_MODE;

void libInit(){
  presets.init();
  sayHello();
  show_curr_program(false);
}

void sayHello() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PhotoPizza DIY");
  Serial.println("PhotoPizza DIY " VER);
  lcd.setCursor(0, 1);
  lcd.print(VER);
  delay(2000);
}

void prvExecutePreset() {
  if(bRun){ //TODO: fix infinite run!
    Serial.println("stopping");
    stepper.stop();
    lcd.setCursor(0, 1);
    lcd.print("Program stopping ");
    return;
  }

  bRun = true;
  lcd.setCursor(0, 1);
  lcd.print("Program started ");

  Serial.println("Run");
  int steps = presets.getValue(STEPS) * presets.getValue(DIR); //TODO: refactor getters (via local vars)
  Serial.println((String)"Accel" + presets.getValue(ACC));
  Serial.println((String)"Steps" + steps);
  Serial.println((String)"Speed" + presets.getValue(SPEED));
  stepper.setCurrentPosition(0L);
  if(presets.getValue(ACC) == 0){
    stepper.setAcceleration(10000000); //no acc.
  }else
    stepper.setAcceleration(presets.getValue(ACC));
  stepper.moveTo(steps);
  stepper.setMaxSpeed(presets.getValue(SPEED));
}

void finishPreset(){
  lcd.setCursor(0, 1);
  lcd.print("Program finished");
  Serial.println("Finished");
  delay(1000);
  show_curr_program(false);
}

void libLoop(){
  //int key = IrGetKey();
  /*if( key == BTN_POWER){
    prvExecutePreset();
  }*/

  if(!stepper.run() && bRun){
    bRun = false;
    finishPreset();
  }

  switch (cur_mode) {
  case MENU_MODE:
    menu_mode();
    break;
  case EDIT_MODE:
    edit_preset_mode();
    break;
  }
}

///////////////////////////////////////

///////// edit preset

void value_u() {
  presets.valueUp();
}

void value_d() {
  presets.valueDown();
}

///////////////////////////////////////

///////// print info

void show_curr_program(bool _is_edit) {
  lcd.clear();
  print_prog_num();
  print_dir_small(presets.getValue(DIR));

  if (_is_edit) {
    lcd.setCursor(14, 0);
    lcd.print("E");
  } else {
    lcd.setCursor(14, 0);
    lcd.print("M");
  }

  switch (presets.getParamNumber()) {
  case SPEED:
    print_ul("sp", presets.getValue());
    break;

  case STEPS:
    if (presets.getValue() == 0) {
      lcd.setCursor(0, 1);
      lcd.print("rot");
      lcd.setCursor(4, 1);
      lcd.print("inf");
    } else {
      print_ul("rot", presets.getValue());
    }
    break;

  case ACC:
    print_ul("acc", presets.getValue());
    break;

  case DIR:
    print_dir(presets.getValue());
    break;
  }

}

void print_prog_num() {
  lcd.setCursor(0, 0);
  lcd.print("Program");
  lcd.setCursor(8, 0);
  lcd.print((presets.getPresetNumber() + 1));
}

void print_ul(String _pref, long _value) {
  lcd.setCursor(0, 1);
  lcd.print(_pref);
  lcd.setCursor(4, 1);
  lcd.print(_value);
}

void print_dir(int _dir) {
  lcd.setCursor(0, 1);
  lcd.print("dir");
  lcd.setCursor(5, 1);
  if (_dir == CW) {
    lcd.print("CW");
  } else {
    lcd.print("CCW");
  }
}

void print_dir_small(int _dir) {
  lcd.setCursor(10, 0);
  if (_dir == CW) {
    lcd.print(">");
  } else {
    lcd.print("<");
  }
}
///////////////////////////////////////

///////// LCD Buttons
int read_LCD_buttons() { // read the buttons
  int adc_key_in;
  adc_key_in = analogRead(0); // read the value from the sensor

  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in > 1000)
    return btnNONE;

  // For V1.1 us this threshold
  if (adc_key_in < 50)
    return btnRIGHT;
  if (adc_key_in < 250)
    return btnUP;
  if (adc_key_in < 450)
    return btnDOWN;
  if (adc_key_in < 650)
    return btnLEFT;
  if (adc_key_in < 850)
    return btnSELECT;

  // For V1.0 comment the other threshold and use the one below:
  /*
   if (adc_key_in < 50)   return btnRIGHT;
   if (adc_key_in < 195)  return btnUP; 
   if (adc_key_in < 380)  return btnDOWN; 
   if (adc_key_in < 555)  return btnLEFT; 
   if (adc_key_in < 790)  return btnSELECT;   
   */

  return btnNONE; // when all others fail, return this.
}

void edit_preset_mode() {
  long val;
  int key = IrGetKey();
  switch (key) {
  /*case 0:
    e_flag = 0;
    show_curr_program(true);
    break;*/

  case BTN_POWER: //exit without writing to mem
    presets.resetParamNumber();
    key = 0;
    e_flag = 0;
    show_curr_program(false);
    cur_mode = MENU_MODE;
    break;

  case BTN_FUNC: // write to mem and exit
    key = 0;
    e_flag = 0;
    presets.save();
    show_curr_program(false);
    cur_mode = MENU_MODE;
    break;

  case BTN_CH_U:
  case btnUP:
    e_flag = 0;
    presets.nextParam();
    show_curr_program(true);
    break;

  case BTN_CH_D:
  case btnDOWN:
    e_flag = 0;
    presets.prevParam();
    show_curr_program(true);
    break;

  case BTN_VOL_D:
  case btnLEFT:
    e_flag = 0;
    value_d();
    show_curr_program(true);
    break;

  case BTN_VOL_U:
  case btnRIGHT:
    e_flag = 0;
    value_u();
    show_curr_program(true);
    break;

  case BTN_0:
    if (e_flag != 0) {
      val = presets.getValue() * 10;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_1:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 1;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_2:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 2;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_3:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 3;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_4:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 4;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_5:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 5;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_6:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 6;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_7:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 7;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_8:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 8;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_9:
    if (e_flag != 0) {
      val = presets.getValue() * 10 + 9;
      presets.setValue(val);
      show_curr_program(true);
    }
    break;

  case BTN_EQ:
    if (e_flag == 0) {
      e_flag = 1;
      presets.setValue(0);
    } else {
      e_flag = 0;
    }
    show_curr_program(true);
    break;

  case BTN_ST:
    presets.setValue(0);
    show_curr_program(true);
    break;

  case BTN_RW:
    val = presets.getValue() / 10;
    presets.setValue(val);
    show_curr_program(true);
    break;

  case BTN_FW:
    val = presets.getValue() * 10;
    presets.setValue(val);
    show_curr_program(true);
    break;

/*  default:
    Serial.println("Unknown key");
    break;*/
  }

}

void menu_mode() {
  int key = IrGetKey();
  switch (key) {
  /*case 0: // first call, no key pressed yet
    presets.resetParamNumber();
    show_curr_program(false);
    break;*/
  case BTN_VOL_U:
    presets.nextParam();
    show_curr_program(false);
    break;

  case BTN_VOL_D:
    presets.prevParam();
    show_curr_program(false);
    break;

  case BTN_CH_U:
  case btnRIGHT:
    presets.nextPreset();
    show_curr_program(false);
    break;

  case BTN_CH_D:
  case btnLEFT:
    presets.prevPreset();
    show_curr_program(false);
    break;

  case BTN_FW:
    presets.changeDirection(CW);
    show_curr_program(false);
    break;

  case BTN_RW:
    presets.changeDirection(CCW);
    show_curr_program(false);
    break;

  case BTN_FUNC:
    key = 0;
    edit_preset_mode();
    show_curr_program(true);
    cur_mode = EDIT_MODE;
    break;

  case BTN_PLAY:
  case btnUP:
    prvExecutePreset();
    break;

/*  default:
    Serial.println("Unknown key");
    break;*/
  }

}


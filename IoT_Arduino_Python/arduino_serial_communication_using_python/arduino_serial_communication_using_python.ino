#include <ResponsiveAnalogRead.h>
#include <ModbusRtu.h>
#include <FileIO.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <dht.h>
#include <SPI.h>
#include <mcp_can.h>
#include <stdio.h>

#define DIGITAL_PIN_IN_1 2 //Digital INPUT
#define DIGITAL_PIN_IN_2 10 //Digital INPUT
#define DIGITAL_PIN_IN_3 49 //Digital INPUT
#define DIGITAL_PIN_IN_4 47 //Digital INPUT
#define DIGITAL_PIN_IN_5 21 //Digital INPUT// Interrupt Pin
#define DIGITAL_PIN_IN_6 19 //Digital INPUT// Interrupt Pin
#define DIGITAL_PIN_IN_7 53 //Digital INPUT
#define DIGITAL_PIN_IN_8 11 //Digital INPUT
#define DIGITAL_PIN_IN_9 48 //Digital INPUT
#define DIGITAL_PIN_IN_10 46 //Digital INPUT
#define DIGITAL_PIN_IN_11 20 //Digital INPUT// Interrupt Pin
#define DIGITAL_PIN_IN_12 18 //Digital INPUT// Interrupt Pin

#define INT_PIN_IN_1 4 //Digital INPUT
#define INT_PIN_IN_2 5 //Digital INPUT
#define INT_PIN_IN_3 10 //Digital INPUT
#define INT_PIN_IN_4 11 //Digital INPUT

#define RELAY_PIN_OUT_1 25 //RELAY OUTPUT
#define RELAY_PIN_OUT_2 26 //RELAY OUTPUT
#define RELAY_PIN_OUT_3 27 //RELAY OUTPUT
#define RELAY_PIN_OUT_4 28 //RELAY OUTPUT

#define RELAY_PIN_IND_1 32 //RELAY INDICATOR
#define RELAY_PIN_IND_2 31 //RELAY INDICATOR
#define RELAY_PIN_IND_3 30 //RELAY INDICATOR
#define RELAY_PIN_IND_4 39 //RELAY INDICATOR

#define PWM_PIN_OUT_1 9  //PWM OUTPUT
#define PWM_PIN_OUT_2 8 //PWM OUTPUT
#define PWM_PIN_OUT_3 7 //PWM OUTPUT
#define PWM_PIN_OUT_4 6 //PWM OUTPUT

#define PWM_PIN_IND_1 36  //PWM INDICATOR
#define PWM_PIN_IND_2 34 //PWM INDICATOR
#define PWM_PIN_IND_3 33 //PWM INDICATOR
#define PWM_PIN_IND_4 35 //PWM INDICATOR

#define ANALOG_PIN_1 A15 //ANALOG INPUT
#define ANALOG_PIN_2 A12 //ANALOG INPUT
#define ANALOG_PIN_3 A14 //ANALOG INPUT
#define ANALOG_PIN_4 A13 //ANALOG INPUT

#define ANALOG_IND_1 37 //ANALOG INDICATOR
#define ANALOG_IND_2 40 //ANALOG INDICATOR
#define ANALOG_IND_3 41 //ANALOG INDICATOR
#define ANALOG_IND_4 38 //ANALOG INDICATOR

int dig[12] = {DIGITAL_PIN_IN_1, DIGITAL_PIN_IN_2, DIGITAL_PIN_IN_3, DIGITAL_PIN_IN_4, DIGITAL_PIN_IN_5, DIGITAL_PIN_IN_6, DIGITAL_PIN_IN_7, DIGITAL_PIN_IN_8, DIGITAL_PIN_IN_9, DIGITAL_PIN_IN_10, DIGITAL_PIN_IN_11, DIGITAL_PIN_IN_12};
int dig_value[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int adc[4] = {ANALOG_PIN_1, ANALOG_PIN_2, ANALOG_PIN_3, ANALOG_PIN_4};
int adc_ind[4] = {ANALOG_IND_1, ANALOG_IND_2, ANALOG_IND_3, ANALOG_IND_4};
int adc_value[4] = {0, 0, 0, 0};

int pwms[4] = {PWM_PIN_OUT_1, PWM_PIN_OUT_2, PWM_PIN_OUT_3, PWM_PIN_OUT_4};
int pwm_ind[4] = {PWM_PIN_IND_1, PWM_PIN_IND_2, PWM_PIN_IND_3, PWM_PIN_IND_4};
int pwm_value[4] = {0, 0, 0, 0};
int pwm_address[4] = {4, 5, 6, 7};

int relays[4] = {RELAY_PIN_OUT_1, RELAY_PIN_OUT_2, RELAY_PIN_OUT_3, RELAY_PIN_OUT_4};
int relay_ind[4] = {RELAY_PIN_IND_1, RELAY_PIN_IND_2, RELAY_PIN_IND_3, RELAY_PIN_IND_4};
int relay_value[4] = {0, 0, 0, 0};
int relay_address[4] = {0, 1, 2, 3};

int intPin[4] = {INT_PIN_IN_1, INT_PIN_IN_2, INT_PIN_IN_3, INT_PIN_IN_4};
//int intPin_ind[4] = {DIGITAL_PIN_IN_5, DIGITAL_PIN_IN_6, DIGITAL_PIN_IN_11, DIGITAL_PIN_IN_12};
int intPin_value[4] = {0, 0, 0, 0};
int intPin_address[4] = {8, 9, 10, 11};
int intPin_noti[4] = {0, 0, 0, 0};

ResponsiveAnalogRead analog1(ANALOG_PIN_1, true);
ResponsiveAnalogRead analog2(ANALOG_PIN_2, true);
ResponsiveAnalogRead analog3(ANALOG_PIN_3, true);
ResponsiveAnalogRead analog4(ANALOG_PIN_4, true);

String string_from_atmega2560 = "{\"D\":[1,2,3,4,5,6,7,8,9,10,11,12]}";
String string_from_dragino;
String method_type = "";
String digital_value_string = "";

void setup() {  
  Serial.begin(115200);  //From Ar9331  Black Window
  Serial1.begin(115200);  //To FTDI  Arduino Serial
}  
  
void loop() { 
    if(Serial.available()>0){
          string_from_dragino = Serial.readStringUntil("\n");
          Serial1.println(string_from_dragino);
    }
    Serial.println();
    
    int first_comma_index = string_from_dragino.indexOf(',');
    int second_comma_index = string_from_dragino.indexOf(',',first_comma_index + 1);

    String first_value_from_dragino = string_from_dragino.substring(0, first_comma_index);
    String second_value_from_dragino = string_from_dragino.substring(first_comma_index + 1, second_comma_index);
    String third_value_from_dragino = string_from_dragino.substring(second_comma_index + 1);

    //Serial1.println(first_value_from_dragino);
    Serial1.println(second_value_from_dragino);
    //Serial1.println(third_value_from_dragino);
    
    //Serial.println(string_from_atmega2560); // print to *OPENWRT*
    delay(1000);
   
    //fun();

    if (second_value_from_dragino == "digital") {
       DigitalData();
    }

    //string_from_dragino = "";
    
}

void fun(){
  Serial.print("From fun()");
  Serial.flush();
  }

///////////////////////////////////////////////////////DIGITAL_DATA
void DigitalData(){
  for (int i = 0; i < 12; i++) {
    dig_value[i] = digitalRead(dig[i]);
    delay(100);
    digital_value_string = digital_value_string + String(dig_value[i]);
   }
  Serial.println(digital_value_string);
  Serial.flush();
  digital_value_string = "";
}

////////////////////////////////////////////////////////////ANALOG_DATA
void AnalogData(){
  analog1.update();
  analog2.update();
  analog3.update();
  analog4.update();

  adc_value[0] = analog1.getValue();//((analog1.getValue()*5.0)/1023);
  adc_value[1] = analog2.getValue();//((analog2.getValue()*5.0)/1023);
  adc_value[2] = analog3.getValue();//((analog3.getValue()*5.0)/1023);
  adc_value[3] = analog4.getValue();//((analog4.getValue().*5.0)/1023);

  for (int i = 0; i < 4; i++){
    if (adc_value[i] == 0) {
      digitalWrite(adc_ind[i], LOW);
    }
    else {
      digitalWrite(adc_ind[i], HIGH);
    }
  }
}

//////////////////////////////////////////////////////RELAY_DATA
void RelayDataAcq(){
  for (int i = 0; i < 4; i++) {
    relay_value[i] = EEPROM.read(relay_address[i]);
    if (relay_value[i] == 0) {
      digitalWrite(relay_ind[i], LOW);
    }
    else {
      digitalWrite(relay_ind[i], HIGH);
    }
    delay(1);
  }
}

///////////////////////////////////////////////PWM_DATA
void PwmDataAcq(){
  for (int i = 0; i < 4; i++) {
    pwm_value[i] = EEPROM.read(pwm_address[i]);
    if (pwm_value[i] == 0) {
      digitalWrite(pwm_ind[i], LOW);
    }
    else {
      digitalWrite(pwm_ind[i], HIGH);
    }
    delay(1);
  }
}


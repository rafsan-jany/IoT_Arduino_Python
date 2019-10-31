#include <ResponsiveAnalogRead.h>
#include <ModbusRtu.h>
#include <FileIO.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <dht.h>
#include <SPI.h>
#include <mcp_can.h>
#include <stdio.h>
#include <ArduinoJson.h>

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

String string_from_dragino = "";

void setup() {  
  Serial.begin(115200);  //From Ar9331  Black Window
  Serial1.begin(115200);  //To FTDI  Arduino Serial
}  
  
void loop() { 
    if(Serial.available()){
          string_from_dragino = Serial.readStringUntil("\n");
          Serial1.println(string_from_dragino);
    }
    getRequestData(string_from_dragino);
}

void getRequestData(String message){
  String test = message;
  StaticJsonBuffer<400> jsonBufferConfigMqttMessage;
  JsonObject& requestObj = jsonBufferConfigMqttMessage.parseObject(test);
  String method_type = requestObj["type"];
  
  if (method_type == "setRelayData") {
    setRelayData(test);
  }
  else if (method_type == "setPwmData") {
    setPwmData(test);
  }
  else if (method_type == "getRelayStatus") {
    getRelayStatus();
  }
  else if (method_type == "getPwmStatus") {
    getPwmStatus();
  }
  //else if (method_type == "setConfigData") {
    //setConfigData(test);
  //}
  //else if (method_type == "setModBusData") {
    //setModBusData(test);
  //}
  //else if (method_type == "getDhtData") {
   // getDhtData();
  //}
  //else if (method_type == "removeData") {
    //removeData(test);
  //}
  //else if (method_type == "getCanBusData") {
    //getCanBusData();
  //}
  //else if (method_type == "version") {
   //VersionNumber();
  //}
}

void setRelayData(String message) {
  String test = message;
  int port = 0;
  int value = 0;

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root_relay = jsonBuffer.parseObject(test);

  int relay_port_size = root_relay["port"].size();
  int relay_value_size = root_relay["value"].size();

  for (int pv = 0; pv < relay_port_size; pv++) {
    port = int(root_relay["port"][pv]);
    value = int(root_relay["value"][pv]);
    EEPROM.write(relay_address[port - 1], value);
    relay_value[port - 1] = EEPROM.read(relay_address[port - 1]);
    digitalWrite(relays[port - 1], relay_value[port - 1]); // relay on/off
    digitalWrite(relay_ind[port - 1], relay_value[port - 1]); //relay indicator led on/off
  }

  StaticJsonBuffer<200> jsonBuffer_relay;
  JsonObject& relay_dict = jsonBuffer_relay.createObject();
  JsonArray& relay_status_array = relay_dict.createNestedArray("relay");

  for (int z = 0; z < 4; z++) {
    relay_status_array.add(relay_value[z]);
  }
  Serial.println(message);
}

void setPwmData(String message) {
  String test = message;
  int port = 0;
  int value = 0;

  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root_pwm = jsonBuffer.parseObject(test);

  int pwm_port_size = root_pwm["port"].size();
  int pwm_value_size = root_pwm["value"].size();

  for (int pv = 0; pv < pwm_port_size; pv++) {
    port = int(root_pwm["port"][pv]);
    value = int(root_pwm["value"][pv]);
    if (value <= 100) {
      EEPROM.write(pwm_address[port - 1], map(value, 0, 100, 0, 255));
      pwm_value[port - 1] = EEPROM.read(pwm_address[port - 1]);
      analogWrite(pwms[port - 1], pwm_value[port - 1]); //pwm level control
      if (pwm_value[port - 1] > 0) {
        digitalWrite(pwm_ind[port - 1], HIGH); //pwm led indicator on
      }
      else {
        digitalWrite(pwm_ind[port - 1], LOW); //pwm led indicator off
      }
    }
  }
  
  StaticJsonBuffer<400> jsonBuffer_pwm;
  JsonObject& pwm_dict = jsonBuffer_pwm.createObject();
  JsonArray& pwm_status_array = pwm_dict.createNestedArray("pwm");

  for (int z = 0; z < 4; z++) {
    pwm_status_array.add(pwm_value[z]);
  }
  Serial.println(message);
}

void getRelayStatus() {
  String relay_status = "{\"type\":\"getRelayStatus\",\"relay\":[";

  for (int z = 0; z < 4; z++) {
    if(z<3){
      relay_status = relay_status + String(relay_value[z]) + ",";
      }
    else{
      relay_status = relay_status + String(relay_value[z]) + "]}";
      }
  }
  Serial.println(relay_status);
}

void getPwmStatus() {

  String pwm_status = "{\"type\":\"getPwmStatus\",\"pwm\":[";

    for (int z = 0; z < 4; z++) {
    if(z<3){
      pwm_status = pwm_status + String(pwm_value[z]) + ",";
      }
    else{
      pwm_status = pwm_status + String(pwm_value[z]) + "]}";
      }
  }
  Serial.println(pwm_status);
}


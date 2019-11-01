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
int intPin_value[4] = {0, 0, 0, 0};
int intPin_address[4] = {8, 9, 10, 11};
int intPin_noti[4] = {0, 0, 0, 0};

ResponsiveAnalogRead analog1(ANALOG_PIN_1, true);
ResponsiveAnalogRead analog2(ANALOG_PIN_2, true);
ResponsiveAnalogRead analog3(ANALOG_PIN_3, true);
ResponsiveAnalogRead analog4(ANALOG_PIN_4, true);

String string_from_dragino;
String method_type = "";
String digital_value_string = "";
String analog_value_string = "";
String dht_value_string = "";

int first_comma_index = 0;
int second_comma_index = 0;
int third_comma_index = 0;
String first_value_from_dragino = "";
String second_value_from_dragino = "";
String third_value_from_dragino = "";
String fourth_value_from_dragino = "";

int FLAG = 0;
dht DHT;

void setup() {  
  Serial.begin(115200);  // OPENWRT SERIAL
  Serial1.begin(115200);  //ARDUINO SERIAL

   for (int i = 0; i < 12; i++) {
    pinMode(dig[i], INPUT);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(relays[i], OUTPUT); // Relay Pins Assigned as Output
    pinMode(relay_ind[i], OUTPUT); // Relay Indicator Pins Assigned as Output
    pinMode(pwms[i], OUTPUT); // PWM Pins Assigned as Output
    pinMode(pwm_ind[i], OUTPUT); // PWM Indicator Pins Assigned as Output
    pinMode(adc_ind[i], OUTPUT); // Analog Indicator Pins Assigned as Output
    digitalWrite(adc_ind[i], LOW);
    relay_value[i] = EEPROM.read(relay_address[i]);
    pwm_value[i] = EEPROM.read(pwm_address[i]);

    if (relay_value[i] == 255) {
      EEPROM.write(relay_address[i], 0);
      relay_value[i] = 0;
      EEPROM.write(pwm_address[i], 0);
      pwm_value[i] = 0;
      EEPROM.write(intPin_address[i], 0);
      intPin_value[i] = 0;
    } 
    digitalWrite(relays[i], relay_value[i]);
    digitalWrite(relay_ind[i], relay_value[i]);
    digitalWrite(pwms[i], pwm_value[i]);
    digitalWrite(pwm_ind[i], pwm_value[i]);    
  }

  RelayDataAcq();
  PwmDataAcq();
}  
  
void loop() { 
    if(Serial.available()>0){
          string_from_dragino = Serial.readStringUntil(10);
          Serial1.println(string_from_dragino);
          Serial.flush();
          FLAG = 1;
    }
    if(FLAG == 1){
      formattingData(string_from_dragino);
      FLAG = 0;
      }         
}

void formattingData(String str){
  first_comma_index = str.indexOf(',');
  second_comma_index = str.indexOf(',',first_comma_index + 1);
        
  first_value_from_dragino = str.substring(0, first_comma_index);
  if(first_value_from_dragino == "getData"){
     second_value_from_dragino = str.substring(first_comma_index + 1, second_comma_index);
     third_value_from_dragino = str.substring(second_comma_index + 1);
     }
  else if(first_value_from_dragino == "setData"){
       second_value_from_dragino = str.substring(first_comma_index + 1, second_comma_index);

       third_comma_index = str.indexOf(',',second_comma_index + 1);
       third_value_from_dragino = str.substring(second_comma_index + 1, third_comma_index);
       Serial.println(third_value_from_dragino);
            
       fourth_value_from_dragino = str.substring(third_comma_index + 1);
       Serial.println(fourth_value_from_dragino);
       }

    method_type = second_value_from_dragino;
    second_value_from_dragino = "";
    delay(1000);

    if (method_type == "digital") {
       DigitalData(third_value_from_dragino);
      }
    else if(method_type == "analog"){
      AnalogData(third_value_from_dragino);
      }
    else if(method_type == "relay"){
      setRelayData(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "pwm"){
      setPwmData(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "relayStatus"){
       getRelayStatus();    
      }
    else if(method_type == "pwmStatus"){
      getPwmStatus();
      }
    else if(method_type == "dht"){
      getDhtData(third_value_from_dragino);
      }
    else if(method_type == "can"){
      getCanData(third_value_from_dragino);
      }
    else{
      Serial.println("Invalid Datasets");
      }
    method_type = "";  
}

///////////////////////////////////////////////////////DIGITAL_DATA
void DigitalData(String pin){
  int port = pin.toInt();
  
  digital_value_string = "getData,digital,[";
  if(port == 99){
    for (int i = 0; i < 12; i++) {
      dig_value[i] = digitalRead(dig[i]);
      delay(100);
      if(i<11){
      digital_value_string = digital_value_string + String(dig_value[i]) + ",";
      }
      else{
      digital_value_string = digital_value_string + String(dig_value[i]);  
      }    
     } 
   }
  else{
    dig_value[port-1] = digitalRead(dig[port-1]);
    digital_value_string = digital_value_string + String(dig_value[port-1]);
    } 

  digital_value_string = digital_value_string + "]";
  Serial.println(digital_value_string);
  Serial.flush();
  digital_value_string = "";
}

////////////////////////////////////////////////////////////ANALOG_DATA
void AnalogData(String pin){
  int port = pin.toInt();
  
  analog1.update();
  analog2.update();
  analog3.update();
  analog4.update();

  adc_value[0] = analog1.getValue();//((analog1.getValue()*5.0)/1023);
  adc_value[1] = analog2.getValue();//((analog2.getValue()*5.0)/1023);
  adc_value[2] = analog3.getValue();//((analog3.getValue()*5.0)/1023);
  adc_value[3] = analog4.getValue();//((analog4.getValue().*5.0)/1023);

  analog_value_string = "getData,analog,[";
  if(port == 99){ 
   for(int i = 0; i <4; i++){
    if(i<3){
      analog_value_string = analog_value_string + String(adc_value[i]) + ",";
    }
    else{
      analog_value_string = analog_value_string + String(adc_value[i]);
    }
  }
 }
 else{
  analog_value_string = analog_value_string + String(adc_value[port - 1]);
  }
  analog_value_string = analog_value_string + "]";

  for (int i = 0; i < 4; i++){
    if (adc_value[i] == 0) {
      digitalWrite(adc_ind[i], LOW);
    }
    else {
      digitalWrite(adc_ind[i], HIGH);
    }
  }
  Serial.println(analog_value_string);
  Serial.flush();
  analog_value_string = "";  
}


////////////////////////////////////////////////////////getDHTdata
void getDhtData(String pin){
  int port = pin.toInt();
  
  int dht_temp = 0;
  int dht_hum = 0;

  int chk = DHT.read11(dig[port - 1]);
  dht_temp = DHT.temperature;
  dht_hum = DHT.humidity;

  dht_value_string = "getData,dht,[";
  dht_value_string = dht_value_string + String(dht_temp) + "," + String(dht_hum) + "]";

  Serial.println(dht_value_string);
  Serial.flush();

  dht_value_string = "";
}

///////////////////////////////////////////////////////setRelayData
void setRelayData(String pin, String state) {
  int port = pin.toInt();
  int value = state.toInt();

  String relay_string = "";

  EEPROM.write(relay_address[port - 1], value);
  relay_value[port - 1] = EEPROM.read(relay_address[port - 1]);
  digitalWrite(relays[port - 1], relay_value[port - 1]); // relay on/off
  digitalWrite(relay_ind[port - 1], relay_value[port - 1]); //relay indicator led on/off

  relay_string = "setRelayData,relay,[" + String(pin) + String(state) + "]";
  Serial.println(relay_string);
  Serial.flush();
}

//////////////////////////////////////////////////////////////setPwmData
void setPwmData(String pin, String state) {
  int port = pin.toInt();
  int value = state.toInt();

  String pwm_string = "";

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
   pwm_string = "setPwmData,pwm,[" + String(pin) + String(state) + "]";

  Serial.println(pwm_string);
  Serial.flush();
}

void getRelayStatus() {
  String relay_status = "[";
  for (int z = 0; z < 4; z++) {
    if(z<3){
      relay_status = relay_status + String(relay_value[z]) + ",";
    }
    else{
    relay_status = relay_status + String(relay_value[z]) + "]";
    }
  }
  Serial.println(relay_status);
  Serial.flush();
}


void getPwmStatus() {
  String pwm_status = "[";

  for (int z = 0; z < 4; z++) {
    if(z<3){
      pwm_status = pwm_status + String(pwm_value[z]) + ",";
      }
    else{
      pwm_status = pwm_status + String(pwm_value[z]) + "]";
      }
  }
  Serial.println(pwm_status);
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
//////////////////////////////////////////////////////////CAN_DATA
void getCanData(String NONE) {
  MCP_CAN CAN(12); // CS/SS pin
  unsigned char can_len = 0;
  unsigned char can_buf[8];
  unsigned int canID;
  String candata = "";
  candata = "getData,can,[";
  //////////////////////////////////////CANBus End///////////////
  CAN.begin(CAN_250KBPS); /////////canbus initialization
  if (CAN_MSGAVAIL == CAN.checkReceive())
  {
    CAN.readMsgBuf(&can_len, can_buf);
    canID = CAN.getCanId();

    for (int i = 0; i < can_len; i++){
      if(i < can_len - 1){
         candata = candata + String((can_buf[i])) + ",";
        }
      else{
        candata = candata + String((can_buf[i])) + "]";
        }
    }
  }
  
  SPI.end();
  pinMode(dig[6], INPUT);
  delay(10);
}


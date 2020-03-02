/*
 *  1. analog sensor set and reset method function added
 *  2. interrupt set and reset function added with interrupt mode (1 --> RISING, 2 --> FALLING, 3 --> CHANGE, 4 --> LOW)
 *  3. analog sensor indicators blink from eeprom data
 *  4. dht added
 *  5. relay and relay indicator added
 *  6. pwm and pwm indicator added
 *  7. relay status show (all and single)
 *  8. pwm status show (all and single)
 *  9. analog sensors data read (all and single)
 *  10. digital sensors data read (all and single)
 */
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
int adc_indicator[4] = {ANALOG_IND_1, ANALOG_IND_2, ANALOG_IND_3, ANALOG_IND_4};
int adc_value[4] = {0, 0, 0, 0};
int adc_address[4] = {12, 13, 14, 15};
int adc_port_value[4] = {0, 0, 0, 0};

int pwms[4] = {PWM_PIN_OUT_1, PWM_PIN_OUT_2, PWM_PIN_OUT_3, PWM_PIN_OUT_4};
int pwm_indicator[4] = {PWM_PIN_IND_1, PWM_PIN_IND_2, PWM_PIN_IND_3, PWM_PIN_IND_4};
int pwm_value[4] = {0, 0, 0, 0};
int pwm_address[4] = {4, 5, 6, 7};

int relays[4] = {RELAY_PIN_OUT_1, RELAY_PIN_OUT_2, RELAY_PIN_OUT_3, RELAY_PIN_OUT_4};
int relay_indicator[4] = {RELAY_PIN_IND_1, RELAY_PIN_IND_2, RELAY_PIN_IND_3, RELAY_PIN_IND_4};
int relay_value[4] = {0, 0, 0, 0};
int relay_address[4] = {0, 1, 2, 3};

int intPin[4] = {INT_PIN_IN_1, INT_PIN_IN_2, INT_PIN_IN_3, INT_PIN_IN_4};
int intPin_value[4] = {0, 0, 0, 0};
int intPin_address[4] = {8, 9, 10, 11};
int intPin_noti[4] = {0, 0, 0, 0};
int intPin_stored_value_address[4] = {31, 32, 33, 34};
int intPin_mode_address[4] = {35, 36, 37, 38};
int intPin_mode = "";

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

String relay_string = "";
String pwm_string = "";

int INTT_flag = 0;
int intt_address = 21;
int intt_pin_array[4] = {0,0,0,0};

int FLAG = 0;
dht DHT;

uint16_t au16data[16];    
uint8_t u8state;    
boolean modbus_req = false;   
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
     
Modbus master(0,2,13);
//Modbus master(0); // this is master and RS-232 or USB-FTDI via software serial
modbus_t telegram;    
boolean sentData = false;

uint8_t slave = 0;
uint8_t func = 0;
uint16_t regAddr = 0;
uint16_t coils = 0;

void setup() {  
  Serial.begin(115200);  // OPENWRT SERIAL
  Serial1.begin(115200);  //ARDUINO SERIAL

  master.begin(9600);
  master.setTimeOut( 2000 );
  u8state = 0;

  for (int i = 0; i < 12; i++) {
   pinMode(dig[i], INPUT);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(relays[i], OUTPUT); // Relay Pins Assigned as Output
    pinMode(relay_indicator[i], OUTPUT); // Relay Indicator Pins Assigned as Output
    pinMode(pwms[i], OUTPUT); // PWM Pins Assigned as Output
    pinMode(pwm_indicator[i], OUTPUT); // PWM Indicator Pins Assigned as Output
    pinMode(adc_indicator[i], OUTPUT); // Analog Indicator Pins Assigned as Output
    
    relay_value[i] = EEPROM.read(relay_address[i]);
    pwm_value[i] = EEPROM.read(pwm_address[i]);
    adc_port_value[i] = EEPROM.read(adc_address[i]);

    if (relay_value[i] == 255) {
      EEPROM.write(relay_address[i], 0);
      relay_value[i] = 0;
      EEPROM.write(pwm_address[i], 0);
      pwm_value[i] = 0;
      EEPROM.write(intPin_address[i], 0);
      intPin_value[i] = 0;
      EEPROM.write(adc_address[i], 0);
      adc_port_value[i] = 0;
    } 
    digitalWrite(relays[i], relay_value[i]);
    digitalWrite(relay_indicator[i], relay_value[i]);
    digitalWrite(pwms[i], pwm_value[i]);
    digitalWrite(pwm_indicator[i], pwm_value[i]); 
  }

  RelayDataAcq(); //relay indicator led on/off function
  PwmDataAcq(); //pwm indicator led on/off function
  AnalogDataAcq(); //analog sensor indicator led on/off function

  INTT_flag = EEPROM.read(intt_address);
  if (INTT_flag == 255)
  {
    EEPROM.write(intt_address, 0);
    INTT_flag = 0;
  }
  else if(INTT_flag == 1)
  {
    configuration(); //interrupt configuration function
  }
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
  INTT_flag = EEPROM.read(intt_address);
  if (INTT_flag == 1)
  {
    checkInterrupt(); //function for checking whether intterrupt is occured
  } 

  if(modbus_req){
    modbus();
    modbus_req = false;
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
       //Serial.println(third_value_from_dragino); 
            
       fourth_value_from_dragino = str.substring(third_comma_index + 1);
       //Serial.println(fourth_value_from_dragino);
       }

    method_type = second_value_from_dragino;
    second_value_from_dragino = "";
    delay(1000);

    if (method_type == "digital") {
       getDigitalSensorData(third_value_from_dragino);
      }
    else if(method_type == "analog"){
      getAnalogSensorData(third_value_from_dragino);
      }
    else if(method_type == "relay"){
      setRelayData(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "pwm"){
      setPwmData(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "relayStatus"){
       getRelayStatus(third_value_from_dragino);    
      }
    else if(method_type =="pwmStatus"){
      getPwmStatus(third_value_from_dragino);
      }
    else if(method_type == "dht"){
      getDhtData(third_value_from_dragino);
      }
    else if(method_type == "can"){
      getCanData(third_value_from_dragino);
      }
    else if(method_type == "setanalogpin"){
      setAnalogPin(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "resetanalogpin"){
      resetAnalogPin(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "setintt"){
      setInterrupt(third_value_from_dragino,fourth_value_from_dragino);
      }
    else if(method_type == "resetintt"){
      resetInterrupt(third_value_from_dragino,fourth_value_from_dragino);
      }
    else{
      Serial.println("Invalid Datasets");
      Serial.flush();
      }
    method_type = "";  
}

///////////////////////////////////////////////////////DIGITAL_DATA
void getDigitalSensorData(String pin){
  int port = pin.toInt();
  
  digital_value_string = "getData,digital,[";
  //if(port == 99){/////////////////////////////////////////////////////////////////
  if(port == 0){
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
void getAnalogSensorData(String pin){
  int port = pin.toInt();

  int analog_sensor_read[4] = {0, 0, 0, 0};
  
  analog1.update();
  analog2.update();
  analog3.update();
  analog4.update();

  adc_value[0] = analog1.getValue();//((analog1.getValue()*5.0)/1023);
  adc_value[1] = analog2.getValue();//((analog2.getValue()*5.0)/1023);
  adc_value[2] = analog3.getValue();//((analog3.getValue()*5.0)/1023);
  adc_value[3] = analog4.getValue();//((analog4.getValue().*5.0)/1023);
  
  for( int i = 0; i < 4; i++){
    if(adc_port_value[i] == 1){
      analog_sensor_read[i] = adc_value[i];
      }
    else{
      analog_sensor_read[i] = 0; 
      }
  }

  analog_value_string = "getData,analog,[";
  //if(port == 99){ //////////////////////////////////////////////////////////////////////////
  if(port == 0){
   for(int i = 0; i <4; i++){
    if(i<3){
      analog_value_string = analog_value_string + String(analog_sensor_read[i]) + ",";
    }
    else{
      analog_value_string = analog_value_string + String(analog_sensor_read[i]);
    }
  }
 }
 else{
  analog_value_string = analog_value_string + String(analog_sensor_read[port - 1]);
  }
  analog_value_string = analog_value_string + "]";
  
  Serial.println(analog_value_string);
  Serial.flush();
  analog_value_string = "";  
}

//////////////////////////////////////////////////////////////////////////////
void setAnalogPin(String pin, String state){
  int port = pin.toInt();
  String set_analog_pin_string = "";
  EEPROM.write(adc_address[port - 1],1);   

  for(int i = 0; i < 4; i++){
  adc_port_value[i] = EEPROM.read(adc_address[i]);
  if(adc_port_value[i] == 1){
    digitalWrite(adc_indicator[i], HIGH);
    }
   else{
    digitalWrite(adc_indicator[i], LOW);
    }
  }

  set_analog_pin_string = "setData,setanalogpin," + String(port);
  Serial.println(set_analog_pin_string);
  Serial.flush();
}

//////////////////////////////////////////////////////////////////////////////
void resetAnalogPin(String pin, String state){
  int port = pin.toInt();
  String reset_analog_pin_string = "";
  EEPROM.write(adc_address[port - 1],0);   

  for(int i = 0; i < 4; i++){
  adc_port_value[i] = EEPROM.read(adc_address[i]);
  if(adc_port_value[i] == 1){
    digitalWrite(adc_indicator[i], HIGH);
    }
   else{
    digitalWrite(adc_indicator[i], LOW);
    }
  }

  reset_analog_pin_string = "setData,resetanalogpin," + String(port);
  Serial.println(reset_analog_pin_string);
  Serial.flush();
}

/////////////////////////////////////////////////////////////////
void AnalogDataAcq(){
  for(int i = 0; i < 4; i++){
    adc_port_value[i] = EEPROM.read(adc_address[i]);
    if(adc_port_value[i] == 1){
      digitalWrite(adc_indicator[i], HIGH);
      }
     else{
      digitalWrite(adc_indicator[i], LOW);
      }
    }
}

////////////////////////////////////////////////////////getDHTdata
void getDhtData(String pin){
  int port = pin.toInt();
  
  int dht_temp = 0;
  int dht_hum = 0;

  int chk = DHT.read11(dig[port - 1]);
  dht_temp = DHT.temperature;
  dht_hum = DHT.humidity;
  delay(1000);

  dht_value_string = "getData,dht,[";
  dht_value_string = dht_value_string + String(dht_temp) + "," + String(dht_hum) + "]";

  Serial.println(dht_value_string);
  Serial.flush();

  dht_value_string = "";
}

//////////////////////////////////////////////////////////CAN_DATA
void getCanData(String NONE) {
  MCP_CAN CAN(12); // CS/SS pin
  unsigned char can_len = 0;
  unsigned char can_buf[8];
  unsigned int canID;
  String candata = "";
  candata = "getData,can,[";
 
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

  Serial.println(candata);
  Serial.flush();
  
  SPI.end();
  pinMode(dig[6], INPUT);
  delay(10);
}

///////////////////////////////////////////////////////setRelayData
void setRelayData(String pin, String state) {
  int port = pin.toInt();
  int value = state.toInt();

  EEPROM.write(relay_address[port - 1], value);
  relay_value[port - 1] = EEPROM.read(relay_address[port - 1]);
  digitalWrite(relays[port - 1], relay_value[port - 1]); // relay on/off
  digitalWrite(relay_indicator[port - 1], relay_value[port - 1]); //relay indicator led on/off

  relay_string = "setData,relay," + String(port) + "," + String(value);
  Serial.println(relay_string);
  Serial.flush();
}

//////////////////////////////////////////////////////////////setPwmData
void setPwmData(String pin, String state) {
  int port = pin.toInt();
  int value = state.toInt();
  
    if (value <= 100) {
      EEPROM.write(pwm_address[port - 1], map(value, 0, 100, 0, 255));
      pwm_value[port - 1] = EEPROM.read(pwm_address[port - 1]);
      analogWrite(pwms[port - 1], pwm_value[port - 1]); //pwm level control
      if (pwm_value[port - 1] > 0) {
        digitalWrite(pwm_indicator[port - 1], HIGH); //pwm led indicator on
      }
      else {
        digitalWrite(pwm_indicator[port - 1], LOW); //pwm led indicator off
      }
    }
    
   pwm_string = "seData,pwm," + String(port) + "," + String(value);
   Serial.println(pwm_string);
   Serial.flush();
}
///////////////////////////////////////////////////////////getRelayStatus
void getRelayStatus(String pin) {
  int port = pin.toInt();
  
  String relay_status = "[";
  //if(port == 99){/////////////////////////////////////////////////////////////////////////
  if(port == 0){
    for (int z = 0; z < 4; z++) {
      if(z<3){
        relay_status = relay_status + String(relay_value[z]) + ",";
      }
      else{
      relay_status = relay_status + String(relay_value[z]) + "]";
      }
    }
  }
  else{
    relay_status = relay_status + String(relay_value[port -1]) + "]";
    }
  Serial.println(relay_status);
}
///////////////////////////////////////////////////////////////////getPwmStatus
void getPwmStatus(String pin) {
  int port = pin.toInt();
  
  String pwm_status = "[";
  //if(port == 99){//////////////////////////////////////////////////////////////////////////
  if(port == 0){
    for (int z = 0; z < 4; z++) {
      if(z<3){
        pwm_status = pwm_status + String(pwm_value[z]) + ",";
        }
      else{
        pwm_status = pwm_status + String(pwm_value[z]) + "]";
        }
    }
  }
  else{
      pwm_status = pwm_status + String(pwm_value[port -1]) + "]";
    }
  Serial.println(pwm_status);
  Serial.flush();
}

//////////////////////////////////////////////////////RelayDataAcq
void RelayDataAcq(){
  for (int i = 0; i < 4; i++) {
    relay_value[i] = EEPROM.read(relay_address[i]);
    if (relay_value[i] == 0) {
      digitalWrite(relay_indicator[i], LOW);
    }
    else {
      digitalWrite(relay_indicator[i], HIGH);
    }
    delay(1);
  }
}

///////////////////////////////////////////////PwmDataAcq
void PwmDataAcq(){
  for (int i = 0; i < 4; i++) {
    pwm_value[i] = EEPROM.read(pwm_address[i]);
    if (pwm_value[i] == 0) {
      digitalWrite(pwm_indicator[i], LOW);
    }
    else {
      digitalWrite(pwm_indicator[i], HIGH);
    }
    delay(1);
  }
}

/////////////////////////////////////////////////////////////////////////setInterrupt
void setInterrupt(String pin, String state){
  int port = pin.toInt();
  int value = state.toInt();
  
 if(port == 5 ){
  EEPROM.write(intPin_stored_value_address[0],5);
  EEPROM.write(intPin_mode_address[0],value);
  }
  if(port == 6){
  EEPROM.write(intPin_stored_value_address[1],6);
  EEPROM.write(intPin_mode_address[1],value);
  }
  if(port == 11){
  EEPROM.write(intPin_stored_value_address[2],11);
  EEPROM.write(intPin_mode_address[2],value);
  }
  if(port == 12){
  EEPROM.write(intPin_stored_value_address[3],12);
  EEPROM.write(intPin_mode_address[3],value);
  }
  configuration();

  String setintt_string = "setData,setintt," + pin;
  Serial.println(setintt_string);
  Serial.flush();  

  setintt_string = "";
}

///////////////////////////////////////////////////////////////////////resetInterrupt
void resetInterrupt(String pin, String state){
  int port = pin.toInt();
  
 if(port == 5 ){
  EEPROM.write(intPin_stored_value_address[0],255);
  EEPROM.write(intPin_mode_address[0],255);
  }
  if(port == 6){
  EEPROM.write(intPin_stored_value_address[1],255);
  EEPROM.write(intPin_mode_address[1],255);
  }
  if(port == 11){
  EEPROM.write(intPin_stored_value_address[2],255);
  EEPROM.write(intPin_mode_address[2],255);
  }
  if(port == 12){
  EEPROM.write(intPin_stored_value_address[3],255);
  EEPROM.write(intPin_mode_address[3],255);
  }
  configuration();

  String resetintt_string = "setData,resetintt," + pin;
  Serial.println(resetintt_string);
  Serial.flush();  
}
////////////////////////////////////////////////////////configuration
void configuration(){
  //intt_pin_array[4] = {5,6,11,12};
  int read_intt_pin_number = 0;
  int intt_pin_array_size = 0;
  int read_intt_pin_mode = 0;

  for(int i = 0; i <4; i++){
    read_intt_pin_number = EEPROM.read(intPin_stored_value_address[i]); //[5,255,255,255]
    read_intt_pin_mode = EEPROM.read(intPin_mode_address[i]); //[1,255,255,255]
    intPin_value[i] = 0;
    if(read_intt_pin_number<255){
      intt_pin_array[i] = EEPROM.read(intPin_stored_value_address[i]);//[5,0,0,0]// intt_pin_array[4] = {0,0,0,0}, initially declared global
      if(read_intt_pin_mode == 1){
        intPin_mode = RISING;
        }
      else if(read_intt_pin_mode == 2){
        intPin_mode = FALLING;
        }
      else if(read_intt_pin_mode == 3){
        intPin_mode = CHANGE;
        }
      else if(read_intt_pin_mode == 4){
        intPin_mode = LOW;
        }
      intPin_value[i] = 1;
      intt_pin_array_size++; //If intt pin is found, increment the intt_size
      }
    }
  
  if (intt_pin_array_size == 0){
    EEPROM.write(intt_address, 0);
    INTT_flag = 0;
  }
  else{
    EEPROM.write(intt_address, 1);
    INTT_flag = 1;
  }
  if (intPin_value[0] == 1){
    //attachInterrupt(digitalPinToInterrupt(dig[intPin[0]]), int1, RISING);
    attachInterrupt(digitalPinToInterrupt(dig[intPin[0]]), int1, intPin_mode);
  }
  else{
    detachInterrupt(digitalPinToInterrupt(dig[intPin[0]]));
  }
  if (intPin_value[1] == 1){
    //attachInterrupt(digitalPinToInterrupt(dig[intPin[1]]), int2, RISING);   
    attachInterrupt(digitalPinToInterrupt(dig[intPin[1]]), int2, intPin_mode);
  }
  else{
    detachInterrupt(digitalPinToInterrupt(dig[intPin[1]]));
  }
  if (intPin_value[2] == 1){
    //attachInterrupt(digitalPinToInterrupt(dig[intPin[2]]), int3, RISING);
    attachInterrupt(digitalPinToInterrupt(dig[intPin[1]]), int2, intPin_mode);   
  }
  else{
    detachInterrupt(digitalPinToInterrupt(dig[intPin[2]]));
  }
  if (intPin_value[3] == 1){
    //attachInterrupt(digitalPinToInterrupt(dig[intPin[3]]), int4, RISING);
    attachInterrupt(digitalPinToInterrupt(dig[intPin[1]]), int2, intPin_mode);   
  }
  else{
    detachInterrupt(digitalPinToInterrupt(dig[intPin[3]]));
  }
}
////////////////////////////////////////////////////////////////
void int1(){
  intPin_noti[0] = 1;
}
void int2(){
  intPin_noti[1] = 1;
}
void int3(){
  intPin_noti[2] = 1;
}
void int4(){
  intPin_noti[3] = 1;
}
////////////////////////////////////////////////////////////////checkInterrupt
void checkInterrupt(){ 
  for (int i = 0; i < 4; i++){
    if (intPin_noti[i] == 1){
      delay(100);
      String intt_string = "setData,intt," + String(intPin[i] + 1);
      intPin_noti[i] = 0;
      Serial.println(intt_string);
      Serial.flush();    
    }
  }
}
////////////////////////////////////////////////////////////////////////////MODBus
void modbus() {
  //Serial3.println("Data: " + String(slave) + ", " + String(func) + ", " + String(regAddr) + ", " + String(coils));

  telegram.u8id = slave; // slave address
  telegram.u8fct = func; // function code (this one is registers read)
  telegram.u16RegAdd = regAddr; // start address in slave
  telegram.u16CoilsNo = coils; // number of elements (coils or registers) to read
  telegram.au16reg = au16data; // pointer to a memory array in the Arduino

  master.query( telegram ); // send query (only once)

  sentData = true;
  previousMillis = millis();
  while (sentData) {
    currentMillis = millis();
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      sentData = false;
      break;
    }
    if (currentMillis - previousMillis >= 2000) {
      break;
    }
  }
}

void setModBusData(String message) {
  modbus_req = true;
  //[1,3,0000,6] 3-> read holding registers, 6-> write a single host register
  //below codes move somewhere else
  String msg = message;
  String test = msg.substring(msg.indexOf("[") + 1 , msg.indexOf("]"));
  int deli[3];
  int pos = -1;
  //Serial3.println(test);

  for (int i = 0; i < 3; i++) {
    pos = test.indexOf(",", pos + 1);
    deli[i] = pos;
    //Serial.println(pos);
  }

  slave = (test.substring(0 , deli[0])).toInt();
  func = (test.substring(deli[0] + 1 , deli[1])).toInt();
  regAddr = (test.substring(deli[1] + 1 , deli[2])).toInt();
  coils = (test.substring(deli[2] + 1)).toInt();
}


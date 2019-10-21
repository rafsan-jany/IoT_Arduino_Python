String string_from_atmega2560 = "{\"D\":[1,2,3,4,5,6,7,8,9,10,11,12]}";

String string_from_dragino;

//int d = 0;

void setup() {  
  Serial.begin(115200);  //From Ar9331  Black Window
  Serial1.begin(115200);  //To FTDI  Arduino Serial
}  
  
void loop() {  
  //From PC to Linino  
  
  //int c = Serial.read();              // read from *OPENWRT* 
  //if (c != -1) {              
    //Serial1.print(c);            //        write char to  *ARDUINO_SERIAL*
    if(Serial.available()){
          string_from_dragino = Serial.readStringUntil("\n");
          Serial1.println(string_from_dragino);
    }
    Serial.println(string_from_atmega2560); // print to *OPENWRT*
    
  //}  
  delay(100);
  //Serial.println("string_from_atmega2560");
  //From Linino to PC  
  //c = Serial1.read();                // read from UART  windows
  //d = c * 10;
  //if (c != -1) {                      // got anything?  
    //Serial.write(c);                  //    write to USB-CDC  
    //Serial.write(d);
    //Serial.write("Hello");
  //}  
}

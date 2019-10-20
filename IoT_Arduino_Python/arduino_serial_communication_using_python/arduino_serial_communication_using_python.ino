String ser = "";

void setup() {  
  Serial.begin(115200);  //From Ar9331  Black Window
  Serial1.begin(115200);  //To FTDI  Arduino Serial
}  
  
void loop() {  
  //From PC to Linino  
  int c = Serial.read();              // read from USB-CDC  
  //if (c != -1) {              
    Serial1.print(c);            //        write char to UART  
    //Serial1.write("Hi");
    //c = c*10;
    //Serial.print(c);
    Serial.println(c);
  //}  
  delay(1000);
  //From Linino to PC  
  //c = Serial1.read();                // read from UART  windows
  //if (c != -1) {                      // got anything?  
    //Serial.write(c);                  //    write to USB-CDC  
    //Serial.write("Hello");
  //}  
}

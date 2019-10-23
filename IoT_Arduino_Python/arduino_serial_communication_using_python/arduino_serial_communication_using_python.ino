String string_from_atmega2560 = "{\"D\":[1,2,3,4,5,6,7,8,9,10,11,12]}";
String string_from_dragino;
String method_type = "";

void setup() {  
  Serial.begin(115200);  //From Ar9331  Black Window
  Serial1.begin(115200);  //To FTDI  Arduino Serial
}  
  
void loop() { 
    if(Serial.available()){
          string_from_dragino = Serial.readStringUntil("\n");
          Serial1.println(string_from_dragino);
    }

    Serial.println(string_from_atmega2560);
    
    int first_comma_index = string_from_dragino.indexOf(',');
    int second_comma_index = string_from_dragino.indexOf(',',first_comma_index + 1);

    String first_value_from_dragino = string_from_dragino.substring(0, first_comma_index);
    String second_value_from_dragino = string_from_dragino.substring(first_comma_index + 1, second_comma_index);
    String third_value_from_dragino = string_from_dragino.substring(second_comma_index + 1);

    Serial1.println(first_value_from_dragino);
    Serial1.println(second_value_from_dragino);
    Serial1.println(third_value_from_dragino);

    method_type = second_value_from_dragino;
    
    Serial.println(string_from_atmega2560); // print to *OPENWRT*
    delay(1000);
    
    fun();

    if (method_type == "digital") {
      setRelayDat();
    }
}


void fun(){
  Serial.flush();
  Serial.print("From fun()");
  }

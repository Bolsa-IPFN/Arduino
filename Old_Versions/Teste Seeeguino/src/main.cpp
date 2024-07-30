#include <Arduino.h>
#include <ArduinoJson.h>
#include "High_Temp.h"

int N = 100; 

int pin1 = A0;
int pin2 = A1;
int pin3 = A2;
int pin4 = A3;
int pin5 = A4;
int pin6 = A5;
int termopar_pin1 = 0;
int termopar_pin2 = 0;
int termopar_pin3 = 0;
int termopar_pin4 = 0;
int termopar_pin5 = 0;
int termopar_pin6 = 0;
int pin_amb=0;
HighTemp ht2(pin2,A0);
HighTemp ht3(pin3,A0);
HighTemp ht4(pin4,A0);
HighTemp ht5(pin5,A0);
HighTemp ht6(pin6,A0);
int incomingByte = 0; // for incoming serial data


// aqui
static char* action ;
static char* samples ;
long time          = 0;
double latitude    = 0;
double longitude   = 0;
int max = 190;
int min = 20;
int i = 0;
int j = 0;
int val_1_stat =0;
int val_2_stat =0;
int val_3_stat =0;
int val_4_stat =0;
int val_5_stat =0;
int Wait_1_sec=0;
String str;
unsigned long previousMillis = 0;
unsigned long OnTime = 5000;




// Pode ser que va ter alterções aqui 
// average temperature
float average1(int pin){
  float sum = 0.0;
  for (int i =0; i < N ; i++){
    sum += analogRead(pin)*(5/(1023*0.01));
  }
    return sum/N;
  
}


float what_to_do(int pin, int type_of_sensor)
{
  if (type_of_sensor==0)
  {
    return average1(pin);
  }
  else
  {
    if (type_of_sensor == 2)
    {
      return ht2.getRoomTmp();
    }
    else
    {
      if (pin ==A1)
      {
        return ht2.getThmc();
      }
      else if (pin ==A2)
      {
        return ht3.getThmc();
      }
      else if (pin ==A3)
      {
        return ht4.getThmc();
      }
      else if (pin ==A4)
      {
        return ht5.getThmc();
      }
      else if (pin ==A5)
      {
        return ht6.getThmc();
      }
    }
    
  }
}

void do_staf(int n_samples, int inf ) {
  
  i =0;
  // Serial.println(n_samples);
  // Serial.println(inf);
  Serial.println("START_DATA");
  delay(1000);
  while (1){
    StaticJsonDocument<500> doc_send;
    // StaticJsonDocument<200> doc_1;
    doc_send["time"]   = 1351824120;
    doc_send["sample"] = i;
    doc_send["data"][0] = random(min,max);
    doc_send["data"][1] = random(min,max);
    doc_send["data"][2] = random(min,max);
    doc_send["data"][3] = random(min,max);
    doc_send["data"][4] = random(min,max);
    doc_send["data"][5] = random(min,max);
    
    serializeJson(doc_send, Serial);
    Serial.println("");
    delay(1000);
    if (inf == 1 )
    {
      if (i >=n_samples){
        Serial.println("END_DATA");
        break;
      }
      i=i+1;
    }
  }
}

int test_JSON(JsonDocument& doc) {
  // JSON : {'samples':100}
  // estou a pensar que de ser algo deste tipo:
  // {'action': 'start/stop', 'samples': '10/inf'}
  // {"action":"start","samples":"10"}
  // {\"action\":\"start\",\"samples\":\"10\"}
  if(doc.containsKey("action")) {
      // Serial.print("Action is ");
      action = doc["action"];
      // Serial.println(action);
  }
  else{
    // Fazer error msg
    // Serial.print("Fazer error msg");
    return 1;
  }
  if(doc.containsKey("samples")) {
      // Serial.print("samples = ");
      samples = doc["samples"];
      // Serial.println(samples);
  }
  else{
    // Fazer error msg
    // Serial.print("Fazer error msg");
    return 1;
  }
  // delay(100);
  return 0;
}




void read_JSON()
{
  if (Serial.available() > 0) {
    // Allocate the JSON document
    // This one must be bigger than for the sender because it must store the strings
    str = Serial.readStringUntil('\n\r');
    StaticJsonDocument<300> doc;
    // Serial.print("what was inputed: ");
    // Serial.print(str);
    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, str);

    if (err == DeserializationError::Ok) 
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      // Serial.print(test_JSON(doc));
      if (test_JSON(doc) == 0){
        // show_JSON(doc);
        // do_staf(int n_samples, int inf )
        //Serial.print(strcmp(samples,"inf"));
        if (strcmp(samples,"inf") == 0)
          {
            do_staf(0,0);
          }
          else{
            do_staf(atoi(samples),1);
          }
      }
      
      else {
        Serial.print("ERROR");
      }
      
      // delay(1000);
      
    } 
    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.readStringUntil('\n\r');
    }
  }
}

void Brian(JsonDocument& doc_rec)
{
  if (doc_rec.containsKey("val_1") )
  {
      val_1_stat = doc_rec["val_1"];
      if (val_1_stat == 1)
      {
        digitalWrite(8, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(8, LOW);
        // Serial.println("Off");
      }
  }
  if(doc_rec.containsKey("val_2"))
  { 
      val_2_stat = doc_rec["val_2"];
      if (val_2_stat == 1)
      {
        digitalWrite(9, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(9, LOW);
        // Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_3"))
  {
      val_3_stat = doc_rec["val_3"];
      if (val_3_stat == 1)
      {
        digitalWrite(10, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(10, LOW);
        // Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_4") )
  {
      val_4_stat = doc_rec["val_4"];
      if (val_4_stat == 1)
      {
        digitalWrite(11, HIGH);
        //Serial.println("On");
      }
      else
      {
        digitalWrite(11, LOW);
        //Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_5"))
  {
      val_5_stat = doc_rec["val_5"];
      if (val_5_stat == 1)
      {
        digitalWrite(12, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(12, LOW);
        // Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("sensor_type"))
  {
    
    if (doc_rec["sensor_type"].containsKey("A0"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A0"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin1 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin1 = 1;
      }
      if (strcmp (sensor,"termopar_amb")==0)
      {
        termopar_pin1 = 2;
      }
      Serial.println(termopar_pin1);
    }
    if (doc_rec["sensor_type"].containsKey("A1"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A1"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin2 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin2 = 1;
        ht2.begin();
      }
      Serial.println(termopar_pin2);
    }
    if (doc_rec["sensor_type"].containsKey("A2"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A2"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin3 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin3 = 1;
      }
      Serial.println(termopar_pin3);
    }
    if (doc_rec["sensor_type"].containsKey("A3"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A3"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin4 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin4 = 1;
      }
      Serial.println(termopar_pin4);
    }
    if (doc_rec["sensor_type"].containsKey("A4"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A4"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin5 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin5 = 1;
      }
      Serial.println(termopar_pin5);
    }
    if (doc_rec["sensor_type"].containsKey("A5"))
    {
      const char* sensor  = doc_rec["sensor_type"]["A5"];
      // Serial.print(sensor);
      if (strcmp (sensor,"lm35")==0)
      {
        termopar_pin6 = 0;
      }
      if (strcmp (sensor,"termopar_ht")==0)
      {
        termopar_pin6 = 1;
      }
      Serial.println(termopar_pin6);
    }
    
  }
  else
  {
    // Serial.println("\nErro no Json!!\n");
  }
  

}

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(pin6, INPUT);
  i=0;
  j=0;
}

void loop() {
  // send data only when you receive data:
  //read_JSON();
  unsigned long currentMillis = millis();
  StaticJsonDocument<500> doc_rec;
  if (currentMillis - previousMillis >= OnTime)
  {
    StaticJsonDocument<500> doc_send;
    previousMillis = currentMillis;  // Remember the time
      // StaticJsonDocument<200> doc_1;
      doc_send["sample"] = i;
      doc_send["temp_top"] =  average1(pin1);
      doc_send["temp_bot"] =  average1(pin2);
      doc_send["temp_in"] =  average1(pin3);
      doc_send["temp_north"] =  average1(pin4);
      doc_send["temp_south"] =  average1(pin5);
      doc_send["temp"] =  average1(pin6);
      // if( termopar == 1)
      // {
      //   HighTemp ht(A2,A0);
      //   doc_send["temp_top"] = ht.getRoomTmp();
      //   doc_send["temp_bot"] = average1(pin2);
      //   doc_send["temp_in"] = ht.getThmc();
      //   doc_send["temp_north"] = average1(pin4);
      //   doc_send["temp_south"] = average1(pin5);
      //   doc_send["temp"] = average1(pin6);
      // }
      i=i+1;
      serializeJson(doc_send, Serial);
      Serial.println("");
  }
      // if (Serial.available() > 0) {
    // // read the incoming byte:
    //   incomingByte = Serial.read();

    //   // say what you got:
      
    // }
    // j = 0;
    // while(Wait_1_sec == 0)
    // {
    if (Serial.available() > 0) {
      deserializeJson(doc_rec, Serial);
      Brian(doc_rec);
    }
    // delay(4500);
    //   if (j==10)
    //   {
    //     Wait_1_sec =1;
    //     Serial.println(Wait_1_sec);
    //   }
    //   j = j + 1;
    // }
    
    // Wait_1_sec = 0;
}
//{"val_1":0,"val_3":1,"val_5":1}
//{"val_4":1}
// {"val_1":1,"val_2":1,"val_3":1,"val_4":1,"val_5":1}
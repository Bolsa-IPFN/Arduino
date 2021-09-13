#include <Arduino.h>
#include <ArduinoJson.h>

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
        Serial.println("On");
      }
      else
      {
        digitalWrite(8, LOW);
        Serial.println("Off");
      }
  }
  if(doc_rec.containsKey("val_2"))
  { 
      val_2_stat = doc_rec["val_2"];
      if (val_2_stat == 1)
      {
        digitalWrite(9, HIGH);
        Serial.println("On");
      }
      else
      {
        digitalWrite(9, LOW);
        Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_3"))
  {
      val_3_stat = doc_rec["val_3"];
      if (val_3_stat == 1)
      {
        digitalWrite(10, HIGH);
        Serial.println("On");
      }
      else
      {
        digitalWrite(10, LOW);
        Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_4") )
  {
      val_4_stat = doc_rec["val_4"];
      if (val_4_stat == 1)
      {
        digitalWrite(11, HIGH);
        Serial.println("On");
      }
      else
      {
        digitalWrite(11, LOW);
        Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_5"))
  {
      val_5_stat = doc_rec["val_5"];
      if (val_5_stat == 1)
      {
        digitalWrite(12, HIGH);
        Serial.println("On");
      }
      else
      {
        digitalWrite(12, LOW);
        Serial.println("Off");
      }
  }
  else
  {

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
  i=0;
  j=0;
}

void loop() {
  // send data only when you receive data:
  //read_JSON();
   StaticJsonDocument<500> doc_send;
   StaticJsonDocument<500> doc_rec;
    // StaticJsonDocument<200> doc_1;
    doc_send["sample"] = i;
    doc_send["temp_top"] = random(min,max);
    doc_send["temp_bot"] = random(min,max);
    doc_send["temp_in"] = random(min,max);
    doc_send["temp_north"] = random(min,max);
    doc_send["temp_south"] = random(min,max);
    doc_send["temp"] = random(min,max);
    
    serializeJson(doc_send, Serial);
    Serial.println("");
    // if (Serial.available() > 0) {
    // // read the incoming byte:
    //   incomingByte = Serial.read();

    //   // say what you got:
      
    // }
    // j = 0;
    // while(Wait_1_sec == 0)
    // {
      deserializeJson(doc_rec, Serial);
      Brian(doc_rec);
      delay(1000);
    //   if (j==10)
    //   {
    //     Wait_1_sec =1;
    //     Serial.println(Wait_1_sec);
    //   }
    //   j = j + 1;
    // }
    i = i + 1;
    // Wait_1_sec = 0;
}
//{"val_1":0,"val_3":1,"val_5":1}
//{"val_4":1}
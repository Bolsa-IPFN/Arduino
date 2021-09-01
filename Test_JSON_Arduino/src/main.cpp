#include <Arduino.h>
#include <ArduinoJson.h>

int incomingByte = 0; // for incoming serial data

static char* action ;
static char* samples ;
long time          = 0;
double latitude    = 0;
double longitude   = 0;
int max = 190;
int min = 20;
int i =0;
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



void setup() {
  Serial.begin(9600);
  i=0;
}

void loop() {
  // send data only when you receive data:
  //read_JSON();
   StaticJsonDocument<500> doc_send;
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
    delay(1000);
    i=i+1;
}
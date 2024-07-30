#include <Arduino.h>
#include <ArduinoJson.h>
//#include "High_Temp.h"  //Used as reference to the functions 
#include "ADS1X15.h"



int N = 100;


ADS1115 ADS_1(0x48);
ADS1115 ADS_2(0x49);
float voltage_factor_1 = 0;
float voltage_factor_2 = 0;


int digital_pin_1 = 22;
int digital_pin_2 = 23;
int digital_pin_3 = 24;
int digital_pin_4 = 25;
int digital_pin_5 = 26; 
int digital_pin_6 = 27;


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


// ----------------------------------- REF "High_Temp.h"----------------------------------- 

const double VOL_OFFSET = 350.0;//285.00;//280.000;                       // offset voltage, mv
// VOL_OFFSET1 = 332.5101
const double VOL_OFFSET_array[6] = {343.6355,344.7605-1.119,344.8855,343.6355,341.0104,339.2604};                        // offset voltage, mv
const double AMP_AV     = 54.400;                     // Av of amplifier
const double AMP_AV_array[6] = {AMP_AV,AMP_AV,AMP_AV,AMP_AV,AMP_AV,AMP_AV};        
const double V_REF      = 4.096;    // in V
const double ADC_bits   = 65535;                // ADC number of bit -1

const double Var_VtoT_K[3][10] =
{
    {0, 2.5173462e1, -1.1662878, -1.0833638, -8.9773540/1e1, -3.7342377/1e1,
    -8.6632643/1e2, -1.0450598/1e2, -5.1920577/1e4},
    {0, 2.508355e1, 7.860106/1e2, -2.503131/1e1, 8.315270/1e2,
    -1.228034/1e2, 9.804036/1e4, -4.413030/1e5, 1.057734/1e6, -1.052755/1e8},
    {-1.318058e2, 4.830222e1, -1.646031, 5.464731/1e2, -9.650715/1e4,
    8.802193/1e6, -3.110810/1e8}
};



double K_VtoT(double mV)
{
    int i = 0;
    double value = 0;

    if(mV >= -6.478 && mV < 0)
    {
        value = Var_VtoT_K[0][8];

        for(i = 8; i >0; i--)
        {
            value = mV * value + Var_VtoT_K[0][i-1];
        }
    }
    else if(mV >= 0 && mV < 20.644)
    {
        value = Var_VtoT_K[1][9];

        for(i = 9; i >0; i--)
        {
            value = mV * value + Var_VtoT_K[1][i-1];
        }
    }
    else if(mV >= 20.644 && mV <= 54.900)
    {
        value = Var_VtoT_K[2][6];

        for(i = 6; i >0; i--)
        {
            value = mV * value + Var_VtoT_K[2][i-1];
        }
    }

    return value;
}


double getThmcVol(double adc_I2C_read_mV,double VOL_OFFSET, double AMP_AV)                                             // get voltage of thmc in mV
{
    double vout = (double)adc_I2C_read_mV;
    //Serial.println(vout,5);
    double vin  = (vout - VOL_OFFSET)/AMP_AV;
    
    return (vin);    
}



double getThmc(double adc_I2C_read_mV, double VOL_OFFSET, double AMP_AV, double Tamb)
{
    double vol  = getThmcVol(adc_I2C_read_mV, VOL_OFFSET, AMP_AV);
    double tempThmc = K_VtoT(vol)+ Tamb;
    
    return tempThmc;
}

double getRoomTmp(unsigned adc_I2C_read ,unsigned adc_max ,float volt_convert)
{ 
    int Ro = 10, B =  3975; //Nominal resistance 10K, Beta constant
    int Rseries = 10;// Series resistor 10K
    float To = 298.15; // Nominal Temperature
    //Convert voltage measured to resistance value
    //All Resistance are in kilo ohms.
    float Vi = adc_I2C_read * volt_convert;
    float R = (Vi * Rseries) / (3.3 - Vi);
    /*Use R value in steinhart and hart equation
    Calculate temperature value in kelvin*/
    float T =  1 / ((1 / To) + ((log(Ro / R)) / B));
    float Tc = T - 273.15 -3.6 ; // Converting kelvin to celsius
    return Tc;
}


// ----------------------------------- REF "High_Temp.h"----------------------------------- 



// Pode ser que va ter alterções aqui 
// average temperature
float average1(int pin){
  float sum = 0.0;
  for (int i =0; i < N ; i++){
    sum += analogRead(pin)*(5/(1023*0.01));
  }
    return sum/N;
  
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
  if(doc.containsKey("action")) {
      action = doc["action"];
  }
  else{
    // Fazer error msg
    // Serial.print("Fazer error msg");
    return 1;
  }
  if(doc.containsKey("samples")) {
      samples = doc["samples"];
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
        digitalWrite(digital_pin_1, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(digital_pin_1, LOW);
        // Serial.println("Off");
      }
  }
  if(doc_rec.containsKey("val_2"))
  { 
      val_2_stat = doc_rec["val_2"];
      if (val_2_stat == 1)
      {
        digitalWrite(digital_pin_2, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(digital_pin_2, LOW);
        // Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_3"))
  {
      val_3_stat = doc_rec["val_3"];
      if (val_3_stat == 1)
      {
        digitalWrite(digital_pin_3, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(digital_pin_3, LOW);
        // Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_4") )
  {
      val_4_stat = doc_rec["val_4"];
      if (val_4_stat == 1)
      {
        digitalWrite(digital_pin_4, HIGH);
        //Serial.println("On");
      }
      else
      {
        digitalWrite(digital_pin_4, LOW);
        //Serial.println("Off");
      }
  }
  if (doc_rec.containsKey("val_5"))
  {
      val_5_stat = doc_rec["val_5"];
      if (val_5_stat == 1)
      {
        digitalWrite(digital_pin_5, HIGH);
        // Serial.println("On");
      }
      else
      {
        digitalWrite(digital_pin_5, LOW);
        // Serial.println("Off");
      }
  }
  else
  {
    // Serial.println("\nErro no Json!!\n");
  }
  

}

float init_16_ADC(ADS1115 ADS){
  ADS.begin();
  ADS.setGain(1);      // 4.096 volt
  // select slow so the led blinks visible for the eye.
  ADS.setDataRate(0);  // 0 = slow   4 = medium   7 = fast
  ADS.requestADC(0);
  
  return ADS.toVoltage(); // voltage factor
}

void setup() {
  Serial.begin(9600);
  // analogReference(EXTERNAL);
  pinMode(digital_pin_1, OUTPUT);
  pinMode(digital_pin_2, OUTPUT);
  pinMode(digital_pin_3, OUTPUT);
  pinMode(digital_pin_4, OUTPUT);
  pinMode(digital_pin_5, OUTPUT);
  pinMode(digital_pin_6, OUTPUT);

  // voltage_factor_1 = init_16_ADC(ADS_1);
  // voltage_factor_2 = init_16_ADC(ADS_2);  
  ADS_1.begin();
  ADS_1.setGain(1);      // 4.096 volt
  // select slow so the led blinks visible for the eye.
  ADS_1.setDataRate(0);  // 0 = slow   4 = medium   7 = fast
  ADS_1.requestADC(0);
  voltage_factor_1 =  ADS_1.toVoltage();

  ADS_2.begin();
  ADS_2.setGain(1);      // 4.096 volt
  // select slow so the led blinks visible for the eye.
  ADS_2.setDataRate(0);  // 0 = slow   4 = medium   7 = fast
  ADS_2.requestADC(0);
  voltage_factor_2 =  ADS_2.toVoltage();
  
  i=0;
  j=0;
}

float round_2(float var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =3767.16    for rounding off value
    // then type cast to int so value is 3767
    // then divided by 100 so the value converted into 37.67
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
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
      if (ADS_1.isReady())
      {
        int16_t val_0 = ADS_1.readADC(0);
        int16_t val_1 = ADS_1.readADC(1);
        int16_t val_2 = ADS_1.readADC(2);
        int16_t val_3 = ADS_1.readADC(3);
        ADS_1.requestADC(0);  // request a new one

        int16_t val_4 = ADS_2.readADC(0);
        int16_t val_5 = ADS_2.readADC(1);
        int16_t val_6 = ADS_2.readADC(2);
        ADS_2.requestADC(0);  // request a new one
        double Tamb = round_2(getRoomTmp(val_6,65535,voltage_factor_2));
        doc_send["temp_top"] =  round_2(getThmc((double)(val_0 * voltage_factor_1 * 1000.0),VOL_OFFSET_array[0],AMP_AV_array[0],Tamb));
        //doc_send["temp_top"] = val_5 * voltage_factor_1 * 1000.0;

        doc_send["temp_bot"] =  round_2(getThmc((double)(val_1 * voltage_factor_1 * 1000.0),VOL_OFFSET_array[1],AMP_AV_array[1],Tamb));
        doc_send["temp_in"] =  round_2(getThmc((double)(val_2 * voltage_factor_1 * 1000.0),VOL_OFFSET_array[2],AMP_AV_array[2],Tamb));
        doc_send["temp_north"] =  round_2(getThmc((double)(val_3 * voltage_factor_1 * 1000.0),VOL_OFFSET_array[3],AMP_AV_array[3],Tamb));
        doc_send["temp_south"] =  round_2(getThmc((double)(val_4 * voltage_factor_2 * 1000.0),VOL_OFFSET_array[4],AMP_AV_array[4],Tamb));
        doc_send["temp"] =  round_2(getThmc((double)(val_5 * voltage_factor_2 * 1000.0),VOL_OFFSET_array[5],AMP_AV_array[5],Tamb));
      }
      else{
        doc_send["temp_top"] =  -1;
        doc_send["temp_bot"] =  -1;
        doc_send["temp_in"] =  -1;
        doc_send["temp_north"] =  -1;
        doc_send["temp_south"] =  -1;
        doc_send["temp"] =  -1;
      }
      // if( termopar == 1)
      // {
      // doc_send["temp_top"] = round_2(ht1.getThmc());
      // doc_send["temp_bot"] = round_2(ht2.getThmc());
      // doc_send["temp_in"] = round_2(ht3.getThmc());
      // doc_send["temp_north"] = round_2(ht4.getThmc());
      // doc_send["temp_south"] = round_2(ht5.getThmc());
      // doc_send["temp"] = round_2(ht6.getThmc());
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


// //
// //    FILE: ADS_read_async_rdy.ino
// //  AUTHOR: Rob.Tillaart
// // PURPOSE: read analog inputs - straightforward.
// //     URL: https://github.com/RobTillaart/ADS1X15

// // test
// // connect 1 potmeter per port.
// //
// // GND ---[   x   ]------ 5V
// //            |
// //
// // measure at x (connect to AIN0).
// //

// // EXPERIMENTAL
// //
// // The RDY pin (or ALERT Pin) is triggered when conversion is ready
// //



// ADS1115 ADS(0x48);
// float f = 0;


// void setup() 
// {
//   Serial.begin(115200);
//   Serial.println(__FILE__);
//   Serial.print("ADS1X15_LIB_VERSION: ");
//   Serial.println(ADS1X15_LIB_VERSION);

//   ADS.begin();
//   ADS.setGain(1);      // 4.096 volt
//   // select slow so the led blinks visible for the eye.
//   ADS.setDataRate(0);  // 0 = slow   4 = medium   7 = fast
//   f = ADS.toVoltage(); // voltage factor
//   ADS.requestADC(0);
  
//   // set the thresholds to Trigger RDY pin
//   // ADS.setComparatorThresholdLow(0x0000);
//   // ADS.setComparatorThresholdHigh(0x0200);
//   // ADS.setComparatorQueConvert(0);             // enable RDY pin !!
//   // ADS.setComparatorLatch(0);
// }


// void loop() 
// {
//   if (ADS.isReady())
//   {
//     int16_t val_0 = ADS.readADC(0);
//     int16_t val_1 = ADS.readADC(1);
//     ADS.requestADC(0);  // request a new one
//     Serial.print("\tAnalog0: ");
//     Serial.print(val_0);
//     Serial.print('\t');
//     Serial.print(val_0 * f * 1000.0);
//     Serial.print('\t');
//     Serial.println(getThmc((double)(val_0 * f * 1000.0)),3);
//     Serial.print("\tAnalog1: ");
//     Serial.print(val_1);
//     Serial.print('\t');
//     Serial.print(val_1 * f * 1000.0);
//     Serial.print('\t');
//     Serial.println(getThmc((double)(val_1 * f * 1000.0)),3);
//   }
//   // simulate other tasks...
//   delay(2000);
// }


// -- END OF FILE --

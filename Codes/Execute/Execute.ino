#include <DHT.h>
#include <RCSwitch.h>
#include <SoftwareSerial.h>

#define DHT11_OUT_PIN 9
#define DHT11_IN_PIN 8
#define DHTTYPE DHT11 //Active DHT 11 compatibility - Use DHT12 if needed
#define Relai_1 7
#define Relai_2 6
#define Relai_3 5
#define Relai_4 4
#define cut_out 10

SoftwareSerial hc06(2,3);
String cmd="";
int ThermistorPin_1 = A0; //NTC 3950 - 100K / https://www.tme.eu/Document/f9d2f5e38227fc1c7d979e546ff51768/NTCM-100K-B3950.pdf
int ThermistorPin_2 = A1;
int ThermistorPin_3 = A2;
int ThermistorPin_4 = A3;
const float TO = 23.5 + 273.15; //basic temp + celsus valiue
const float RT0 = 100000.0; //termistance resistor => 100K hom
const float R0 = 100000.0; //resistance resistor => 100K hom
const float B = 3950.0; //beta result

RCSwitch Switch_cut_out = RCSwitch();
DHT dhtOUT(DHT11_OUT_PIN, DHTTYPE);
DHT dhtIN(DHT11_IN_PIN, DHTTYPE);

//parallel activity by current Millisecondes
unsigned long currentMillis_eau = 0;
const long interval_eau = 2500;

unsigned long currentMillis_moteur = 0;
const long interval_moteur = 2500;

unsigned long currentMillis_aux1 = 0;
const long interval_aux1 = 2500;

unsigned long currentMillis_aux2 = 0;
const long interval_aux2 = 2500;

unsigned long currentMillis_DHT11 = 0;
const long interval_DHT11 = 5000;

void setup(){
  //Initialize Serial Monitor
  Serial.begin(9600);

  //Initialize Bluetooth Serial Port
  hc06.begin(9600);

  //Initialize DHT interfaces
  dhtOUT.begin();
  dhtIN.begin();

  //Initialize relay interfaces in down position
  pinMode(Relai_1,OUTPUT);
  pinMode(Relai_2,OUTPUT);
  pinMode(Relai_3,OUTPUT);
  pinMode(Relai_4,OUTPUT);

  //Transmitter is connected to Arduino Pin #10  
  Switch_cut_out.enableTransmit(10);

  //Optional set pulse length.
  Switch_cut_out.setPulseLength(360);
  
  //Optional set protocol (default is 1, will work for most outlets)
  Switch_cut_out.setProtocol(1);

}

void loop(){

  //Read data from HC06
  while(hc06.available()>0){
    cmd+=(char)hc06.read();
  }
  
  //Select function with cmd
  if(cmd!=""){
    Serial.print("Command recieved : ");
    Serial.println(cmd);
    
    // ON / OFF relai_1
    if(cmd=="R1O"){
        Serial.println("Relai_1 is on");
        digitalWrite(Relai_1,HIGH);
    }else if(cmd=="R1C"){
        Serial.println("Relai_1 is off");
        digitalWrite(Relai_1,LOW);
    }

    // ON / OFF relai_2
    if(cmd=="R2O"){
        Serial.println("Relai_2 is on");
        digitalWrite(Relai_2,HIGH);
    }else if(cmd=="R2C"){
        Serial.println("Relai_2 is off");
        digitalWrite(Relai_2,LOW);
    }

     // ON / OFF relai_3
    if(cmd=="R3O"){
        Serial.println("Relai_3 is on");
        digitalWrite(Relai_3,HIGH);
    }else if(cmd=="R3C"){
        Serial.println("Relai_3 is off");
        digitalWrite(Relai_3,LOW);
    }

      // ON / OFF relai_4
    if(cmd=="R4O"){
        Serial.println("Relai_4 is on");
        digitalWrite(Relai_4,HIGH);
    }else if(cmd=="R4C"){
        Serial.println("Relai_4 is off");
        digitalWrite(Relai_4,LOW);
    }
      // ON / OFF cut_out
    if(cmd=="X"){
        Serial.println("cut_out is open");
        Switch_cut_out.send("000001100011001101001000");
    }else if(cmd=="Y"){
        Serial.println("cut_out is close");
        Switch_cut_out.send("000001100011001101000100");
    }
    cmd=""; //reset cmd
  }
  
if ( millis() - currentMillis_eau >= interval_eau)
  {
    fonc_temp_eau();
    currentMillis_eau = millis();
  }
if ( millis() - currentMillis_moteur >= interval_moteur)
  {
    fonc_temp_moteur();
    currentMillis_moteur = millis();
  }
if ( millis() - currentMillis_aux1 >= interval_aux1)
  {
    fonc_temp_auxiliere_1();
    currentMillis_aux1 = millis();
  }
if ( millis() - currentMillis_aux2 >= interval_aux2)
  {
    fonc_temp_auxiliere_2();
    currentMillis_aux2 = millis();
  }
if ( millis() - currentMillis_DHT11 >= interval_DHT11)
  {
    fonc_temp_DHT_out();
    fonc_temp_DHT_in();
    currentMillis_DHT11 = millis();
  }
delay(250);
}

float fonc_temp_eau(){
  uint16_t Vtn;
  float T0;
  Vtn = analogRead(ThermistorPin_1);
  T0 = 1/((1/TO)+(1/B)*log((R0/RT0)*((1024.0/Vtn)-1)))-273.15;
  
  Serial.print("Temperature eau : ");
  Serial.print(T0);
  Serial.println(" C");
  hc06.println("*A"+String(T0)+"*");
}


float fonc_temp_moteur(){
  uint16_t Vtn;
  float T1;
  Vtn = analogRead(ThermistorPin_2);
  T1 = 1/((1/TO)+(1/B)*log((R0/RT0)*((1024.0/Vtn)-1)))-273.15;
  
  Serial.print("Temperature moteur : "); 
  Serial.print(T1);
  Serial.println(" C");
  hc06.println("*B"+String(T1)+"*");
}

float fonc_temp_auxiliere_1(){
  uint16_t Vtn;
  float T2;
  Vtn = analogRead(ThermistorPin_3);
  T2 = 1/((1/TO)+(1/B)*log((R0/RT0)*((1024.0/Vtn)-1)))-273.15;
  
  Serial.print("Temperature auxiliere 1 : "); 
  Serial.print(T2);
  Serial.println(" C");
  hc06.println("*C"+String(T2)+"*");
}
float fonc_temp_auxiliere_2(){
  uint16_t Vtn;
  float T3;
  Vtn = analogRead(ThermistorPin_4);
  T3 = 1/((1/TO)+(1/B)*log((R0/RT0)*((1024.0/Vtn)-1)))-273.15;
  
  Serial.print("Temperature auxiliere 2 : "); 
  Serial.print(T3);
  Serial.println(" C");
  hc06.println("*D"+String(T3)+"*");
}
float fonc_temp_DHT_out(){
  float HUOUT1 = dhtOUT.readHumidity(); // Read Celsius
  float TEOUT1 = dhtOUT.readTemperature(); // Read Fahrenheit
  
  Serial.print("DHT_out Humidity: ");
  Serial.print(HUOUT1);
  Serial.println(" %");
  Serial.print("DHT_out Temperature: ");
  Serial.print(TEOUT1);
  Serial.println(" Degrees ");

  hc06.println("*E"+String(HUOUT1)+"*");
  hc06.println("*F"+String(TEOUT1)+"*");
}
float fonc_temp_DHT_in(){
  float HUIN1 = dhtIN.readHumidity(); // Read Celsius
  float TEIN1 = dhtIN.readTemperature(); // Read Fahrenheit
  
  Serial.print("DHT_in Humidity: ");
  Serial.print(HUIN1);
  Serial.println(" %");
  Serial.print("DHT_in Temperature: ");
  Serial.print(TEIN1);
  Serial.println(" Degrees ");

  hc06.println("*G"+String(HUIN1)+"*");
  hc06.println("*H"+String(TEIN1)+"*");
}

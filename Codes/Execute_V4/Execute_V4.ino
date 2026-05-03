#include <SoftwareSerial.h>
#include <RCSwitch.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT_in,DHT_out;
SoftwareSerial hc06(2,3);

String cmd="";
int ThermistorPin_1 = A0;
int ThermistorPin_2 = A1;
int ThermistorPin_3 = A2;
int ThermistorPin_4 = A3;
int Vo;
float sensor_val=0;
float R1 = 10000;
float logR2, R2, T, T1, T2, T3, To;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

#define Relai_1 4
#define Relai_2 5
#define Relai_3 6
#define Relai_4 7
#define cut_out 10
#define DHT11_OUT_PIN 8
#define DHT11_IN_PIN 9

RCSwitch Switch_cut_out = RCSwitch();

void setup(){
  //Initialize Serial Monitor
  Serial.begin(9600);
  //Initialize Bluetooth Serial Port
  hc06.begin(9600);
  pinMode(Relai_1,OUTPUT);digitalWrite(Relai_1,HIGH);
  pinMode(Relai_2,OUTPUT);digitalWrite(Relai_2,HIGH);
  pinMode(Relai_3,OUTPUT);digitalWrite(Relai_3,HIGH);
  pinMode(Relai_4,OUTPUT);digitalWrite(Relai_4,HIGH);

  // Transmitter is connected to Arduino Pin #10  
  Switch_cut_out.enableTransmit(10);

  // Optional set pulse length.
  Switch_cut_out.setPulseLength(360);
  
  // Optional set protocol (default is 1, will work for most outlets)
  Switch_cut_out.setProtocol(1);

}

void loop(){
  DHT_in.read(DHT11_IN_PIN);
  DHT_out.read(DHT11_OUT_PIN);
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
        digitalWrite(Relai_1,LOW);
    }else if(cmd=="R1C"){
        Serial.println("Relai_1 is off");
        digitalWrite(Relai_1,HIGH);
    }

    // ON / OFF relai_2
    if(cmd=="R2O"){
        Serial.println("Relai_2 is on");
        digitalWrite(Relai_2,LOW);
    }else if(cmd=="R2C"){
        Serial.println("Relai_2 is off");
        digitalWrite(Relai_2,HIGH);
    }else{
        Serial.println("Function is off by default");
    }

     // ON / OFF relai_3
    if(cmd=="R3O"){
        Serial.println("Relai_3 is on");
        digitalWrite(Relai_3,LOW);
    }else if(cmd=="R3C"){
        Serial.println("Relai_3 is off");
        digitalWrite(Relai_3,HIGH);
    }else{
        Serial.println("Function is off by default");
    }

      // ON / OFF relai_4
    if(cmd=="O"){
        Serial.println("Relai_4 is on");
        digitalWrite(Relai_4,HIGH);
    }else if(cmd=="C"){
        Serial.println("Relai_4 is off");
        digitalWrite(Relai_4,LOW);
    }else{
        Serial.println("Function is off by default");
    }

      // ON / OFF cut_out
    if(cmd=="C1O"){
        Serial.println("cut_out is open");
        Switch_cut_out.send("000001100011001101001000");
    }else if(cmd=="C1C"){
        Serial.println("cut_out is close");
        Switch_cut_out.send("000001100011001101000100");
    }else{
        Serial.println("Function is off by default");
    }
    cmd=""; //reset cmd
  }
  fonc_temp_eau();
  fonc_temp_moteur();
  fonc_temp_auxiliere_1();
  fonc_temp_auxiliere_2();
  
  DHT_in.read(DHT11_IN_PIN);
  Serial.print("DHT_in - temp:");
  Serial.print(DHT_in.temperature);
  Serial.print("  humi:");
  Serial.println(DHT_in.humidity);
  hc06.println("*E"+String(DHT_in.temperature)+"*");
  hc06.println("*F"+String(DHT_in.humidity)+"*");
  //delay(100);
  DHT_out.read(DHT11_OUT_PIN);
  Serial.print("DHT_out - temp:");
  Serial.print(DHT_out.temperature);
  Serial.print("  humi:");
  Serial.println(DHT_out.humidity);
  //delay(100);
}

void fonc_temp_eau(){
  Vo = analogRead(ThermistorPin_1);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  To = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = To - 273.15;
  Serial.print("Temperature eau : "); 
  Serial.print(T);
  Serial.println(" C");
  hc06.println("*A"+String(T)+"*");
  delay(250);
}

void fonc_temp_moteur(){
  Vo = analogRead(ThermistorPin_2);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  To = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T1 = To - 273.15;
  Serial.print("Temperature moteur : "); 
  Serial.print(T1);
  Serial.println(" C");
  hc06.println("*B"+String(T1)+"*");
  delay(250);
}
void fonc_temp_auxiliere_1(){
  Vo = analogRead(ThermistorPin_3);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  To = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T2 = To - 273.15;
  Serial.print("Temperature auxiliere 1 : "); 
  Serial.print(T2);
  Serial.println(" C");
  hc06.println("*C"+String(T2)+"*");
  delay(250);
}
void fonc_temp_auxiliere_2(){
  Vo = analogRead(ThermistorPin_4);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  To = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T3 = To - 273.15;
  Serial.print("Temperature auxiliere 2 : "); 
  Serial.print(T3);
  Serial.println(" C");
  hc06.println("*D"+String(T3)+"*");
  delay(250);
}

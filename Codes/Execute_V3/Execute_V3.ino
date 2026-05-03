#include <SoftwareSerial.h>

SoftwareSerial hc06(2,3);

String cmd="";
int ThermistorPin = A0;
int Vo;
float sensor_val=0;
float R1 = 10000;
float logR2, R2, T, To, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

#define Relai_1 4
#define Relai_2 5
#define Relai_3 6
#define Relai_4 7
#define Led_1 8

void setup(){
  //Initialize Serial Monitor
  Serial.begin(9600);
  //Initialize Bluetooth Serial Port
  hc06.begin(9600);
  pinMode(Relai_1,OUTPUT);digitalWrite(Relai_1,HIGH);
  pinMode(Relai_2,OUTPUT);digitalWrite(Relai_2,HIGH);
  pinMode(Relai_3,OUTPUT);digitalWrite(Relai_3,HIGH);
  pinMode(Relai_4,OUTPUT);digitalWrite(Relai_4,HIGH);
  pinMode(Led_1,OUTPUT);
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

      // ON / OFF Led_1
    if(cmd=="L1O"){
        Serial.println("Led_1 is on");
        digitalWrite(Led_1,HIGH);
    }else if(cmd=="L1C"){
        Serial.println("Led_1 is off");
        digitalWrite(Led_1,LOW);
    }else{
        Serial.println("Function is off by default");
    }
    cmd=""; //reset cmd
  }

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  To = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = To - 273.15;
//  Tf = (Tc * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature: "); 
//  Serial.print(Tf);
 // Serial.print(" F; ");
  Serial.print(T);
  Serial.println(" C");
  hc06.println("*T"+String(T)+"*");
  //hc06.println(T);
  delay(1000);
  
  // Simulate sensor measurement
  sensor_val=(float)random(200); // random number between 0 and 255
  hc06.println("*G"+String(sensor_val)+"*");
  //hc06.print(sensor_val);
  delay(100);
}

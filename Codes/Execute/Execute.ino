#include <DHT.h>
#include <RCSwitch.h>
#include <SoftwareSerial.h>

#define DHT11_OUT_PIN 9
#define DHT11_IN_PIN 8
#define DHTTYPE DHT11
#define Relai_1 7
#define Relai_2 6
#define Relai_3 5
#define Relai_4 4
#define cut_out 10

SoftwareSerial hc06(2,3);
String cmd = "";

// Pins des thermistances
const int ThermistorPins[4] = {A0, A1, A2, A3};

// Constantes mathématiques pré-calculées
const float TO = 23.5 + 273.15;
const float B = 3950.0;
// R0 / RT0 est égal à 1, donc retiré du calcul pour économiser des cycles CPU

RCSwitch Switch_cut_out = RCSwitch();
DHT dhtOUT(DHT11_OUT_PIN, DHTTYPE);
DHT dhtIN(DHT11_IN_PIN, DHTTYPE);

// Regroupement des timers
unsigned long currentMillis_thermistors = 0;
const long interval_thermistors = 2500;

unsigned long currentMillis_DHT11 = 0;
const long interval_DHT11 = 5000;

void setup() {
  Serial.begin(9600);
  hc06.begin(9600);
  
  dhtOUT.begin();
  dhtIN.begin();

  pinMode(Relai_1, OUTPUT);
  pinMode(Relai_2, OUTPUT);
  pinMode(Relai_3, OUTPUT);
  pinMode(Relai_4, OUTPUT);

  Switch_cut_out.enableTransmit(cut_out);
  Switch_cut_out.setPulseLength(360);
  Switch_cut_out.setProtocol(1);
}

void loop() {
  
  // Lecture Bluetooth (avec légère tempo pour laisser le buffer se remplir)
  while(hc06.available() > 0) {
    cmd += (char)hc06.read();
    delay(2); // Laisse le temps au prochain caractère d'arriver (baud rate 9600)
  }
  
  // Traitement des commandes
  if(cmd != "") {
    Serial.print(F("Command recieved : "));
    Serial.println(cmd);
    
    if(cmd == "R1O") {
      Serial.println(F("Relai_1 is on"));
      digitalWrite(Relai_1, HIGH);
    } else if(cmd == "R1C") {
      Serial.println(F("Relai_1 is off"));
      digitalWrite(Relai_1, LOW);
    } else if(cmd == "R2O") {
      Serial.println(F("Relai_2 is on"));
      digitalWrite(Relai_2, HIGH);
    } else if(cmd == "R2C") {
      Serial.println(F("Relai_2 is off"));
      digitalWrite(Relai_2, LOW);
    } else if(cmd == "R3O") {
      Serial.println(F("Relai_3 is on"));
      digitalWrite(Relai_3, HIGH);
    } else if(cmd == "R3C") {
      Serial.println(F("Relai_3 is off"));
      digitalWrite(Relai_3, LOW);
    } else if(cmd == "R4O") {
      Serial.println(F("Relai_4 is on"));
      digitalWrite(Relai_4, HIGH);
    } else if(cmd == "R4C") {
      Serial.println(F("Relai_4 is off"));
      digitalWrite(Relai_4, LOW);
    } else if(cmd == "X") {
      Serial.println(F("cut_out is open"));
      Switch_cut_out.send("000001100011001101001000");
    } else if(cmd == "Y") {
      Serial.println(F("cut_out is close"));
      Switch_cut_out.send("000001100011001101000100");
    }
    
    cmd = ""; // Reset de la commande
  }
  
  // Timer unique pour les 4 thermistances
  if (millis() - currentMillis_thermistors >= interval_thermistors) {
    lire_et_envoyer_NTC(ThermistorPins[0], "eau", 'A');
    lire_et_envoyer_NTC(ThermistorPins[1], "moteur", 'B');
    lire_et_envoyer_NTC(ThermistorPins[2], "auxiliere 1", 'C');
    lire_et_envoyer_NTC(ThermistorPins[3], "auxiliere 2", 'D');
    
    currentMillis_thermistors = millis();
  }

  // Timer pour les DHT
  if (millis() - currentMillis_DHT11 >= interval_DHT11) {
    lire_et_envoyer_DHT(&dhtOUT, "out", 'E', 'F');
    lire_et_envoyer_DHT(&dhtIN, "in", 'G', 'H');
    
    currentMillis_DHT11 = millis();
  }
  
  // Suppression du delay(250) qui bloquait le programme !
}

// --- Fonctions auxiliaires optimisées (type void car elles ne retournent rien) ---

void lire_et_envoyer_NTC(int pin, const char* nom, char prefix) {
  uint16_t Vtn = analogRead(pin);
  // Calcul optimisé
  float temp = 1.0 / ((1.0 / TO) + (1.0 / B) * log((1024.0 / Vtn) - 1.0)) - 273.15;
  
  Serial.print(F("Temperature "));
  Serial.print(nom);
  Serial.print(F(" : "));
  Serial.print(temp);
  Serial.println(F(" C"));
  
  hc06.print("*");
  hc06.print(prefix);
  hc06.print(temp);
  hc06.println("*");
}

void lire_et_envoyer_DHT(DHT* dht, const char* nom, char prefixHum, char prefixTemp) {
  float hum = dht->readHumidity();
  float temp = dht->readTemperature();
  
  Serial.print(F("DHT_"));
  Serial.print(nom);
  Serial.print(F(" Humidity: "));
  Serial.print(hum);
  Serial.println(F(" %"));
  
  Serial.print(F("DHT_"));
  Serial.print(nom);
  Serial.print(F(" Temperature: "));
  Serial.print(temp);
  Serial.println(F(" Degrees"));

  hc06.print("*"); hc06.print(prefixHum); hc06.print(hum); hc06.println("*");
  hc06.print("*"); hc06.print(prefixTemp); hc06.print(temp); hc06.println("*");
}

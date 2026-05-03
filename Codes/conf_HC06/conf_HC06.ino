/* HC-06 Config est un programme qui permet de configurer le module Bluetooth*/
// Création d'une liaison série sur les broches 2 et 3
#include <SoftwareSerial.h>                                // appel de la bibliothèque
SoftwareSerial HC06(2, 3);                                   // RX, TX

void setup()  
{ 
HC06.begin(9600);                                                 // initialisation connexion série Bluetooth à 9600 bauds
Serial.begin(9600);                                                // initialisation liaison série à 9600 bauds
Serial.println("Entrer la commande AT:");
}

void loop()
{  
 // si le HC-06 transmet, on écrit le message dans le moniteur série
 if (HC06.available())                  
 {
 Serial.write(HC06.read());
 }
 // si on écrit dans le moniteur série, on transmet le message dans le module HC-06
 if (Serial.available())
 {
 HC06.write(Serial.read());
 }   
}

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#include <yasm.h>
#include <btn.h>
YASM menu, retro, moteur;



#include <Streaming.h>

#define PIN_OUT 13

#define MD_CHOISIR 0
#define MD_CONTINU 1
#define MD_PERIODE 2
#define MD_NB_CHOIX 3
uint8_t mode = MD_CHOISIR;

uint8_t P_duree_tourne = 5;            //5secondes
uint8_t P_duree_arret = 5;  
uint8_t P_vitesse = 5; //1 à 10
uint8_t P_pwm_min = 15; 

uint16_t P_tempoLCD = 600;


void setup()
{
  pinMode(PIN_OUT, OUTPUT);
  
  lcd.begin(16, 2);
  
  menu.next(menu_init);
  retro.next(retro_on);
  moteur.next(mot_arret);
}

void loop()
{  
  //execution des différentes tâches :
  retro.run();
  menu.run();
  moteur.run();
}




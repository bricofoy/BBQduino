#include <btn.h>


BTN gauche,droite,haut,bas,select;

void printValeur(uint8_t x)
{
  if (x < 10) lcd<<" ";
  lcd<<x;
}

void printReglage(uint8_t reg)
{
  switch (reg) {
    case 1 : lcd<<"vitesse "; printValeur(P_vitesse); lcd<<"     "; break;
    case 2 : lcd<<"temps on "; printValeur(P_duree_tourne); lcd<<"s   "; break;
    case 3 : lcd<<"temps off "; printValeur(P_duree_arret); lcd<<"s  "; break;
  }
}

void printMode() 
{
  lcd<<(mode==MD_CHOISIR?F("choisir mode"):mode==MD_PERIODE?F("periodique  "):F("continue    "));
}

void valPlus(uint8_t reg)
{
  switch (reg) {
    case 1 : P_vitesse<10?P_vitesse++:P_vitesse=10; activeMoteur(); break;
    case 2 : P_duree_tourne++; break;
    case 3 : P_duree_arret++; break;
  }
}

void valMoins(uint8_t reg)
{
  switch (reg) {
    case 1 : P_vitesse>1?P_vitesse--:P_vitesse=1; activeMoteur(); break;
    case 2 : P_duree_tourne>1?P_duree_tourne--:P_duree_tourne=1; break;
    case 3 : P_duree_arret>1?P_duree_arret--:P_duree_arret=1; break;
  }
}

void purgeBtn() {
  haut.state();
  bas.state();
  droite.state();
  gauche.state();
  select.state();
}

////////////////////////////MENU/////////////////////////////////////////////////////



// 0132456789012345
// **  BBQduino  **
// appui pour debut

void menu_init()
{
  if(menu.isFirstRun()) {
    moteur.next(mot_arret);
    purgeBtn();
    lcd.clear();
    lcd<<"**  BBQduino  **";    
  }
  
  if(menu.elapsed(1*1000UL)&& !menu.elapsed(3*1000UL)) {
    lcd.setCursor(6,1);
    lcd<<"V1.0";
  }
  
  if(menu.elapsed(3*1000UL)) {
    lcd.setCursor(0,1);
    lcd<<"appui pour debut";
  }
  
  if(select.state(BTN_CLICK)) menu.next(menu_mode);
  
}

// 0132456789012345
// Mode rotation :
//   Choisir mode
//   Continue
//   Periodique


void menu_mode()
{
  if(menu.isFirstRun()) {
    purgeBtn();
    lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Mode rotation :");
  }
  
  if(menu.periodic(500)) {
    lcd.setCursor(2,1); printMode();
  }
  
  if(haut.state(BTN_CLICK)) mode<2?mode++:mode=0;
  if(bas.state(BTN_CLICK)) mode>0?mode--:mode=2;
  
  if(select.state(BTN_CLICK)) 
    switch (mode) {
      case MD_PERIODE : menu.next(menu_periode); moteur.next(mot_perio_on); break;
      case MD_CONTINU : menu.next(menu_continu); moteur.next(mot_continu); break;
      default : break;
    }
}

// 0132456789012345
// Rota periodique
//  vitesse 05
//  temps on 05s
//  temps off 05s
void menu_periode()
{
  static uint8_t reglage=2;
  
  if(menu.isFirstRun()) {
    P_vitesse=10;
    purgeBtn();
    lcd.clear();
    lcd<<"Rota. "; printMode();
  }
  
  if(menu.periodic(500)) {
    lcd.setCursor(1,1); 
    printReglage(reglage);
  }
  
  if(gauche.state(BTN_CLICK)) reglage>1?reglage--:reglage=3;
  if(droite.state(BTN_CLICK)) reglage<3?reglage++:reglage=1;
  
  if(haut.state(BTN_CLICK)) valPlus(reglage);
  if(bas.state(BTN_CLICK)) valMoins(reglage);
  
  if(select.state(BTN_LONGCLICK)) menu.next(menu_init); 
  
}


// Rota continue
//  vitesse 05
void menu_continu()
{
  if(menu.isFirstRun()) {
    purgeBtn();
    lcd.clear();
    lcd<<"Rota. "; printMode();
    lcd.setCursor(1,1);
    printReglage(1);
  }
  
  if(bas.state(BTN_CLICK)) {
    P_vitesse>1?P_vitesse--:P_vitesse=1;
    activeMoteur(); 
    lcd.setCursor(1,1); 
    printReglage(1);
  }
  if(haut.state(BTN_CLICK)) {
    P_vitesse<10?P_vitesse++:P_vitesse=10;
    activeMoteur();
    lcd.setCursor(1,1); 
    printReglage(1);
  }
  
  if(select.state(BTN_LONGCLICK)) menu.next(menu_init);  
  
  if(droite.state(BTN_LONGCLICK) && P_vitesse==1) menu.next(menu_pwm_min);
}

void menu_pwm_min()
{  
  if(menu.isFirstRun()) {
    purgeBtn();
    lcd.clear();
    lcd<<"Valeur mini PWM"; 
    lcd.setCursor(7,1);
    printValeur(P_pwm_min);
  }
  if(bas.state(BTN_CLICK)) {
    P_pwm_min>5?P_pwm_min-=5:P_pwm_min=1;
    activeMoteur(); 
    lcd.setCursor(7,1); 
    printValeur(P_pwm_min);
    lcd<<" ";
  }
  if(haut.state(BTN_CLICK)) {
    P_pwm_min<196?P_pwm_min+=5:P_pwm_min=200;
    activeMoteur();
    lcd.setCursor(7,1); 
    printValeur(P_pwm_min);
    lcd<<" ";
  }
  
  if(select.state(BTN_CLICK)) menu.next(menu_continu); 
}


///////////////////////RETROECLAIRAGE///////////////////////////////////////////////////////////

void retro_on()
{
  if (retro.isFirstRun()) lcd.setBacklight(0x01);
  
  if (retro.elapsed(P_tempoLCD * 1000UL)) retro.next(retro_off);
  
  //lecture des boutons
  uint8_t buttons = lcd.readButtons();
  
  //si il y a eu de l'activité, raz du chrono
  if(buttons) retro.next(retro_on, true);
  
  //et on transmet l'activité à qui de droit
  haut.update(buttons & BUTTON_UP);
  bas.update(buttons & BUTTON_DOWN);
  gauche.update(buttons & BUTTON_LEFT);
  droite.update(buttons & BUTTON_RIGHT);
  select.update(buttons & BUTTON_SELECT);
}

void retro_off()
{
  if (retro.isFirstRun()) {
    lcd.setBacklight(0);
  }
  
  if (lcd.readButtons()) {
    retro.next(retro_on);
  }
}

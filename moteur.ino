void activeMoteur()
{
  if(moteur.isInState(mot_continu)||moteur.isInState(mot_perio_on))
    analogWrite(PIN_OUT,map(P_vitesse,1,10,P_pwm_min,254));
  else analogWrite(PIN_OUT,0);
}

/////////moteur/////////////////////////////

void mot_arret()
{
  if(moteur.isFirstRun()) activeMoteur();
}

void mot_continu()
{
  if(moteur.isFirstRun()) activeMoteur();
}

void mot_perio_on()
{
  if(moteur.isFirstRun()) activeMoteur();
  
  if(moteur.elapsed(P_duree_tourne*1000UL)) moteur.next(mot_perio_off);
}

void mot_perio_off()
{
  if(moteur.isFirstRun()) activeMoteur();
  
  if(moteur.elapsed(P_duree_arret*1000UL)) moteur.next(mot_perio_on);
}


void menu_page()
{
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("MENU");
  lcd.setCursor(0,1);
  lcd.print("prSet");
  lcd.setCursor(11,1);
  lcd.print("build");

  int val = 1024;
  while(val > 1000 && data == 0) // need a pull-up resistor (10KΩ)
  {
    val = analogRead(switchPin);
  }
  
  if (val < 30 || data == 10) //right button pressed => build profile
  {
    #ifdef USE_LCD_KEYPAD_SHIELD
      delay(200);
      profileBuild();
    #endif
  }
    
  else if (val >= 30 || data == 100)
  {    
    while(analogRead(switchPin) < 1000) { delay(100); } // debounce
    delay(100);
    profileSet();
  }

  data = 0;
}

void profileSet()
{
   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lead-free paste?");
  lcd.setCursor(0,1);
  lcd.print("yes");
  lcd.setCursor(14, 1);
  lcd.print("no");

  int val = 1024;

  while(val > 1000 && data == 0) // need a pull-up resistor (10KΩ)
  {
    val = analogRead(switchPin);
    if(Serial.available() > 0)
    {
      digitalWrite(13,HIGH);
      data = Serial.read();
      delay(100);
    }
    delay(100);
    digitalWrite(13,LOW);
    //Serial.println(reflowStatus);
  }

  if (val < 30 || data == 10) //right button pressed => LEADED_PROFILE
  {
    #ifdef USE_LCD_KEYPAD_SHIELD
      delay(200);
    #endif
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 177;
    TEMPERATURE_REFLOW_MAX = 210;
    type = "  [L]";
  }
    
  else if (val >= 30 || data == 100)
  {
    #ifdef USE_LCD_KEYPAD_SHIELD
      delay(200);
    #endif
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 200;
    TEMPERATURE_REFLOW_MAX = 250;
    type = " [LF]";
  }

  data = 0;
  delay(1000);
  
  sendProfile();
}

void sendProfile()
{
  String dataToSend =
    "(" + type + ","
        + TEMPERATURE_SOAK_MIN   + ","
        + TEMPERATURE_SOAK_MAX   + ","
        + TEMPERATURE_REFLOW_MAX + ")";

#ifndef USE_ETHERNET
  char* buf = (char*)malloc(sizeof(char)*dataToSend.length()+1);
  dataToSend.toCharArray(buf, dataToSend.length()+1);
  Serial.println(buf);
  free(buf);
#else
  client.print(dataToSend);
#endif

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T Preheat=");
  lcd.setCursor(11, 0);
  lcd.print(TEMPERATURE_SOAK_MIN);
  lcd.write((uint8_t)0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("T Soak=");
  lcd.setCursor(11, 1);
  lcd.print(TEMPERATURE_SOAK_MAX);
  lcd.write((uint8_t)0);
  lcd.print("C");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T Refl=");
  lcd.setCursor(11, 0);
  lcd.print(TEMPERATURE_REFLOW_MAX);
  lcd.write((uint8_t)0);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("T Cool=");
  lcd.setCursor(11, 1);
  lcd.print(TEMPERATURE_COOL_MIN);
  lcd.write((uint8_t)0);
  lcd.print("C");
  delay(1000);
}

void soundStart() //Roppoppò
{
  tone(buzzerPin, note_g);
  delay(400);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_b);
  delay(200);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_d*2);
  delay(200);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_g);
  delay(400);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_b);
  delay(200);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_d*2);
  delay(200);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_g);
  delay(400);
  noTone(buzzerPin);
}

void soundComplete() //Indiana Jones
{
  tone(buzzerPin, note_cd);
  delay(300);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_cd);
  delay(100);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_ga);
  delay(900);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_ab);
  delay(120);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_ga);
  delay(120);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_fg);
  delay(120);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_ga);
  delay(800);
  noTone(buzzerPin);
}

void soundError() //Imperial March
{
  tone(buzzerPin, note_f);
  delay(500);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_f);
  delay(500);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_f);
  delay(500);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_cd);
  delay(400);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_ga);
  delay(100);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_f);
  delay(500);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_cd);
  delay(400);
  noTone(buzzerPin);
  delay(100);
  tone(buzzerPin, note_ga);
  delay(100);
  noTone(buzzerPin);
  delay(50);
  tone(buzzerPin, note_f);
  delay(800);
  noTone(buzzerPin);
  delay(100);
}

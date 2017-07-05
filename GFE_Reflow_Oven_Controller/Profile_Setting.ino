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
    /*TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 177;
    TEMPERATURE_REFLOW_MAX = 230;*/
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 177;
    TEMPERATURE_REFLOW_MAX = 230;
    type = "  (L)";
  }
    
  else if (val >= 30 || data == 100)
  {
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 200;
    TEMPERATURE_REFLOW_MAX = 250;
    type = " (LF)";
  }

  data = 0;
  delay(1000);
}

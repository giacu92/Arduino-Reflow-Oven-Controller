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

  while(val > 1000) // need a pull-up resistor (10KΩ)
  {
    delay(100);
    val = analogRead(switchPin);
  }

  if (val < 30) //left button pressed => LEADED_PROFILE
  {
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 177;
    TEMPERATURE_REFLOW_MAX = 230;
    type = "      ";
  }
    
  else
  {
    TEMPERATURE_SOAK_MIN = 150;
    TEMPERATURE_SOAK_MAX = 200;
    TEMPERATURE_REFLOW_MAX = 250;
    type = " (LF)";
  }

  delay(1000);
}
